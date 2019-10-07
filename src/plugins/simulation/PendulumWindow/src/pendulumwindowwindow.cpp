/*******************************************************************************

Copyright (C) The University of Auckland

OpenCOR is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OpenCOR is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*******************************************************************************/

//==============================================================================
// Pendulum window
//==============================================================================

#include "borderedwidget.h"
#include "corecliutils.h"
#include "pendulumwindowwindow.h"
#include "widget.h"
#include "zincwidget.h"

//==============================================================================

#include "ui_pendulumwindowwindow.h"

//==============================================================================

#include <QCheckBox>
#include <QDir>
#include <QLabel>
#include <QSlider>

//==============================================================================

#include <array>
#include <iostream>

//==============================================================================

#include "zincbegin.h"
    #include "opencmiss/zinc/fieldarithmeticoperators.hpp"
    #include "opencmiss/zinc/fieldcomposite.hpp"
    #include "opencmiss/zinc/fieldconstant.hpp"
    #include "opencmiss/zinc/fieldcoordinatetransformation.hpp"
    #include "opencmiss/zinc/fieldmodule.hpp"
    #include "opencmiss/zinc/fieldtime.hpp"
    #include "opencmiss/zinc/fieldvectoroperators.hpp"
#include "zincend.h"

//==============================================================================

namespace OpenCOR {
namespace PendulumWindow {

//==============================================================================

PendulumWindowWindow::PendulumWindowWindow(QWidget *pParent) :
    Core::WindowWidget(pParent),
    mGui(new Ui::PendulumWindowWindow),
    mZincContext(nullptr),
    mZincSceneViewerDescription(nullptr),
    mAxesFontPointSize(0),
    mInitialiseZincScene(true),
    mCurrentDataSize(0),
    mTimeValues(nullptr),
    mR0Values(nullptr),
    mQ1Values(nullptr),
    mThetaValues(nullptr)
{
    // Set up the GUI

    mGui->setupUi(this);

    // Create and add a Zinc widget

    mZincWidget = new ZincWidget::ZincWidget(this);

    connect(mZincWidget, SIGNAL(contextAboutToBeDestroyed()),
            this, SLOT(createAndSetZincContext()));
    connect(mZincWidget, SIGNAL(graphicsInitialized()),
            this, SLOT(graphicsInitialized()));
    connect(mZincWidget, SIGNAL(devicePixelRatioChanged(const int &)),
            this, SLOT(devicePixelRatioChanged(const int &)));

    mGui->layout->addWidget(new Core::BorderedWidget(mZincWidget,
                                                     true, true, true, true));

    // Create and add our time label and check box

    Core::Widget *timeWidget = new Core::Widget(QSize(), this);

    timeWidget->createLayout(Core::Widget::Layout::Horizontal);

    mTimeLabel = new QLabel(timeWidget);

    mTimeLabel->setEnabled(false);
    mTimeLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

    timeWidget->layout()->addWidget(mTimeLabel);

    mTimeCheckBox = new QCheckBox(timeWidget);

    mTimeCheckBox->setEnabled(false);
    mTimeCheckBox->setText(tr("Auto"));

    connect(mTimeCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(autoMode()));

    timeWidget->layout()->addWidget(mTimeCheckBox);

    mGui->layout->addWidget(timeWidget);

    // Create and add our time slider

    mTimeSlider = new QSlider(this);

    mTimeSlider->setEnabled(false);
    mTimeSlider->setOrientation(Qt::Horizontal);

    connect(mTimeSlider, SIGNAL(valueChanged(int)),
            this, SLOT(updateScene(const int &)));

    mGui->layout->addWidget(mTimeSlider);

    // Create and set our Zinc context

    createAndSetZincContext();

    // Customise our timer

    connect(&mTimer, SIGNAL(timeout()),
            this, SLOT(updateScene()));
}

//==============================================================================

PendulumWindowWindow::~PendulumWindowWindow()
{
    // Delete some internal objects

    delete mZincContext;

    delete mTimeValues;

    // Delete the GUI

    delete mGui;
}

//==============================================================================

void PendulumWindowWindow::retranslateUi()
{
    // Retranslate our whole window

    mGui->retranslateUi(this);
}

//==============================================================================

void PendulumWindowWindow::createAndSetZincContext()
{
    // Keep track of our current scene viewer's description

    mZincSceneViewerDescription = mZincWidget->sceneViewer().writeDescription();

    // Create and set our Zinc context

    mZincContext = new OpenCMISS::Zinc::Context("PendulumWindowWindow");

    mZincContext->getMaterialmodule().defineStandardMaterials();
    mZincContext->getGlyphmodule().defineStandardGlyphs();

    mZincWidget->setContext(mZincContext);
}

//==============================================================================

void PendulumWindowWindow::initData(const quint64 &pDataSize,
                                    const double &pMinimumTime,
                                    const double &pMaximumTime,
                                    const double &pTimeInterval,
                                    double *pR0Values, double *pQ1Values,
                                    double *pThetaValues)
{
    // Initialise our data
    // Note: mTimeValues must be fully populated for Zinc to work as expected.
    //       However, the list of simulation's results' points is effectively
    //       empty when coming here (see the call to this method from
    //       SimulationExperimentViewWidget::checkSimulationResults()), hence we
    //       we create and populate mTimeValues ourselves...

    mCurrentDataSize = 0;

    delete mTimeValues;

    mTimeValues = new double[pDataSize];

    for (quint64 i = 0; i < pDataSize; ++i) {
        mTimeValues[i] = i*pTimeInterval;
    }

    mR0Values = pR0Values;
    mQ1Values = pQ1Values;
    mThetaValues = pThetaValues;

    // Initialise our Zinc scene, if needed, or reset it

    if (mInitialiseZincScene) {
        mInitialiseZincScene = false;

        // Retrieve the default time keeper

        OpenCMISS::Zinc::Timekeepermodule timeKeeperModule = mZincContext->getTimekeepermodule();

        mTimeKeeper = timeKeeperModule.getDefaultTimekeeper();

        // Get the field module of our default region and do a few things with
        // it

        OpenCMISS::Zinc::Region defaultRegion = mZincContext->getDefaultRegion();

        defaultRegion.readFile("~/mesh.exnode");
        defaultRegion.readFile("~/mesh.exelem");

        mFieldModule = defaultRegion.getFieldmodule();

        mFieldModule.beginChange();

            // Create a single 1D element with only 1D xi coordinates to provide
            // a domain for visualising the coordinates time path

            OpenCMISS::Zinc::Mesh mesh = mFieldModule.findMeshByDimension(1);

        mFieldModule.endChange();

        mFieldModule.beginChange();
            // Create a field looking up the node coordinates at time as a
            // function of element xi
            // Note: Zinc has a known defect in that the xi field doesn't appear
            //       until change caching ends, hence the need to call
            //       endChange() and beginChange() above, to get things to work
            //       as expected...

            OpenCMISS::Zinc::Field coordinates = mFieldModule.findFieldByName("coordinates");

            // Assign parameters at the node for the above fields

            mFieldCache = mFieldModule.createFieldcache();

            // mFieldCache.setNode(node);
        mFieldModule.endChange();

        // Use a fine tessellation with as many divisions as time steps, so that
        // we visualise the time path of coordinates on the element with
        // sufficient resolution

        OpenCMISS::Zinc::Scene scene = defaultRegion.getScene();
        OpenCMISS::Zinc::Tessellationmodule tessellationModule = scene.getTessellationmodule();
        OpenCMISS::Zinc::Tessellation tessellation = tessellationModule.createTessellation();

        std::array<const int, 1> tessellationData = { int(pDataSize) };

        tessellation.setMinimumDivisions(1, tessellationData.data());

        // Also increase the circle divisions quality of the default points
        // tessellation (so the cylinder used for the weights looks better than
        // the default 12 divisions)

        tessellationModule.getDefaultPointsTessellation().setCircleDivisions(36);

        // Now set up some graphics

        scene.beginChange();
            OpenCMISS::Zinc::Materialmodule materialModule = scene.getMaterialmodule();

            // Draw the axes at the origin

            OpenCMISS::Zinc::GraphicsPoints axes = scene.createGraphicsPoints();

            axes.setFieldDomainType(OpenCMISS::Zinc::Field::DOMAIN_TYPE_POINT);

            OpenCMISS::Zinc::Graphicspointattributes pointAttributes = axes.getGraphicspointattributes();

            OpenCMISS::Zinc::Material material = materialModule.createMaterial();

            std::array<double, 3> rgbValues = { 0.0, 0.0, 0.0 };

            material.setAttributeReal3(OpenCMISS::Zinc::Material::ATTRIBUTE_AMBIENT, rgbValues.data());
            material.setAttributeReal3(OpenCMISS::Zinc::Material::ATTRIBUTE_DIFFUSE, rgbValues.data());

            axes.setMaterial(material);

            // Make a thin cylinder glyph representing the string of the
            // pendulum, which starts from rcOrigin extending in the direction
            // and magnitude of delta

            OpenCMISS::Zinc::GraphicsPoints string = scene.createGraphicsPoints();

            string.setFieldDomainType(OpenCMISS::Zinc::Field::DOMAIN_TYPE_NODES);
            string.setCoordinateField(coordinates);

            pointAttributes = string.getGraphicspointattributes();

            pointAttributes.setGlyphShapeType(OpenCMISS::Zinc::Glyph::SHAPE_TYPE_SPHERE);

            string.setMaterial(materialModule.findMaterialByName("silver"));

            // Draw the time path of the pendulum coordinates

            OpenCMISS::Zinc::GraphicsLines path = scene.createGraphicsLines();

            path.setCoordinateField(coordinates);
            path.setTessellation(tessellation);

            material = materialModule.createMaterial();

            rgbValues[1] = 0.445;
            rgbValues[2] = 0.738;

            material.setAttributeReal3(OpenCMISS::Zinc::Material::ATTRIBUTE_AMBIENT, rgbValues.data());
            material.setAttributeReal3(OpenCMISS::Zinc::Material::ATTRIBUTE_DIFFUSE, rgbValues.data());

            path.setMaterial(material);
        scene.endChange();
    } else {

        static const double zero = 0.0;

        mFieldModule.beginChange();
            for (quint64 i = 0; i < pDataSize; ++i) {
                mFieldCache.setTime(mTimeValues[i]);

                mR0.assignReal(mFieldCache, 1, &zero);
                mQ1.assignReal(mFieldCache, 1, &zero);
                mTheta.assignReal(mFieldCache, 1, &zero);
            }
        mFieldModule.endChange();
    }

    // Set the range of valid times in our default time keeper

    mTimeKeeper.setMinimumTime(pMinimumTime);
    mTimeKeeper.setMaximumTime(pMaximumTime);

    mTimeSlider->setMinimum(int(pMinimumTime/pTimeInterval));
    mTimeSlider->setMaximum(int(pMaximumTime/pTimeInterval));

    // Disable our time-related widgets

    mTimeLabel->setEnabled(false);
    mTimeCheckBox->setEnabled(false);
    mTimeSlider->setEnabled(false);

    mTimeCheckBox->setChecked(false);
    mTimeSlider->setValue(mTimeSlider->minimum());
}

//==============================================================================

void PendulumWindowWindow::addData(const int &pCurrentDataSize)
{
    // Assign the time-varying parameters for mR0, mQ1 and mTheta

    mFieldModule.beginChange();
        for (int i = mCurrentDataSize; i < pCurrentDataSize; ++i) {
            mFieldCache.setTime(mTimeValues[i]);

            mR0.assignReal(mFieldCache, 1, mR0Values+i);
            mQ1.assignReal(mFieldCache, 1, mQ1Values+i);
            mTheta.assignReal(mFieldCache, 1, mThetaValues+i);
        }
    mFieldModule.endChange();

    mCurrentDataSize = pCurrentDataSize;

    // Enable our time-related widgets

    mTimeLabel->setEnabled(true);
    mTimeCheckBox->setEnabled(true);
    mTimeSlider->setEnabled(true);

    mTimeCheckBox->setChecked(pCurrentDataSize-1 == mTimeSlider->maximum());
    mTimeSlider->setValue(pCurrentDataSize-1);
}

//==============================================================================

void PendulumWindowWindow::graphicsInitialized()
{
    // Set our 'new' scene viewer's description

    OpenCMISS::Zinc::Sceneviewer sceneViewer = mZincWidget->sceneViewer();

    // sceneViewer.readDescription(mZincSceneViewerDescription);
    OpenCMISS::Zinc::Region defaultRegion = mZincContext->getDefaultRegion();
    sceneViewer.setScene(defaultRegion.getScene());
    // Our Zinc widget has had its graphics initialised, so now we can set its
    // background colour

    std::array<double, 4> backgroundColor = { 1.0, 1.0, 1.0, 1.0 };

    sceneViewer.setBackgroundColourRGBA(backgroundColor.data());
    sceneViewer.viewAll();

    // Our initial look at and eye positions, and up vector

    sceneViewer.setViewingVolume(-1.922499, 1.922499, -1.922499, 1.922499, 0.632076, 22.557219);

    std::array<const double, 3> lookAtPosition = { 0.612522, -0.044677, 0.000000 };
    std::array<const double, 3> eyePosition = { 0.612522, -0.044677, 7.469910 };
    std::array<const double, 3> upVector = { -1.000000, 0.000000, 0.000000 };

    sceneViewer.setLookatPosition(lookAtPosition.data());
    sceneViewer.setEyePosition(eyePosition.data());
    sceneViewer.setUpVector(upVector.data());
}

//==============================================================================

void PendulumWindowWindow::devicePixelRatioChanged(const int &pDevicePixelRatio)
{
    // Update our scene using the given devide pixel ratio

    OpenCMISS::Zinc::Scene scene = mZincContext->getDefaultRegion().getScene();

    scene.beginChange();
        scene.createGraphicsPoints().getGraphicspointattributes().getFont().setPointSize(pDevicePixelRatio*mAxesFontPointSize);
    scene.endChange();
}

//==============================================================================

void PendulumWindowWindow::updateScene(const int &pTime)
{
    // Update our scene

    double time = 0.01*pTime;

    mTimeLabel->setText(tr("Time: %1 s").arg(time));

    mTimeKeeper.setTime(time);

    // Retrieve the viewing volume, as well as look at and eye positions, and
    // the vector
    // Note: this is so that we can customise the way we want our pendulum scene
    //       to look...

//#define CAN_CUSTOMIZE

#ifdef CAN_CUSTOMIZE
    OpenCMISS::Zinc::Sceneviewer sceneViewer = mZincWidget->sceneViewer();

    double left, right, bottom, top, nearPlane, farPlane;

    sceneViewer.getViewingVolume(&left, &right, &bottom, &top, &nearPlane, &farPlane);

    qDebug("---------");
    qDebug("sceneViewer.setViewingVolume(%f, %f, %f, %f, %f, %f);\n", left, right, bottom, top, nearPlane, farPlane);

    double lookAtPosition[3];
    double eyePosition[3];
    double upVector[3];

    sceneViewer.getLookatPosition(lookAtPosition);
    sceneViewer.getEyePosition(eyePosition);
    sceneViewer.getUpVector(upVector);

    qDebug("const double lookAtPosition[] = { %f, %f, %f };", lookAtPosition[0], lookAtPosition[1], lookAtPosition[2]);
    qDebug("const double eyePosition[] = { %f, %f, %f };", eyePosition[0], eyePosition[1], eyePosition[2]);
    qDebug("const double upVector[] = { %f, %f, %f };", upVector[0], upVector[1], upVector[2]);
#endif
}

//==============================================================================

void PendulumWindowWindow::updateScene()
{
    // Update our scene

    int value = mTimeSlider->value();

    if (value == mTimeSlider->maximum()) {
        value = 0;
    } else {
        ++value;
    }

    mTimeSlider->setValue(value);
}

//==============================================================================

void PendulumWindowWindow::autoMode()
{
    // Enable/disable our timer

    if (mTimeCheckBox->isChecked()) {
        mTimer.start();
    } else {
        mTimer.stop();
    }
}

//==============================================================================

}   // namespace PendulumWindow
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
