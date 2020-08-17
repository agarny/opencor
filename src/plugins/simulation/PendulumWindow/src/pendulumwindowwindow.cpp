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

//==============================================================================

#include "zincbegin.h"
    #include "opencmiss/zinc/fieldarithmeticoperators.hpp"
    #include "opencmiss/zinc/fieldcomposite.hpp"
    #include "opencmiss/zinc/fieldconstant.hpp"
    #include "opencmiss/zinc/fieldcoordinatetransformation.hpp"
    #include "opencmiss/zinc/fieldmodule.hpp"
    #include "opencmiss/zinc/fieldtime.hpp"
#include "zincend.h"

//==============================================================================

namespace OpenCOR {
namespace PendulumWindow {

//==============================================================================

PendulumWindowWindow::PendulumWindowWindow(QWidget *pParent) :
    Core::WindowWidget(pParent),
    mGui(new Ui::PendulumWindowWindow)
{
    // Set up the GUI

    mGui->setupUi(this);

    // Create and add a Zinc widget

    mZincWidget = new ZincWidget::ZincWidget(this);

    connect(mZincWidget, &ZincWidget::ZincWidget::contextAboutToBeDestroyed,
            this, &PendulumWindowWindow::createAndSetZincContext);
    connect(mZincWidget, &ZincWidget::ZincWidget::graphicsInitialized,
            this, &PendulumWindowWindow::graphicsInitialized);
    connect(mZincWidget, &ZincWidget::ZincWidget::devicePixelRatioChanged,
            this, &PendulumWindowWindow::devicePixelRatioChanged);

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

    connect(mTimeCheckBox, &QCheckBox::toggled,
            this, &PendulumWindowWindow::autoMode);

    timeWidget->layout()->addWidget(mTimeCheckBox);

    mGui->layout->addWidget(timeWidget);

    // Create and add our time slider

    mTimeSlider = new QSlider(this);

    mTimeSlider->setEnabled(false);
    mTimeSlider->setOrientation(Qt::Horizontal);

    connect(mTimeSlider, &QSlider::valueChanged,
            this, &PendulumWindowWindow::timeSliderValueChanged);

    mGui->layout->addWidget(mTimeSlider);

    // Create and set our Zinc context

    createAndSetZincContext();

    // Customise our timer

    connect(&mTimer, &QTimer::timeout,
            this, &PendulumWindowWindow::timerTimeOut);
}

//==============================================================================

PendulumWindowWindow::~PendulumWindowWindow()
{
    // Keep track of the fact that we are shutting down

    mShuttingDown = true;

    // Delete some internal objects

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
    // Make sure that we are not shutting down (i.e. skip the case where we are
    // coming here as a result of closing OpenCOR)

    if (mShuttingDown) {
        return;
    }

    // Keep track of our current scene viewer's description

    mZincSceneViewerDescription = mZincWidget->sceneViewer().writeDescription();

    // Create and set our Zinc context

    mZincContext = OpenCMISS::Zinc::Context("PendulumWindowWindow");

    mZincContext.getMaterialmodule().defineStandardMaterials();
    mZincContext.getGlyphmodule().defineStandardGlyphs();

    mZincWidget->setContext(mZincContext);

    // Retrieve our axes' font point size

    mAxesFontPointSize = mZincContext.getDefaultRegion().getScene().createGraphicsPoints().getGraphicspointattributes().getFont().getPointSize();

    // Update our scene using our initial device pixel ratio

    devicePixelRatioChanged(mZincWidget->devicePixelRatio());

    // Use any cached data that we may have

    useCachedData();
}

//==============================================================================

void PendulumWindowWindow::initializeZincScene(int pDataSize)
{
    // Retrieve the default time keeper

    OpenCMISS::Zinc::Timekeepermodule timeKeeperModule = mZincContext.getTimekeepermodule();

    mTimeKeeper = timeKeeperModule.getDefaultTimekeeper();

    // Get the field module of our default region and do a few things with
    // it

    OpenCMISS::Zinc::Region defaultRegion = mZincContext.getDefaultRegion();

    mFieldModule = defaultRegion.getFieldmodule();

    mFieldModule.beginChange();
        // Declare our stored finite element fields

        mR0 = mFieldModule.createFieldFiniteElement(1);
        mQ1 = mFieldModule.createFieldFiniteElement(1);
        mTheta = mFieldModule.createFieldFiniteElement(1);

        // Defining fields as functions of other fields

        OpenCMISS::Zinc::FieldAdd r = mFieldModule.createFieldAdd(mR0, mQ1);

        // Define cylindrical polar coordinates

        std::array<OpenCMISS::Zinc::Field, 2> coordinatesData = { r, mTheta };
        OpenCMISS::Zinc::FieldConcatenate coordinates = mFieldModule.createFieldConcatenate(2, coordinatesData.data());

        coordinates.setCoordinateSystemType(OpenCMISS::Zinc::Field::COORDINATE_SYSTEM_TYPE_CYLINDRICAL_POLAR);

        // Define a constant field at the (default rectangular cartesian)
        // origin

        std::array<const double, 3> rcOriginData = { 0.0, 0.0, 0.0 };

        OpenCMISS::Zinc::FieldConstant rcOrigin = mFieldModule.createFieldConstant(3, rcOriginData.data());

        // Define a field converting the polar coordinates to rectangular
        // cartesian

        OpenCMISS::Zinc::FieldCoordinateTransformation rcCoordinates = mFieldModule.createFieldCoordinateTransformation(coordinates);

        rcCoordinates.setCoordinateSystemType(OpenCMISS::Zinc::Field::COORDINATE_SYSTEM_TYPE_RECTANGULAR_CARTESIAN);

        // Get the difference from rcCoordinates to rcOrigin

        OpenCMISS::Zinc::FieldSubtract delta = mFieldModule.createFieldSubtract(rcCoordinates, rcOrigin);

        // Create a single node with storage for time-varying mR0, mQ1 and
        // mTheta

        OpenCMISS::Zinc::Timesequence timeSequence = mFieldModule.getMatchingTimesequence(pDataSize, mTimeValues);
        OpenCMISS::Zinc::Nodeset nodeSet = mFieldModule.findNodesetByFieldDomainType(OpenCMISS::Zinc::Field::DOMAIN_TYPE_NODES);
        OpenCMISS::Zinc::Nodetemplate nodeTemplate = nodeSet.createNodetemplate();

        nodeTemplate.defineField(mR0);
        nodeTemplate.defineField(mQ1);
        nodeTemplate.defineField(mTheta);

        nodeTemplate.setTimesequence(mQ1, timeSequence);
        nodeTemplate.setTimesequence(mTheta, timeSequence);

        // Create a single node with the above field definitions

        OpenCMISS::Zinc::Node node = nodeSet.createNode(1, nodeTemplate);

        // Create a single 1D element with only 1D xi coordinates to provide
        // a domain for visualising the coordinates time path

        OpenCMISS::Zinc::Mesh mesh = mFieldModule.findMeshByDimension(1);
        OpenCMISS::Zinc::Elementtemplate elementTemplate = mesh.createElementtemplate();

        elementTemplate.setElementShapeType(OpenCMISS::Zinc::Element::SHAPE_TYPE_LINE);

        mesh.createElement(1, elementTemplate);
    mFieldModule.endChange();

    mFieldModule.beginChange();
        // Create a field looking up the node coordinates at time as a function
        // of element xi
        // Note: Zinc has a known defect in that the xi field doesn't appear
        //       until change caching ends, hence the need to call
        //       endChange() and beginChange() above, to get things to work
        //       as expected...

        OpenCMISS::Zinc::Field xi = mFieldModule.findFieldByName("xi");
        OpenCMISS::Zinc::FieldComponent xi1 = mFieldModule.createFieldComponent(xi, 1);

        // Fixed scale factor to work for the entire range of times
        // Note: if we are reading times during solution, we could
        //       dynamically change it (and the fine tessellation below)...

        std::array<const double, 1> constantData = { 100.0 };

        OpenCMISS::Zinc::FieldConstant fieldConstant = mFieldModule.createFieldConstant(1, constantData.data());
        OpenCMISS::Zinc::FieldMultiply xi1Time = mFieldModule.createFieldMultiply(xi1, fieldConstant);

        // xiCoordinates returns node's value of rcCoordinates at the
        // current time on any other domain

        OpenCMISS::Zinc::FieldNodeLookup nodeCoordinates = mFieldModule.createFieldNodeLookup(rcCoordinates, node);

        // xi1TimeNodeCoordinates converts the time variation to be spatial,
        // showing the values of nodeCoordinates at xi1Time

        OpenCMISS::Zinc::FieldTimeLookup xi1TimeNodeCoordinates = mFieldModule.createFieldTimeLookup(nodeCoordinates, xi1Time);

        // Assign parameters at the node for the above fields

        mFieldCache = mFieldModule.createFieldcache();

        mFieldCache.setNode(node);
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

        std::array<double, 3> pointAttributesData = { 1.0, 1.0, 1.0 };

        pointAttributes.setGlyphShapeType(OpenCMISS::Zinc::Glyph::SHAPE_TYPE_AXES_XYZ);
        pointAttributes.setBaseSize(3, pointAttributesData.data());

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
        string.setCoordinateField(rcOrigin);

        pointAttributes = string.getGraphicspointattributes();

        pointAttributes.setGlyphShapeType(OpenCMISS::Zinc::Glyph::SHAPE_TYPE_CYLINDER);
        pointAttributes.setOrientationScaleField(delta);

        pointAttributesData[0] = 0.0;
        pointAttributesData[1] = 0.05;
        pointAttributesData[2] = 0.05;

        pointAttributes.setBaseSize(3, pointAttributesData.data());

        pointAttributesData[0] = 1.0;
        pointAttributesData[1] = 0.0;
        pointAttributesData[2] = 0.0;

        pointAttributes.setScaleFactors(3, pointAttributesData.data());

        string.setMaterial(materialModule.findMaterialByName("silver"));

        // Make a cylinder to represent the weight

        OpenCMISS::Zinc::GraphicsPoints weight = scene.createGraphicsPoints();

        weight.setFieldDomainType(OpenCMISS::Zinc::Field::DOMAIN_TYPE_NODES);
        weight.setCoordinateField(rcCoordinates);

        pointAttributes = weight.getGraphicspointattributes();

        pointAttributes.setGlyphShapeType(OpenCMISS::Zinc::Glyph::SHAPE_TYPE_CYLINDER_SOLID);
        pointAttributes.setOrientationScaleField(delta);

        pointAttributesData[0] = 0.5;
        pointAttributesData[1] = 0.5;
        pointAttributesData[2] = 0.5;

        pointAttributes.setBaseSize(3, pointAttributesData.data());

        pointAttributesData[0] = 0.0;
        pointAttributesData[1] = 0.0;
        pointAttributesData[2] = 0.0;

        pointAttributes.setScaleFactors(3, pointAttributesData.data());

        weight.setMaterial(materialModule.findMaterialByName("gold"));

        // Draw the time path of the pendulum coordinates

        OpenCMISS::Zinc::GraphicsLines path = scene.createGraphicsLines();

        path.setCoordinateField(xi1TimeNodeCoordinates);
        path.setTessellation(tessellation);

        material = materialModule.createMaterial();

        rgbValues[1] = 0.445;
        rgbValues[2] = 0.738;

        material.setAttributeReal3(OpenCMISS::Zinc::Material::ATTRIBUTE_AMBIENT, rgbValues.data());
        material.setAttributeReal3(OpenCMISS::Zinc::Material::ATTRIBUTE_DIFFUSE, rgbValues.data());

        path.setMaterial(material);
    scene.endChange();
}

//==============================================================================

void PendulumWindowWindow::initData(quint64 pDataSize, double pMinimumTime,
                                    double pMaximumTime, double pTimeInterval,
                                    double *pR0Values, double *pQ1Values,
                                    double *pThetaValues)
{
    // Initialise our data
    // Note: mTimeValues must be fully populated for Zinc to work as expected.
    //       However, the list of simulation's results' points is effectively
    //       empty when coming here (see the call to this method from
    //       SimulationExperimentViewWidget::checkSimulationResults()), hence we
    //       we create and populate mTimeValues ourselves...

    mDataSize = 0;

    delete mTimeValues;

    mTimeValues = new double[pDataSize];

    for (quint64 i = 0; i < pDataSize; ++i) {
        mTimeValues[i] = double(i)*pTimeInterval;
    }

    mR0Values = pR0Values;
    mQ1Values = pQ1Values;
    mThetaValues = pThetaValues;

    // Initialise our Zinc scene, if needed, or reset it

    if (mNeedZincSceneInitialization) {
        mNeedZincSceneInitialization = false;

        initializeZincScene(int(pDataSize));
    } else {
        // Reset our different fields

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

void PendulumWindowWindow::addData(int pDataSize)
{
    // Assign the time-varying parameters for mR0, mQ1 and mTheta

    mFieldModule.beginChange();
        for (int i = mDataSize; i < pDataSize; ++i) {
            mFieldCache.setTime(mTimeValues[i]);

            mR0.assignReal(mFieldCache, 1, mR0Values+i);
            mQ1.assignReal(mFieldCache, 1, mQ1Values+i);
            mTheta.assignReal(mFieldCache, 1, mThetaValues+i);
        }
    mFieldModule.endChange();

    mDataSize = pDataSize;

    // Enable our time-related widgets, if all the data has been added

    if (pDataSize-1 == mTimeSlider->maximum()) {
        mTimeLabel->setEnabled(true);
        mTimeCheckBox->setEnabled(true);
        mTimeSlider->setEnabled(true);

        mTimeCheckBox->setChecked(true);
    }
}

//==============================================================================

void PendulumWindowWindow::useCachedData()
{
    // (Re)initialise our Zinc scene, if we had one before

    if (!mNeedZincSceneInitialization) {
        initializeZincScene(mDataSize);
    }

    // Use the cached time-varying parameters for mR0, mQ1 and mTheta, if any

    mFieldModule.beginChange();
        for (int i = 0; i < mDataSize; ++i) {
            mFieldCache.setTime(mTimeValues[i]);

            mR0.assignReal(mFieldCache, 1, mR0Values+i);
            mQ1.assignReal(mFieldCache, 1, mQ1Values+i);
            mTheta.assignReal(mFieldCache, 1, mThetaValues+i);
        }
    mFieldModule.endChange();
}

//==============================================================================

void PendulumWindowWindow::graphicsInitialized()
{
    // Set our 'new' scene viewer's description

    OpenCMISS::Zinc::Sceneviewer sceneViewer = mZincWidget->sceneViewer();

    sceneViewer.readDescription(mZincSceneViewerDescription);

    // Our Zinc widget has had its graphics initialised, so now we can set its
    // background colour

    std::array<double, 4> backgroundColor = { 1.0, 1.0, 1.0, 1.0 };

    sceneViewer.setBackgroundColourRGBA(backgroundColor.data());

    // Set our initial look at and eye positions, and up vector, if needed

    if (mNeedZincSceneViewerInitialization) {
        mNeedZincSceneViewerInitialization = false;

        sceneViewer.setViewingVolume(-2.0, 2.0, -2.0, 2.0, 0.5, 22.5);

        std::array<const double, 3> lookAtPosition = { 0.5, 0.0, 0.0 };
        std::array<const double, 3> eyePosition = { 0.0, 0.0, 7.5 };
        std::array<const double, 3> upVector = { -1.0, 0.0, 0.0};

        sceneViewer.setLookatPosition(lookAtPosition.data());
        sceneViewer.setEyePosition(eyePosition.data());
        sceneViewer.setUpVector(upVector.data());
    }
}

//==============================================================================

void PendulumWindowWindow::devicePixelRatioChanged(int pDevicePixelRatio)
{
    // Update our scene using the given device pixel ratio

    OpenCMISS::Zinc::Scene scene = mZincContext.getDefaultRegion().getScene();

    scene.beginChange();
        scene.createGraphicsPoints().getGraphicspointattributes().getFont().setPointSize(pDevicePixelRatio*mAxesFontPointSize);
    scene.endChange();
}

//==============================================================================

void PendulumWindowWindow::timeSliderValueChanged(int pTime)
{
    // Update our scene

    double time = 0.01*pTime;

    mTimeLabel->setText(tr("Time: %1 s").arg(time));

    mTimeKeeper.setTime(time);
}

//==============================================================================

void PendulumWindowWindow::timerTimeOut()
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
