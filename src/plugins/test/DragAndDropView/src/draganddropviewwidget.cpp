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
// Drag & drop view widget
//==============================================================================

#include "corecliutils.h"
#include "draganddropviewwidget.h"
#include "interfaces.h"

//==============================================================================

#include <QDesktopServices>
#include <QDragEnterEvent>
#include <QHBoxLayout>
#include <QListView>
#include <QStandardItemModel>

//==============================================================================

namespace OpenCOR {
namespace DragAndDropView {

//==============================================================================

DragAndDropViewWidget::DragAndDropViewWidget(QWidget *pParent) :
    ViewWidget(pParent)
{
    // Create our layout

    auto layout = new QHBoxLayout(this);

    layout->setContentsMargins({});

    setLayout(layout);

    // Create our list view and add it to our layout

    mListView = new QListView(this);
    mModel = new QStandardItemModel(this);

    mListView->setFrameShape(QFrame::NoFrame);
    mListView->setModel(mModel);

    layout->addWidget(mListView);

    // Allow for things to be dropped on us

    setAcceptDrops(true);
}

//==============================================================================

QWidget * DragAndDropViewWidget::widget(const QString &pFileName)
{
    Q_UNUSED(pFileName)

    // Return the requested (self) widget

    return this;
}

//==============================================================================

void DragAndDropViewWidget::dragEnterEvent(QDragEnterEvent *pEvent)
{
    // Accept the proposed action for the event, but only if it refers to one or
    // several data store files

    const QStringList fileNames = Core::droppedFileNames(pEvent);
    const FileTypeInterfaces supportedFileTypeInterfaces = Core::fileTypeInterfaces()+Core::dataStoreFileTypeInterfaces();
    bool acceptEvent = false;

    for (const auto &fileName : fileNames) {
        for (auto supportedFileTypeInterface : supportedFileTypeInterfaces) {
            if (supportedFileTypeInterface->isFile(fileName)) {
                mSupportedFileTypeInterfaces.insert(fileName, supportedFileTypeInterface);

                acceptEvent = true;

                break;
            }
        }
    }

    if (acceptEvent) {
        pEvent->acceptProposedAction();
    } else {
        pEvent->ignore();
    }
}

//==============================================================================

void DragAndDropViewWidget::dragMoveEvent(QDragMoveEvent *pEvent)
{
    // Accept the proposed action for the event

    pEvent->acceptProposedAction();
}

//==============================================================================

void DragAndDropViewWidget::dropEvent(QDropEvent *pEvent)
{
    // Import/open the one or several files

    const QStringList fileNames = Core::droppedFileNames(pEvent);

    for (const auto &fileName : fileNames) {
        if (mSupportedFileTypeInterfaces.contains(fileName)) {
            auto item = new QStandardItem(fileName);

            item->setEditable(false);

            mModel->appendRow(item);
        } else {
            QDesktopServices::openUrl("opencor://openFile/"+fileName);
        }
    }

    // Accept the proposed action for the event

    pEvent->acceptProposedAction();
}

//==============================================================================

} // namespace DragAndDropView
} // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
