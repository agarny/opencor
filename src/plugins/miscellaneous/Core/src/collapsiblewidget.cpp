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
// Collapsible widget
//==============================================================================

#include "collapsiblewidget.h"
#include "coreguiutils.h"

#ifdef Q_OS_MAC
    #include "macos.h"
#endif

//==============================================================================

#include <QEvent>
#include <QMenu>
#include <QSettings>
#include <QToolButton>
#include <QVariant>
#include <QVBoxLayout>

//==============================================================================

namespace OpenCOR {
namespace Core {

//==============================================================================

CollapsibleHeaderTitleWidget::CollapsibleHeaderTitleWidget(QWidget *pParent) :
    QLabel(pParent)
{
}

//==============================================================================

void CollapsibleHeaderTitleWidget::mouseDoubleClickEvent(QMouseEvent *pEvent)
{
    // Default handling of the event

    QLabel::mouseDoubleClickEvent(pEvent);

    // Let people know that the widget has been double clicked

    emit doubleClicked();
}

//==============================================================================

CollapsibleHeaderWidget::CollapsibleHeaderWidget(bool pCollapsible,
                                                 QWidget *pParent) :
    QWidget(pParent)
{
    // Create and set our vertical layout

    auto layout = new QVBoxLayout(this);

    layout->setContentsMargins({});
    layout->setSpacing(0);

    setLayout(layout);

    // Create a sub-widget with a horizontal layout

    auto subWidget = new QWidget(this);
    auto subLayout = new QHBoxLayout(subWidget);

    subLayout->setContentsMargins({});
    subLayout->setSpacing(0);

    subWidget->setLayout(subLayout);

    // Create and customise our button and title

    static const QIcon NoIcon;
    static const QIcon DownIcon = QIcon(":/oxygen/actions/arrow-down.png");

    mButton = new QToolButton(subWidget);
    mTitle = new CollapsibleHeaderTitleWidget(subWidget);
    mMenu = new QToolButton(subWidget);

    int iconSize = int(0.4*mTitle->height());

    mButton->setIcon(pCollapsible?DownIcon:NoIcon);
    mButton->setIconSize(QSize(iconSize, iconSize));

    mTitle->setAlignment(Qt::AlignCenter);

    mMenu->setIcon(NoIcon);
    mMenu->setIconSize(QSize(iconSize, iconSize));

    // Initialise our "palette"

    paletteChanged();

    // Add our button and title to our sub-layout

    subLayout->addWidget(mButton);
    subLayout->addWidget(mTitle);
    subLayout->addWidget(mMenu);

    // Add a top separator, our sub-widget and a bottom separator to our main
    // layout
    // Note: we keep track of our top and bottom separators since we may need
    //       to hide them in some cases/circumstances...

    mTopSeparator = newLineWidget(this);
    mBottomSeparator = newLineWidget(this);

    layout->addWidget(mTopSeparator);
    layout->addWidget(subWidget);
    layout->addWidget(mBottomSeparator);

    // Connections to toggle our collapsed state

    if (pCollapsible) {
        connect(mButton, &QToolButton::clicked,
                this, &CollapsibleHeaderWidget::toggleCollapsedState);
        connect(mTitle, &CollapsibleHeaderTitleWidget::doubleClicked,
                this, &CollapsibleHeaderWidget::toggleCollapsedState);
    }
}

//==============================================================================

void CollapsibleHeaderWidget::changeEvent(QEvent *pEvent)
{
    // Default handling of the event

    QWidget::changeEvent(pEvent);

    // Do a few more things for some changes

    if (pEvent->type() == QEvent::PaletteChange) {
        paletteChanged();
    }
}

//==============================================================================

void CollapsibleHeaderWidget::setFirstHeader(bool pFirstHeader)
{
    // Show/hide our top separator depending on whether we are the first header

    mTopSeparator->setVisible(!pFirstHeader);
}

//==============================================================================

void CollapsibleHeaderWidget::setLastHeader(bool pLastHeader)
{
    // Keep track of our last header status and update our bottom separator
    // visible status, if needed

    if (pLastHeader != mLastHeader) {
        mLastHeader = pLastHeader;

        updateBottomSeparatorVisibleStatus();
    }
}

//==============================================================================

bool CollapsibleHeaderWidget::isCollapsable() const
{
    // Return wheter we are collapsable

    return !mButton->icon().isNull();
}

//==============================================================================

bool CollapsibleHeaderWidget::isCollapsed() const
{
    // Return wheter we are collapsed

    return mCollapsed;
}

//==============================================================================

void CollapsibleHeaderWidget::setCollapsed(bool pCollapsed)
{
    // Collapse or uncollapse ourselves, if needed

    if (pCollapsed != mCollapsed) {
        toggleCollapsedState();
    }
}

//==============================================================================

QString CollapsibleHeaderWidget::title() const
{
    // Return our title

    return mTitle->text();
}

//==============================================================================

void CollapsibleHeaderWidget::setTitle(const QString &pTitle)
{
    // Set our title

    mTitle->setText(pTitle);
}

//==============================================================================

QMenu * CollapsibleHeaderWidget::menu() const
{
    // Return our menu

    return mMenuMenu;
}

//==============================================================================

void CollapsibleHeaderWidget::setMenu(QMenu *pMenu)
{
    // Set our menu and update our palette to make sure that our menu icon is
    // correct

    if (pMenu != mMenuMenu) {
        static const QIcon NoIcon;
        static const QIcon MenuIcon = QIcon(":/menu.png");

        mMenu->setIcon((pMenu != nullptr)?MenuIcon:NoIcon);

        mMenuMenu = pMenu;

        if (pMenu != nullptr) {
            connect(mMenu, &QToolButton::clicked,
                    this, &CollapsibleHeaderWidget::showMenu);
        } else {
            disconnect(mMenu, &QToolButton::clicked,
                       this, &CollapsibleHeaderWidget::showMenu);
        }

        paletteChanged();
    }
}

//==============================================================================

void CollapsibleHeaderWidget::updateBottomSeparatorVisibleStatus()
{
    // Show/hide our bottom separator depending on whether we are collapsed or
    // whether we are the last header

    mBottomSeparator->setVisible(!mCollapsed || mLastHeader);
}

//==============================================================================

void CollapsibleHeaderWidget::paletteChanged()
{
    // Our palette has changed, so update our background colour, but only if it
    // has really changed (otherwise we get into a recursive loop)

    QString backgroundColor = windowColor().name();

    if (backgroundColor != mBackgroundColor) {
        static const QString IconStyleSheet = "QToolButton {"
                                              "    background-color: %1;"
                                              "    border: none;"
                                              "}"
                                              ""
                                              "QToolButton:pressed {"
                                              "    margin: 0px;"
                                              "}";
        static const QString TitleStyleSheet = "QLabel {"
                                               "    background-color: %1;"
                                               "}";

        mBackgroundColor = backgroundColor;

        mButton->setStyleSheet(IconStyleSheet.arg(mBackgroundColor));
        mTitle->setStyleSheet(TitleStyleSheet.arg(mBackgroundColor));
        mMenu->setStyleSheet(IconStyleSheet.arg(mBackgroundColor));
    }

    // Also update our menu icon, if needed
    // Note: unlike for our background colour, we must always do it since we can
    //       set/reset our menu using setMenu()...

    static const QIcon NoIcon   = QIcon();
    static const QIcon MenuIcon = QIcon(":/menu.png");
#ifdef Q_OS_MAC
    static const QIcon InvertedMenuIcon = invertedIcon(MenuIcon);
#endif

#ifdef Q_OS_MAC
    mMenu->setIcon((mMenuMenu != nullptr)?
                       isDarkMode()?
                           InvertedMenuIcon:
                           MenuIcon:
                       NoIcon);
#else
    mMenu->setIcon(mMenuMenu?MenuIcon:NoIcon);
#endif
}

//==============================================================================

void CollapsibleHeaderWidget::toggleCollapsedState()
{
    // Toggle our collapsed state

    mCollapsed = !mCollapsed;

    // Update our button's icon to reflect our new collapsed state

    static const QIcon DownIcon  = QIcon(":/oxygen/actions/arrow-down.png");
    static const QIcon RightIcon = QIcon(":/oxygen/actions/arrow-right.png");

    mButton->setIcon(mCollapsed?RightIcon:DownIcon);

    // Update our bottom separator visible status

    updateBottomSeparatorVisibleStatus();

    // Let people know about our new new collapsed state

    emit widgetVisible(!mCollapsed);
}

//==============================================================================

void CollapsibleHeaderWidget::showMenu()
{
    // Show our menu

    mMenuMenu->exec(QCursor::pos());
}

//==============================================================================

CollapsibleWidget::CollapsibleWidget(QWidget *pParent) :
    Widget({}, pParent)
{
    // Create a vertical layout that will contain our headers and widgets

    mLayout = new QVBoxLayout(this);

    mLayout->setContentsMargins({});
    mLayout->setSpacing(0);

    setLayout(mLayout);
}

//==============================================================================

int CollapsibleWidget::count() const
{
    // Return our number of headers

    return mHeaders.count();
}

//==============================================================================

bool CollapsibleWidget::isCollapsed(int pIndex) const
{
    // Return wheter our requested header is collapsed

    return (mHeaders.value(pIndex) != nullptr)?mHeaders[pIndex]->isCollapsed():false;
}

//==============================================================================

void CollapsibleWidget::setCollapsed(int pIndex, bool pCollapsed)
{
    // Collapse or uncollapse our requested header

    if (mHeaders.value(pIndex) != nullptr) {
        mHeaders[pIndex]->setCollapsed(pCollapsed);
    }
}

//==============================================================================

QString CollapsibleWidget::headerTitle(int pIndex) const
{
    // Return the title of our requested header

    return (mHeaders.value(pIndex) != nullptr)?mHeaders[pIndex]->title():QString();
}

//==============================================================================

void CollapsibleWidget::setHeaderTitle(int pIndex, const QString &pTitle)
{
    // Set the title of our requested header

    if (mHeaders.value(pIndex) != nullptr) {
        mHeaders[pIndex]->setTitle(pTitle);
    }
}

//==============================================================================

CollapsibleHeaderWidget * CollapsibleWidget::addWidget(QWidget *pWidget,
                                                       bool pCollapsible)
{
    // Make sure that there is a widget to add

    if (pWidget == nullptr) {
        return nullptr;
    }

    // We want to add a widget, so we first need to add a header to our layout

    auto header = new CollapsibleHeaderWidget(pCollapsible, this);

    // Let our header know whether it is the first header

    header->setFirstHeader(mHeaders.isEmpty());

    // Let our header know that, if anything, it is our new last header, meaning
    // that our previous last header is not the last one anymore

    if (!mHeaders.isEmpty()) {
        mHeaders.last()->setLastHeader(false);
    }

    header->setLastHeader(true);

    // Now, we can actually add the header to our layout, as well as keep track
    // of it

    mLayout->addWidget(header);

    mHeaders << header;

    // Add the widget itself

    mLayout->addWidget(pWidget);

    // Create a connection to show/hide our widget depending on the collapsed
    // state of our header, as well as one to let people know about our overall
    // new collapsible state

    if (pCollapsible) {
        connect(header, &CollapsibleHeaderWidget::widgetVisible,
                pWidget, &QWidget::setVisible);

        connect(header, &CollapsibleHeaderWidget::widgetVisible,
                this, &CollapsibleWidget::emitCollapsed);
    }

    return header;
}

//==============================================================================

void CollapsibleWidget::emitCollapsed()
{
    // Let people know that a collapsed state has changed

    for (int i = 0, iMax = mHeaders.count(); i < iMax; ++i) {
        if (mHeaders[i] == sender()) {
            emit collapsed(i, mHeaders[i]->isCollapsed());

            return;
        }
    }
}

//==============================================================================

} // namespace Core
} // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
