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

#pragma once

//==============================================================================

#include "viewwidget.h"

//==============================================================================

class QListView;
class QStandardItemModel;

//==============================================================================

namespace OpenCOR {

//==============================================================================

class FileTypeInterface;

//==============================================================================

namespace DragAndDropView {

//==============================================================================

class DragAndDropViewWidget : public Core::ViewWidget
{
    Q_OBJECT

public:
    explicit DragAndDropViewWidget(QWidget *pParent);

    QWidget * widget(const QString &pFileName) override;

protected:
    void dragEnterEvent(QDragEnterEvent *pEvent) override;
    void dragMoveEvent(QDragMoveEvent *pEvent) override;
    void dropEvent(QDropEvent *pEvent) override;

private:
    QListView *mListView;
    QStandardItemModel *mModel;

    QMap<QString, FileTypeInterface *> mSupportedFileTypeInterfaces;
};

//==============================================================================

} // namespace DragAndDropView
} // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
