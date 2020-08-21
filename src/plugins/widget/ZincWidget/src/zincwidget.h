/*******************************************************************************

Copyright (C) The University of Auckland

OpenCOR is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OpenCOR is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://gnu.org/licenses>.

*******************************************************************************/

//==============================================================================
// Zinc widget
//==============================================================================

#pragma once

//==============================================================================

#include "commonwidget.h"
#include "zincwidgetglobal.h"

//==============================================================================

#include <QOpenGLWidget>
#include <QTime>

//==============================================================================

#include "zincbegin.h"
    #include "opencmiss/zinc/sceneviewer.hpp"
    #include "opencmiss/zinc/sceneviewerinput.hpp"
#include "zincend.h"

//==============================================================================

class QMenu;

//==============================================================================

namespace OpenCMISS {
namespace Zinc {
    class Context;
} // namespace Zinc
} // namespace OpenCMISS

//==============================================================================

namespace OpenCOR {
namespace ZincWidget {

//==============================================================================

class ZincWidget;

//==============================================================================

class ZincWidgetSceneViewerCallback : public OpenCMISS::Zinc::Sceneviewercallback
{
public:
    explicit ZincWidgetSceneViewerCallback(ZincWidget *pZincWidget);

private:
    ZincWidget *mZincWidget;

    void operator()(const OpenCMISS::Zinc::Sceneviewerevent &pSceneViewerEvent) override;
};

//==============================================================================

class ZINCWIDGET_EXPORT ZincWidget : public QOpenGLWidget,
                                     public Core::CommonWidget
{
    Q_OBJECT

public:
    enum class ProjectionMode {
        Invalid,
        Parallel,
        Perspective
    };

    explicit ZincWidget(QWidget *pParent);

    void reset();

    OpenCMISS::Zinc::Context context() const;
    void setContext(const OpenCMISS::Zinc::Context &pContext);

    QMenu * contextMenu() const;
    void setContextMenu(QMenu *pContextMenu);

    OpenCMISS::Zinc::Sceneviewer sceneViewer() const;

    ProjectionMode projectionMode();
    void setProjectionMode(ProjectionMode pProjectionMode);

    int viewParameters(double *pEye, double *pLookAt, double *pUp,
                       double &pAngle);
    void setViewParameters(double *pEye, double *pLookAt, double *pUp,
                           double &pAngle);

    OpenCMISS::Zinc::Scenefilter sceneFilter();
    void setSceneFilter(const OpenCMISS::Zinc::Scenefilter &pSceneFilter);

    double tumbleRate();
    void setTumbleRate(double pTumbleRate);

    int project(double *pInCoordinates, double *pOutCoordinates);
    int unproject(double *pInCoordinates, double *pOutCoordinates);

    void viewAll();

    double fps() const;

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int pWidth, int pHeight) override;

    void mouseMoveEvent(QMouseEvent *pEvent) override;
    void mousePressEvent(QMouseEvent *pEvent) override;
    void mouseReleaseEvent(QMouseEvent *pEvent) override;
    void wheelEvent(QWheelEvent *pEvent) override;

    QSize sizeHint() const override;

private:
    int mDevicePixelRatio = -1;

    OpenCMISS::Zinc::Context mContext;
    OpenCMISS::Zinc::Sceneviewer mSceneViewer;
    OpenCMISS::Zinc::Sceneviewernotifier mSceneViewerNotifier;

    ZincWidgetSceneViewerCallback mZincWidgetSceneViewerCallback;

    bool mNeedContextMenu = false;
    QMenu *mContextMenu = nullptr;

    QTime mFpsClock;
    int mNbOfFrames = 0;
    double mFps = 0.0;

    OpenCMISS::Zinc::Sceneviewerinput::ButtonType buttonMap(const Qt::MouseButton &pButton) const;
    OpenCMISS::Zinc::Sceneviewerinput::ModifierFlags modifierMap(const Qt::KeyboardModifiers &pModifiers) const;

    void resetFps();

    void checkDevicePixelRatio(bool pForceSettingViewportSize = false);

signals:
    void contextAboutToBeDestroyed();
    void graphicsInitialized();
    void devicePixelRatioChanged(int pDevicePixelRatio);
};

//==============================================================================

} // namespace ZincWidget
} // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
