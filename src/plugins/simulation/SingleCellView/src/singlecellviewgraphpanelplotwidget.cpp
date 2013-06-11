//==============================================================================
// Single cell view graph panel plot widget
//==============================================================================

#include "singlecellviewgraphpanelplotwidget.h"

//==============================================================================

#include <QApplication>
#include <QClipboard>
#include <QDesktopWidget>
#include <QMouseEvent>

//==============================================================================

#include <QtNumeric>

//==============================================================================

#include <float.h>

//==============================================================================

#include "qwt_painter.h"
#include "qwt_plot_canvas.h"
#include "qwt_plot_curve.h"
#include "qwt_plot_directpainter.h"
#include "qwt_plot_grid.h"
#include "qwt_plot_layout.h"
#include "qwt_plot_renderer.h"
#include "qwt_scale_div.h"

//==============================================================================

namespace OpenCOR {
namespace SingleCellView {

//==============================================================================

SingleCellViewGraphPanelPlotCurve::SingleCellViewGraphPanelPlotCurve() :
    QwtPlotCurve()
{
    // Customise it a bit

    setPen(QPen(Qt::darkBlue));
    setRenderHint(QwtPlotItem::RenderAntialiased);
}

//==============================================================================

static const double MinZoomFactor =    1.0;
static const double MaxZoomFactor = 1024.0;

//==============================================================================

SingleCellViewGraphPanelPlotWidget::SingleCellViewGraphPanelPlotWidget(QWidget *pParent) :
    QwtPlot(pParent),
    mCurves(QList<SingleCellViewGraphPanelPlotCurve *>()),
    mInteractive(true),
    mAction(None),
    mOriginPoint(QPoint()),
    mEndPoint(QPoint()),
    mMinX(0.0),
    mMinY(0.0),
    mMaxX(1000.0),
    mMaxY(1000.0),
    mFixedAxisX(false),
    mFixedAxisY(false),
    mCanvasPixmap(QPixmap())
{
    // Get ourselves a direct painter

    mDirectPainter = new QwtPlotDirectPainter(this);

    // Speedup painting on X11 systems
    // Note: this can only be done on X11 systems...

    if (QwtPainter::isX11GraphicsSystem())
        canvas()->setAttribute(Qt::WA_PaintOnScreen, true);

    // Customise ourselves a bit

    setCanvasBackground(Qt::white);

    // We don't want a frame around ourselves

    qobject_cast<QwtPlotCanvas *>(canvas())->setFrameShape(QFrame::NoFrame);

    // Set our axes' local minimum/maximum values

    setLocalMinX(mMinX);
    setLocalMaxX(mMaxX);
    setLocalMinY(mMinY);
    setLocalMaxY(mMaxY);

    // Attach a grid to ourselves

    QwtPlotGrid *grid = new QwtPlotGrid();

    grid->setMajorPen(Qt::gray, 0, Qt::DotLine);

    grid->attach(this);
}

//==============================================================================

SingleCellViewGraphPanelPlotWidget::~SingleCellViewGraphPanelPlotWidget()
{
    // Delete some internal objects

    delete mDirectPainter;
}

//==============================================================================

bool SingleCellViewGraphPanelPlotWidget::eventFilter(QObject *pObject,
                                                     QEvent *pEvent)
{
    // Default handling of the event

    bool res = QwtPlot::eventFilter(pObject, pEvent);

    // We want to handle a double mouse click, but for some reasons to override
    // mouseDoubleClickEvent() doesn't work, so...

    if (pEvent->type() == QEvent::MouseButtonDblClick)
        handleMouseDoubleClickEvent(static_cast<QMouseEvent *>(pEvent));

    // We are all done, so...

    return res;
}

//==============================================================================

void SingleCellViewGraphPanelPlotWidget::handleMouseDoubleClickEvent(QMouseEvent *pEvent)
{
    // Copy the contents of the plot to the clipboard, in case we double-clicked
    // using the left mouse button

    if (pEvent->button() == Qt::LeftButton) {
        // Render the plot to an image

        QwtPlotRenderer renderer;
        QImage image = QImage(size(), QImage::Format_ARGB32_Premultiplied);
        QPainter painter(&image);

        renderer.render(this, &painter, rect());

        // Set the image to the clipboard

        QApplication::clipboard()->setImage(image);
    }
}

//==============================================================================

double SingleCellViewGraphPanelPlotWidget::minX() const
{
    // Return our minimum X value

    return mMinX;
}

//==============================================================================

void SingleCellViewGraphPanelPlotWidget::setMinX(const double &pMinX)
{
    // Set our minimum X value

    mMinX = pMinX;
}

//==============================================================================

double SingleCellViewGraphPanelPlotWidget::maxX() const
{
    // Return our maximum X value

    return mMaxX;
}

//==============================================================================

void SingleCellViewGraphPanelPlotWidget::setMaxX(const double &pMaxX)
{
    // Set our maximum X value

    mMaxX = pMaxX;
}

//==============================================================================

double SingleCellViewGraphPanelPlotWidget::minY() const
{
    // Return our minimum Y value

    return mMinY;
}

//==============================================================================

void SingleCellViewGraphPanelPlotWidget::setMinY(const double &pMinY)
{
    // Set our minimum Y value

    mMinY = pMinY;
}

//==============================================================================

double SingleCellViewGraphPanelPlotWidget::maxY() const
{
    // Return our maximum Y value

    return mMaxY;
}

//==============================================================================

void SingleCellViewGraphPanelPlotWidget::setMaxY(const double &pMaxY)
{
    // Set our maximum Y value

    mMaxY = pMaxY;
}

//==============================================================================

void SingleCellViewGraphPanelPlotWidget::setLocalAxis(const int &pAxis,
                                                      const double &pMin,
                                                      const double &pMax)
{
    // Set our local axes
    // Note #1: to use setAxisScale() on its own is not sufficient unless we
    //          were to replot ourselves immediately after, but we don't want to
    //          do that, so instead we also use setAxisScaleDiv() to make sure
    //          that our local axes are indeed taken into account (i.e. we can
    //          retrieve them using localMinX(), localMaxX(), localMinY() and
    //          localMaxY()). Also, we must call setAxisScaleDiv() before
    //          setAxisScale() to make sure that the axis data is not considered
    //          as valid which is important when it comes to plotting
    //          ourselves...
    // Note #2: the way QwtPlot create ticks for an axis means that it cannot
    //          handle an axis that would go from -DBL_MAX to DBL_MAX, even
    //          though it would be a valid axis, so we check that the axis fits
    //          within what we know works fine with QwtPlot...

    static const double DBL_MAX_AXIS = 0.3*DBL_MAX;

    setAxisScaleDiv(pAxis, QwtScaleDiv(qMax(-DBL_MAX_AXIS, pMin),
                                       qMin( DBL_MAX_AXIS, pMax)));
    setAxisScale(pAxis, qMax(-DBL_MAX_AXIS, pMin),
                        qMin( DBL_MAX_AXIS, pMax));
}

//==============================================================================

void SingleCellViewGraphPanelPlotWidget::checkLocalAxisValues(const int &pAxis,
                                                              double &pMin,
                                                              double &pMax,
                                                              const bool &pCanResetMin,
                                                              const bool &pCanResetMax)
{
    // Make sure that the minimum/maximum values of our local axis have a valid
    // zoom factor

    double min = (pAxis == QwtPlot::xBottom)?mMinX:mMinY;
    double max = (pAxis == QwtPlot::xBottom)?mMaxX:mMaxY;
    double zoomFactor = (max-min)/(pMax-pMin);

    if (zoomFactor < MinZoomFactor) {
        // The local axis' zoom factor is too small, so reset our
        // minimum/maximum values no matter what

        pMin = min;
        pMax = max;
    } else if (zoomFactor > MaxZoomFactor) {
        // The local axis' zoom factor is too big, so reset the minimum and/or
        // maximum values, depending what we are allowed to reset

        if (pCanResetMin && !pCanResetMax) {
            pMin = pMax-(max-min)/MaxZoomFactor;
        } else if (!pCanResetMin && pCanResetMax) {
            pMax = pMin+(max-min)/MaxZoomFactor;
        } else if (pCanResetMin && pCanResetMax) {
            double length = (max-min)/MaxZoomFactor;

            pMin = qMax(min, 0.5*(pMin+pMax-length));
            pMax = pMin+length;
        }
    }
}

//==============================================================================

double SingleCellViewGraphPanelPlotWidget::localMinX() const
{
    // Return our local minimum X value

    return axisScaleDiv(QwtPlot::xBottom).lowerBound();
}

//==============================================================================

void SingleCellViewGraphPanelPlotWidget::setLocalMinX(const double &pLocalMinX)
{
    // Set our local minimum X value

    double locMinX = pLocalMinX;
    double locMaxX = localMaxX();

    checkLocalAxisValues(QwtPlot::xBottom, locMinX, locMaxX, true, false);

    setLocalAxis(QwtPlot::xBottom, locMinX, locMaxX);
}

//==============================================================================

double SingleCellViewGraphPanelPlotWidget::localMaxX() const
{
    // Return our local maximum X value

    return axisScaleDiv(QwtPlot::xBottom).upperBound();
}

//==============================================================================

void SingleCellViewGraphPanelPlotWidget::setLocalMaxX(const double &pLocalMaxX)
{
    // Set our local maximum X value

    double locMinX = localMinX();
    double locMaxX = pLocalMaxX;

    checkLocalAxisValues(QwtPlot::xBottom, locMinX, locMaxX, false, true);

    setLocalAxis(QwtPlot::xBottom, locMinX, locMaxX);
}

//==============================================================================

double SingleCellViewGraphPanelPlotWidget::localMinY() const
{
    // Return our local minimum Y value

    return axisScaleDiv(QwtPlot::yLeft).lowerBound();
}

//==============================================================================

void SingleCellViewGraphPanelPlotWidget::setLocalMinY(const double &pLocalMinY)
{
    // Set our local minimum Y value

    double locMinY = pLocalMinY;
    double locMaxY = localMaxY();

    checkLocalAxisValues(QwtPlot::yLeft, locMinY, locMaxY, true, false);

    setLocalAxis(QwtPlot::yLeft, locMinY, locMaxY);
}

//==============================================================================

double SingleCellViewGraphPanelPlotWidget::localMaxY() const
{
    // Return our local maximum Y value

    return axisScaleDiv(QwtPlot::yLeft).upperBound();
}

//==============================================================================

void SingleCellViewGraphPanelPlotWidget::setLocalMaxY(const double &pLocalMaxY)
{
    // Set our local maximum Y value

    double locMinY = localMinY();
    double locMaxY = pLocalMaxY;

    checkLocalAxisValues(QwtPlot::yLeft, locMinY, locMaxY, false, true);

    setLocalAxis(QwtPlot::yLeft, locMinY, locMaxY);
}

//==============================================================================

void SingleCellViewGraphPanelPlotWidget::setFixedAxisX(const bool &pFixedAxisX)
{
    // Specify whether our X axis is to be fixed

    mFixedAxisX = pFixedAxisX;
}

//==============================================================================

void SingleCellViewGraphPanelPlotWidget::setFixedAxisY(const bool &pFixedAxisY)
{
    // Specify whether our Y axis is to be fixed

    mFixedAxisY = pFixedAxisY;
}

//==============================================================================

void SingleCellViewGraphPanelPlotWidget::setInteractive(const bool &pInteractive)
{
    // Specify whether interaction is allowed

    mInteractive = pInteractive;
}

//==============================================================================

void SingleCellViewGraphPanelPlotWidget::checkAnyAxesValues(double &pMinX,
                                                            double &pMaxX,
                                                            double &pMinY,
                                                            double &pMaxY)
{
    // Make sure that the minimum/maximum values of our (local) axes have finite
    // values

    if (!qIsFinite(pMinX))
        pMinX = -DBL_MAX;

    if (!qIsFinite(pMaxX))
        pMaxX = DBL_MAX;

    if (!qIsFinite(pMinY))
        pMinY = -DBL_MAX;

    if (!qIsFinite(pMaxY))
        pMaxY = DBL_MAX;
}

//==============================================================================

void SingleCellViewGraphPanelPlotWidget::setLocalAxes(const double &pLocalMinX,
                                                      const double &pLocalMaxX,
                                                      const double &pLocalMinY,
                                                      const double &pLocalMaxY,
                                                      const bool &pCanReplot,
                                                      const bool &pForceMinMaxValues,
                                                      const bool &pUpdateMinMaxValues,
                                                      const bool &pResetMinMaxValues)
{
    // Update our axes

    double oldLocalMinX = localMinX();
    double oldLocalMaxX = localMaxX();
    double oldLocalMinY = localMinY();
    double oldLocalMaxY = localMaxY();

    double newLocalMinX = pLocalMinX;
    double newLocalMaxX = pLocalMaxX;
    double newLocalMinY = pLocalMinY;
    double newLocalMaxY = pLocalMaxY;

    // Retrieve the bounding rectangle for all our curves (but only for those
    // that have some data)

    QRectF boundingRect = QRectF();

    foreach (SingleCellViewGraphPanelPlotCurve *curve, mCurves)
        if (curve->dataSize())
            boundingRect |= curve->boundingRect();

    // Update the minimum/maximum values of our axes, should we have retrieved a
    // valid bounding rectangle

    if (boundingRect != QRectF()) {
        // Optimise our bounding rectangle by first retrieving the
        // minimum/maximum values of our axes

        double minX = boundingRect.left();
        double maxX = boundingRect.right();
        double minY = boundingRect.top();
        double maxY = boundingRect.bottom();

        // Make sure that the minimum/maximum values of our axes have finite
        // values

        checkAnyAxesValues(minX, maxX, minY, maxY);

        // Optimise the minimum/maximum values of our axes by rounding them
        // down/up, respectively

        double powerX = qPow(10.0, qMax(minX?qFloor(log10(qAbs(minX))):0, maxX?qFloor(log10(qAbs(maxX))):0));
        double powerY = qPow(10.0, qMax(minY?qFloor(log10(qAbs(minY))):0, maxY?qFloor(log10(qAbs(maxY))):0));

        minX = powerX*qFloor(minX/powerX);
        maxX = powerX*qCeil(maxX/powerX);
        minY = powerY*qFloor(minY/powerY);
        maxY = powerY*qCeil(maxY/powerY);

        // Make sure that the optimised minimum/maximum values of our axes have
        // finite values

        checkAnyAxesValues(minX, maxX, minY, maxY);

        // Update the minimum/maximum values of our axes, if required

        if (!mFixedAxisX) {
            if (pResetMinMaxValues) {
                mMinX = minX;
                mMaxX = maxX;
            } else if (pUpdateMinMaxValues) {
                mMinX = qMin(mMinX, minX);
                mMaxX = qMax(mMaxX, maxX);
            }
        }

        if (!mFixedAxisY) {
            if (pResetMinMaxValues) {
                mMinY = minY;
                mMaxY = maxY;
            } else if (pUpdateMinMaxValues) {
                mMinY = qMin(mMinY, minY);
                mMaxY = qMax(mMaxY, maxY);
            }
        }
    }

    // Make sure that the new minimum/maximum values of our local axes fit
    // within the minimum/maximum values of our axes

    if (pForceMinMaxValues) {
        newLocalMinX = mMinX;
        newLocalMaxX = mMaxX;
        newLocalMinY = mMinY;
        newLocalMaxY = mMaxY;
    } else {
        newLocalMinX = qMax(newLocalMinX, mMinX);
        newLocalMaxX = qMin(newLocalMaxX, mMaxX);
        newLocalMinY = qMax(newLocalMinY, mMinY);
        newLocalMaxY = qMin(newLocalMaxY, mMaxY);
    }

    // Make sure that the new minimum/maximum values of our local axes have
    // finite values

    checkAnyAxesValues(newLocalMinX, newLocalMaxX, newLocalMinY, newLocalMaxY);

    // Make sure that the new minimum/maximum values of our local axes have a
    // valid zoom factor

    checkLocalAxisValues(QwtPlot::xBottom, newLocalMinX, newLocalMaxX);
    checkLocalAxisValues(QwtPlot::yLeft, newLocalMinY, newLocalMaxY);

    // Update the minimum/maximum values of our local axes, if needed

    bool needReplot = false;

    if (newLocalMinX != oldLocalMinX) {
        setLocalMinX(newLocalMinX);

        needReplot = true;
    }

    if (newLocalMaxX != oldLocalMaxX) {
        setLocalMaxX(newLocalMaxX);

        needReplot = true;
    }

    if (newLocalMinY != oldLocalMinY) {
        setLocalMinY(newLocalMinY);

        needReplot = true;
    }

    if (newLocalMaxY != oldLocalMaxY) {
        setLocalMaxY(newLocalMaxY);

        needReplot = true;
    }

    // Replot ourselves, if needed and allowed

    if (needReplot && pCanReplot)
        replotNow();
}

//==============================================================================

void SingleCellViewGraphPanelPlotWidget::scaleLocalAxes(const double &pScalingFactorX,
                                                        const double &pScalingFactorY)
{
    // Determine the local minimum/maximum values of our two axes

    double rangeX = localMaxX()-localMinX();
    double centerX = localMinX()+0.5*rangeX;
    double rangeOverTwoX = 0.5*pScalingFactorX*rangeX;

    double rangeY = localMaxY()-localMinY();
    double centerY = localMinY()+0.5*rangeY;
    double rangeOverTwoY = 0.5*pScalingFactorY*rangeY;

    // Rescale our two local axes

    setLocalAxes(centerX-rangeOverTwoX, centerX+rangeOverTwoX,
                 centerY-rangeOverTwoY, centerY+rangeOverTwoY);
}

//==============================================================================

void SingleCellViewGraphPanelPlotWidget::checkLocalAxes(const bool &pCanReplot,
                                                        const bool &pForceMinMaxValues,
                                                        const bool &pUpdateMinMaxValues)
{
    // Check our local axes by trying to set them

    setLocalAxes(localMinX(), localMaxX(), localMinY(), localMaxY(),
                 pCanReplot, pForceMinMaxValues, pUpdateMinMaxValues);
}

//==============================================================================

void SingleCellViewGraphPanelPlotWidget::resetLocalAxes(const bool &pCanReplot)
{
    // Reset our local axes by trying to set them

    setLocalAxes(0.0, 0.0, 0.0, 0.0, pCanReplot, true, false, true);
}

//==============================================================================

static const double NoScalingFactor  = 1.0;
static const double ScalingInFactor  = 0.57;
static const double ScalingOutFactor = 1.0/ScalingInFactor;

//==============================================================================

QPoint SingleCellViewGraphPanelPlotWidget::mousePositionWithinCanvas(const QPoint &pPoint) const
{
    // Return the mouse position relative to our canvas, after making sure that
    // its mapped values are within our local ranges

    QPoint res = pPoint-plotLayout()->canvasRect().toRect().topLeft();

    QwtScaleMap canvasMapX = canvasMap(QwtPlot::xBottom);
    QwtScaleMap canvasMapY = canvasMap(QwtPlot::yLeft);

    double x = qMin(localMaxX(), qMax(localMinX(), canvasMapX.invTransform(res.x())));
    double y = qMin(localMaxY(), qMax(localMinY(), canvasMapY.invTransform(res.y())));

    res.setX(canvasMapX.transform(x));
    res.setY(canvasMapY.transform(y));

    return res;
}

//==============================================================================

void SingleCellViewGraphPanelPlotWidget::mouseMoveEvent(QMouseEvent *pEvent)
{
    // Default handling of the event

    QwtPlot::mouseMoveEvent(pEvent);

    // Check that interaction is allowed

    if (!mInteractive)
        return;

    // Retrieve the current point

    QPoint currentPoint = mousePositionWithinCanvas(pEvent->pos());

    // Carry out the action

    switch (mAction) {
    case Pan: {
        // Determine the X/Y shifts for the panning

        QwtScaleMap canvasMapX = canvasMap(QwtPlot::xBottom);
        QwtScaleMap canvasMapY = canvasMap(QwtPlot::yLeft);

        double shiftX = canvasMapX.invTransform(currentPoint.x())-canvasMapX.invTransform(mOriginPoint.x());
        double shiftY = canvasMapY.invTransform(currentPoint.y())-canvasMapY.invTransform(mOriginPoint.y());

        // Determine our new local minimum/maximum values for our axes

        double newLocalMinX = localMinX()-shiftX;
        double newLocalMaxX = localMaxX()-shiftX;
        double newLocalMinY = localMinY()-shiftY;
        double newLocalMaxY = localMaxY()-shiftY;

        // Make sure that our new local minimum/maximum values for our axes
        // are within our local minimum/maximum values

        if (newLocalMinX < minX()) {
            newLocalMinX = minX();
            newLocalMaxX = newLocalMinX+localMaxX()-localMinX();
        } else if (newLocalMaxX > maxX()) {
            newLocalMaxX = maxX();
            newLocalMinX = newLocalMaxX-localMaxX()+localMinX();
        }

        if (newLocalMinY < minY()) {
            newLocalMinY = minY();
            newLocalMaxY = newLocalMinY+localMaxY()-localMinY();
        } else if (newLocalMaxY > maxY()) {
            newLocalMaxY = maxY();
            newLocalMinY = newLocalMaxY-localMaxY()+localMinY();
        }

        // Set our new local minimum/maximum values for our local axes which
        // will replot ourselves as a result

        setLocalAxes(newLocalMinX, newLocalMaxX, newLocalMinY, newLocalMaxY);

        break;
    }
    case ShowCoordinates:
        // Show the coordinates by simply replotting ourselves

        replotNow();

        break;
    case Zoom: {
        // Rescale ourselves (which will replot ourselves as a result)

        int deltaX = currentPoint.x()-mOriginPoint.x();
        int deltaY = currentPoint.y()-mOriginPoint.y();

        scaleLocalAxes(deltaX?
                           (deltaX > 0)?
                               ScalingInFactor:
                               ScalingOutFactor:
                           NoScalingFactor,
                       deltaY?
                           (deltaY > 0)?
                               ScalingInFactor:
                               ScalingOutFactor:
                           NoScalingFactor);

        break;
    }
    case ZoomRegion:
        // Draw our zoom region by updating our end point and then replotting
        // ourselves

        mEndPoint = mousePositionWithinCanvas(pEvent->pos());

        replotNow();

        break;
    default:
        // None

        ;
    }

    // Reset our point of origin, but only if we are doing something and it's
    // not zooming a region

    if ((mAction != None) && (mAction != ZoomRegion))
        mOriginPoint = mousePositionWithinCanvas(pEvent->pos());
}

//==============================================================================

void SingleCellViewGraphPanelPlotWidget::mousePressEvent(QMouseEvent *pEvent)
{
    // Default handling of the event

    QwtPlot::mousePressEvent(pEvent);

    // Check that interaction is allowed

    if (!mInteractive)
        return;

    // Check that the position of the mouse is over our canvas

    if (!plotLayout()->canvasRect().contains(pEvent->pos()))
        return;

    // Check which action we can carry out

    if (   (pEvent->button() == Qt::LeftButton)
        && (pEvent->modifiers() == Qt::NoModifier)) {
        // We want to pan

        mAction = Pan;
    } else if (   (pEvent->button() == Qt::LeftButton)
               && (pEvent->modifiers() == Qt::ShiftModifier)) {
        // We want to show the coordinates

        mAction = ShowCoordinates;
    } else if (   (pEvent->button() == Qt::RightButton)
               && (pEvent->modifiers() == Qt::NoModifier)) {
        // We want to zoom in/out

        mAction = Zoom;
    } else if (   (pEvent->button() == Qt::RightButton)
               && (pEvent->modifiers() == Qt::ControlModifier)) {
        // We want to zoom a region, but we can only do this if we are not
        // already fully zoomed in

        if (   ((mMaxX-mMinX)/(localMaxX()-localMinX()) < MaxZoomFactor)
            || ((mMaxY-mMinY)/(localMaxY()-localMinY()) < MaxZoomFactor)) {
            mAction = ZoomRegion;
        } else {
            // We are already fully zoomed in, so...

            mAction = None;

            return;
        }
    } else {
        // We cannot carry out any action, so check whether we need to replot
        // ourselves (in case we were in the middle of carrying out a visual
        // action), make sure that we have no action to carry out, replot
        // ourselves if needed, and then leave

        bool needReplotNow;

        switch (mAction) {
        case ShowCoordinates:
        case ZoomRegion:
            needReplotNow = true;

            break;
        default:
            needReplotNow = false;
        }

        mAction = None;

        if (needReplotNow)
            replotNow();

        return;
    }

    // Retrieve a pixmap version of our canvas, if needed

    if ((mAction == ShowCoordinates) || (mAction == ZoomRegion))
        mCanvasPixmap = grab(plotLayout()->canvasRect().toRect());

    // Keep track of the mouse position

    mOriginPoint = mousePositionWithinCanvas(pEvent->pos());
}

//==============================================================================

void SingleCellViewGraphPanelPlotWidget::mouseReleaseEvent(QMouseEvent *pEvent)
{
    // Default handling of the event

    QwtPlot::mouseReleaseEvent(pEvent);

    // Check that interaction is allowed

    if (!mInteractive)
        return;

    // Check whether we need to carry out an action

    if (mAction == None)
        return;

    // Keep track of the action to carry out
    // Note: this is so that we can reset mAction while still being able to
    //       finish carrying out the action...

    Action action = mAction;

    // We are done carrying out an action, so...

    mAction = None;

    // Finish carrying out the action

    switch (action) {
    case ShowCoordinates:
        // Remove the coordinates by replotting ourselves

        replotNow();

        break;
    case ZoomRegion: {
        // Zoom our region

        QwtScaleMap canvasMapX = canvasMap(QwtPlot::xBottom);
        QwtScaleMap canvasMapY = canvasMap(QwtPlot::yLeft);

        setLocalAxes(canvasMapX.invTransform(qMin(mOriginPoint.x(), mEndPoint.x())),
                     canvasMapX.invTransform(qMax(mOriginPoint.x(), mEndPoint.x())),
                     canvasMapY.invTransform(qMax(mOriginPoint.y(), mEndPoint.y())),
                     canvasMapY.invTransform(qMin(mOriginPoint.y(), mEndPoint.y())));

        break;
    }
    default:
        // Another action which needs nothing more to be done

        ;
    }
}

//==============================================================================

void SingleCellViewGraphPanelPlotWidget::wheelEvent(QWheelEvent *pEvent)
{
    // Default handling of the event

    QwtPlot::wheelEvent(pEvent);

    // Check that interaction is allowed

    if (!mInteractive)
        return;

    // The only action we support using the wheel is zooming in/out, but this
    // requires no modifiers being used

    if (pEvent->modifiers() != Qt::NoModifier)
        return;

    // Make sure that there is a need to zoom in/out

    if (!pEvent->delta())
        return;

    // Zoom in/out by scaling our two local axes

    double scalingFactor = (pEvent->delta() > 0)?ScalingInFactor:ScalingOutFactor;

    scaleLocalAxes(scalingFactor, scalingFactor);
}

//==============================================================================

void SingleCellViewGraphPanelPlotWidget::replotNow()
{
    // Replot ourselves

    replot();

    // Make sure that the replotting occurs immediately
    // Note: this is needed when running a simulation since, otherwise,
    //       replotting won't occur immediately (because of threading)...

    qApp->processEvents();
}

//==============================================================================

void SingleCellViewGraphPanelPlotWidget::drawCoordinates(QPainter *pPainter,
                                                         const QPoint &pCoordinates,
                                                         const QColor &pBackgroundColor,
                                                         const QColor &pForegroundColor,
                                                         const Location &pLocation,
                                                         const bool &pCanMoveLocation)
{
    // Retrieve the size of coordinates as they will appear on the screen,
    // which means using the same font as the one used for the axes

    pPainter->setFont(axisFont(QwtPlot::xBottom));

    QString coords = QString("(%1, %2)").arg(QString::number(canvasMap(QwtPlot::xBottom).invTransform(pCoordinates.x())),
                                             QString::number(canvasMap(QwtPlot::yLeft).invTransform(pCoordinates.y())));
    QRect desktopGeometry = qApp->desktop()->availableGeometry();
    QRectF coordsRect = pPainter->boundingRect(QRectF(0.0, 0.0, desktopGeometry.width(), desktopGeometry.height()), coords);

    // Determine where the coordinates and its background should be drawn

    switch (pLocation) {
    case TopLeft:
        coordsRect.moveTo(pCoordinates.x()-coordsRect.right()-1,
                          pCoordinates.y()-coordsRect.bottom()-1);

        break;
    case TopRight:
        coordsRect.moveTo(pCoordinates.x()+2,
                          pCoordinates.y()-coordsRect.bottom()-1);

        break;
    case BottomLeft:
        coordsRect.moveTo(pCoordinates.x()-coordsRect.right()-1,
                          pCoordinates.y()+2);

        break;
    case BottomRight:
        coordsRect.moveTo(pCoordinates.x()+2,
                          pCoordinates.y()+2);

        break;
    }

    if (pCanMoveLocation) {
        if (coordsRect.top() < 0)
            coordsRect.moveTop(pCoordinates.y()+2);

        if (coordsRect.left() < 0)
            coordsRect.moveLeft(pCoordinates.x()+2);

        if (coordsRect.bottom() > plotLayout()->canvasRect().height())
            coordsRect.moveTop(pCoordinates.y()-coordsRect.height()-1);

        if (coordsRect.right() > plotLayout()->canvasRect().width())
            coordsRect.moveLeft(pCoordinates.x()-coordsRect.width()-1);
    }

    // Draw a filled rectangle to act as the background of the coordinates
    // we are to show

    pPainter->fillRect(coordsRect, pBackgroundColor);

    // Draw the text for the coordinates, using a white pen

    QPen pen = pPainter->pen();

    pen.setColor(pForegroundColor);

    pPainter->setPen(pen);

    pPainter->drawText(coordsRect, coords);
}

//==============================================================================

void SingleCellViewGraphPanelPlotWidget::drawCanvas(QPainter *pPainter)
{
    switch (mAction) {
    case ShowCoordinates: {
        // We are showing some coordinates, so start by drawing our pixmap

        pPainter->drawPixmap(0, 0, mCanvasPixmap);

        // Draw the two dashed lines that show the coordinates, using a dark
        // cyan pen

        QPen pen = pPainter->pen();
        QColor backgroundColor = Qt::darkCyan;

        backgroundColor.setAlphaF(0.69);

        pen.setColor(backgroundColor);
        pen.setStyle(Qt::DashLine);

        pPainter->setPen(pen);

        pPainter->drawLine(0.0, mOriginPoint.y(),
                           plotLayout()->canvasRect().width(), mOriginPoint.y());
        pPainter->drawLine(mOriginPoint.x(), 0.0,
                           mOriginPoint.x(), plotLayout()->canvasRect().height());

        // Draw the coordinates

        drawCoordinates(pPainter, mOriginPoint, backgroundColor, Qt::white);

        break;
    }
    case ZoomRegion: {
        // We are zooming a region, so start by drawing our pixmap

        pPainter->drawPixmap(0, 0, mCanvasPixmap);

        // Determine the coordinates of the region to be zoomed

        double xMin;
        double xMax;
        double yMin;
        double yMax;

        if ((mMaxX-mMinX)/(localMaxX()-localMinX()) < MaxZoomFactor) {
            xMin = qMin(mOriginPoint.x(), mEndPoint.x());
            xMax = qMax(mOriginPoint.x(), mEndPoint.x());
        } else {
            // We are already fully zoomed in on the X axis, so hard set the
            // minimum and maximum X values

            QwtScaleMap canvasMapX = canvasMap(QwtPlot::xBottom);

            xMin = canvasMapX.transform(localMinX());
            xMax = canvasMapX.transform(localMaxX());
        }

        if ((mMaxY-mMinY)/(localMaxY()-localMinY()) < MaxZoomFactor) {
            yMin = qMin(mOriginPoint.y(), mEndPoint.y());
            yMax = qMax(mOriginPoint.y(), mEndPoint.y());
        } else {
            // We are already fully zoomed in on the Y axis, so hard set the
            // minimum and maximum Y values

            QwtScaleMap canvasMapY = canvasMap(QwtPlot::yLeft);

            yMin = canvasMapY.transform(localMaxY());
            yMax = canvasMapY.transform(localMinY());
        }

        QRect zoomRegionRect(xMin, yMin, xMax-xMin, yMax-yMin);

        // Now, draw the region to be zoomed

        QColor penColor = Qt::darkRed;
        QColor brushColor = Qt::yellow;

        penColor.setAlphaF(0.69);
        brushColor.setAlphaF(0.19);

        pPainter->setPen(penColor);

        pPainter->fillRect(zoomRegionRect, brushColor);
        pPainter->drawRect(zoomRegionRect);

        // Draw the two sets of coordinates
        // Note: for historical reasons, QRect::bottomRight() returns
        //       QPoint(left()+width()-1, top()+height()-1), hence we add
        //       QPoint(1, 1)...

        drawCoordinates(pPainter, zoomRegionRect.topLeft(), penColor, Qt::white, BottomRight, false);
        drawCoordinates(pPainter, zoomRegionRect.bottomRight()+QPoint(1, 1), penColor, Qt::white, TopLeft, false);

        break;
    }
    default:
        // We aren't doing anything special, so just draw our canvas normally

        QwtPlot::drawCanvas(pPainter);
    }
}

//==============================================================================

void SingleCellViewGraphPanelPlotWidget::attach(SingleCellViewGraphPanelPlotCurve *pCurve)
{
    // Make sure that the given curve is not already attached to us

    if (mCurves.contains(pCurve))
        return;

    // Attach the given curve to ourselves

    pCurve->attach(this);

    // Add the given curve to our list of curves

    mCurves << pCurve;
}

//==============================================================================

void SingleCellViewGraphPanelPlotWidget::detach(SingleCellViewGraphPanelPlotCurve *pCurve)
{
    // Check that the given curve is attached to us

    if (!mCurves.contains(pCurve))
        return;

    // Detach the given curve from ourselves

    pCurve->detach();

    // Remove the given curve from our list of curves

    mCurves.removeOne(pCurve);
}

//==============================================================================

void SingleCellViewGraphPanelPlotWidget::drawCurveSegment(SingleCellViewGraphPanelPlotCurve *pCurve,
                                                          const qulonglong &pFrom,
                                                          const qulonglong &pTo)
{
    // Make sure that we have a curve segment to draw

    if (pFrom == pTo)
        return;

    // Reset our local axes and replot ourselves, if it is our first curve
    // segment, or carry on as normal

    if (!pFrom) {
        // It is our first curve segment, so check our local axes
        // Note: we always want to replot, hence our passing false as an
        //       argument to resetLocalAxes()...

        resetLocalAxes(false);
        replotNow();
    } else {
        // It's not our first curve segment, so determine the minimum/maximum
        // X/Y values of our new data

        double xMin = 0.0;
        double xMax = 0.0;
        double yMin = 0.0;
        double yMax = 0.0;

        for (qulonglong i = pFrom; i <= pTo; ++i)
            if (i == pFrom) {
                xMin = xMax = pCurve->data()->sample(i).x();
                yMin = yMax = pCurve->data()->sample(i).y();
            } else {
                double xVal = pCurve->data()->sample(i).x();
                double yVal = pCurve->data()->sample(i).y();

                xMin = qMin(xMin, xVal);
                xMax = qMax(xMax, xVal);

                yMin = qMin(yMin, yVal);
                yMax = qMax(yMax, yVal);
            }

        // Check whether our X/Y axis can handle the minimum/maximum X/Y values
        // of our new data

        if (   (xMin < minX()) || (xMax > maxX())
            || (yMin < minY()) || (yMax > maxY()))
            // Our X/Y axis cannot handle the minimum/maximum X/Y values of our
            // new data, so check our local axes

            checkLocalAxes(true, true, true);
        else
            // Our X/Y axis can handle the X/Y min/max of our new data, so just
            // draw our new curve segment

            mDirectPainter->drawSeries(pCurve, pFrom, pTo);
    }
}

//==============================================================================

}   // namespace SingleCellView
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
