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
// Splitter widget
//==============================================================================

#include "coreguiutils.h"
#include "splitterwidget.h"

//==============================================================================

#include <QEvent>

//==============================================================================

namespace OpenCOR {
namespace Core {

//==============================================================================

SplitterWidget::SplitterWidget(const Qt::Orientation &pOrientation,
                               QWidget *pParent) :
    QSplitter(pParent),
    CommonWidget(this),
    mBackgroundColor(QString())
{
    // Customise ourselves so that we are as thin as possible

    setHandleWidth(2);
    setOrientation(pOrientation);

    // Initialise our "palette"

    paletteChanged();
}

//==============================================================================

SplitterWidget::SplitterWidget(QWidget *pParent) :
    SplitterWidget(Qt::Horizontal, pParent)
{
}

//==============================================================================

SplitterWidget::~SplitterWidget() = default;

//==============================================================================

void SplitterWidget::changeEvent(QEvent *pEvent)
{
    // Default handling of the event

    QSplitter::changeEvent(pEvent);

    // Do a few more things for some changes

    if (pEvent->type() == QEvent::PaletteChange) {
        paletteChanged();
    }
}

//==============================================================================

void SplitterWidget::paletteChanged()
{
    // Our palette has changed, so update our background color (and make sure
    // that we have no image for our handle), but only if it has really changed
    // (otherwise we get into a recursive loop)

    QString backgroundColor = windowColor().name();

    if (backgroundColor != mBackgroundColor) {
        static const QString StyleSheet = "QSplitter {"
                                          "    background-color: %1"
                                          "}"
                                          ""
                                          "QSplitter::handle {"
                                          "    image: none;"
                                          "}";

        mBackgroundColor = backgroundColor;

        setStyleSheet(StyleSheet.arg(mBackgroundColor));
    }
}

//==============================================================================

} // namespace Core
} // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
