/*******************************************************************************

Licensed to the OpenCOR team under one or more contributor license agreements.
See the NOTICE.txt file distributed with this work for additional information
regarding copyright ownership. The OpenCOR team licenses this file to you under
the Apache License, Version 2.0 (the "License"); you may not use this file
except in compliance with the License. You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed
under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
CONDITIONS OF ANY KIND, either express or implied. See the License for the
specific language governing permissions and limitations under the License.

*******************************************************************************/

//==============================================================================
// Core CellML editing widget
//==============================================================================

#include "borderedwidget.h"
#include "cliutils.h"
#include "corecellmleditingeditorlistwidget.h"
#include "corecellmleditingwidget.h"
#include "editorwidget.h"
#include "filemanager.h"
#include "guiutils.h"
#include "viewerwidget.h"

//==============================================================================

#include "ui_corecellmleditingwidget.h"

//==============================================================================

#include <QDesktopWidget>
#include <QFileInfo>
#include <QListView>
#include <QSettings>
#include <QSplitter>

//==============================================================================

namespace OpenCOR {
namespace CoreCellMLEditing {

//==============================================================================

CoreCellmlEditingWidget::CoreCellmlEditingWidget(const QString &pContents,
                                                 const bool &pReadOnly,
                                                 QsciLexer *pLexer,
                                                 QWidget *pParent) :
    QSplitter(pParent),
    Core::CommonWidget(pParent),
    mGui(new Ui::CoreCellmlEditingWidget)
{
    // Set up the GUI

    mGui->setupUi(this);

    // Create our bordered viewer

    mViewer = new Viewer::ViewerWidget(this);
    mBorderedViewer = new Core::BorderedWidget(mViewer,
                                               false, false, true, false);

    // Create our bordered editor

    mEditor = new Editor::EditorWidget(pContents, pReadOnly, pLexer, this);
    mBorderedEditor = new Core::BorderedWidget(mEditor,
                                               true, false, true, false);

    // Create our bordered editor list

    mEditorList = new CoreCellmlEditingEditorListWidget(this);
    mBorderedEditorList = new Core::BorderedWidget(mEditorList,
                                                   true, false, false, false);

    // Add the bordered viewer, editor and editor list to ourselves

    addWidget(mBorderedViewer);
    addWidget(mBorderedEditor);
    addWidget(mBorderedEditorList);

    // Set our focus proxy to our editor

    setFocusProxy(mEditor);
}

//==============================================================================

CoreCellmlEditingWidget::~CoreCellmlEditingWidget()
{
    // Delete the GUI

    delete mGui;
}

//==============================================================================

static const auto SettingsEditingWidgetSizes = QStringLiteral("EditingWidgetSizes");

//==============================================================================

void CoreCellmlEditingWidget::loadSettings(QSettings *pSettings)
{
    // Retrieve our sizes
    // Note #1: the viewer's and editor list's default height is 19% and 13%,
    //          respectively, of the desktop's height while that of the editor
    //          is as big as it can be...
    // Note #2: because the editor's default height is much bigger than that of
    //          our widget, the viewer's and editor list's default height will
    //          effectively be less than 19% and 13%, respectively, of the
    //          desktop's height, but that doesn't matter at all...

    QVariantList defaultEditingWidgetSizes = QVariantList() << 0.19*qApp->desktop()->screenGeometry().height()
                                                            << qApp->desktop()->screenGeometry().height()
                                                            << 0.13*qApp->desktop()->screenGeometry().height();

    setSizes(qVariantListToIntList(pSettings->value(SettingsEditingWidgetSizes, defaultEditingWidgetSizes).toList()));

    // Retrieve our viewer's and editor's settings

    mViewer->loadSettings(pSettings);
    mEditor->loadSettings(pSettings);
}

//==============================================================================

void CoreCellmlEditingWidget::saveSettings(QSettings *pSettings) const
{
    // Keep track of our sizes

    pSettings->setValue(SettingsEditingWidgetSizes, qIntListToVariantList(sizes()));

    // Keep track of our viewer's and editor's settings

    mViewer->saveSettings(pSettings);
    mEditor->saveSettings(pSettings);
}

//==============================================================================

void CoreCellmlEditingWidget::retranslateUi()
{
    // Retranslate our viewer and editor

    mViewer->retranslateUi();
    mEditor->retranslateUi();
}

//==============================================================================

void CoreCellmlEditingWidget::updateSettings(CoreCellmlEditingWidget *pCoreCellmlEditingWidget)
{
    // Make sure that we are given another widget

    if (!pCoreCellmlEditingWidget)
        return;

    // Update our sizes, viewer settings and editor settings

    setSizes(pCoreCellmlEditingWidget->sizes());

    mViewer->updateSettings(pCoreCellmlEditingWidget->viewer());
    mEditor->updateSettings(pCoreCellmlEditingWidget->editor());
}

//==============================================================================

Viewer::ViewerWidget * CoreCellmlEditingWidget::viewer() const
{
    // Return our viewer

    return mViewer;
}

//==============================================================================

Editor::EditorWidget * CoreCellmlEditingWidget::editor() const
{
    // Return our editor

    return mEditor;
}

//==============================================================================

CoreCellmlEditingEditorListWidget * CoreCellmlEditingWidget::editorList() const
{
    // Return our editor list

    return mEditorList;
}

//==============================================================================

}   // namespace CoreCellMLEditing
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
