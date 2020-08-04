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
// Mapping view widget
//==============================================================================

#include "borderedwidget.h"
#include "mappingviewwidget.h"
#include "zincwidget.h"
#include "toolbarwidget.h"

//==============================================================================

#include <QMenu>
#include <QApplication>
#include <QScreen>

//==============================================================================

#include "zincbegin.h" //TODO takeaway the useless
    #include "opencmiss/zinc/fieldconstant.hpp"
    #include "opencmiss/zinc/fieldmodule.hpp"
    #include "opencmiss/zinc/fieldvectoroperators.hpp"
    #include "opencmiss/zinc/status.h"
#include "zincend.h"

//==============================================================================

namespace OpenCOR {
namespace MappingView {

//==============================================================================

MappingViewWidget::MappingViewWidget(QWidget *pParent) :
    Core::SplitterWidget(pParent)
{

    // Set our orientation

    setOrientation(Qt::Vertical);

    //create and add toolbar

    mToolBarWidget = new Core::ToolBarWidget();

        QRect availableGeometry = qApp->primaryScreen()->availableGeometry();

        mDelayWidget = new QwtWheel(mToolBarWidget);

        mDelayWidget->setBorderWidth(0);
        mDelayWidget->setFixedSize(int(0.07*availableGeometry.width()),
                                   mDelayWidget->height()/2);
        mDelayWidget->setFocusPolicy(Qt::NoFocus);
        mDelayWidget->setRange(0.0, 100.0);
        mDelayWidget->setWheelBorderWidth(0);

        mDelayWidget->setValue(MappingViewZincWidget::nodeSizeOrigin);

    mToolBarWidget->addWidget(mDelayWidget);

    addWidget(mToolBarWidget);

    //create and add informative labels

    QLabel *nodeLabel = new QLabel("Node:",this);
    addWidget(nodeLabel);

    mNodeValue = new QLabel(this);
    addWidget(mNodeValue);

    QLabel *variableLabel = new QLabel("Variable:",this);
    addWidget(variableLabel);

    mVariableValue = new QLabel(this);
    addWidget(mVariableValue);

    //create and add the variable tree:

    mVariableTree = new QTreeView(this);
    addWidget(mVariableTree);

    // Keep track of our movement
    /*
    connect(this, &Core::SplitterWidget::splitterMoved,
            this, &MappingViewEditingWidget::splitterMoved);
    */
    
    //addWidget(mListWidgetVariables);

    //TODO
    mMeshFileName = "/home/tuareg/Documents/OpenCOR/opencor/meshes/circulation.exnode";

    // add a Zinc widget

    mMappingViewZincWidget = new MappingViewZincWidget(this, mMeshFileName);

    connect(mMappingViewZincWidget, &MappingViewZincWidget::nodeSelection,
            this, &MappingViewWidget::nodeSelection);
    connect(mDelayWidget, &QwtWheel::valueChanged,
            mMappingViewZincWidget, &MappingViewZincWidget::setNodeSizes );

    addWidget(mMappingViewZincWidget);

    //mToolBarWidget = new Core::ToolBarWidget();

}

//==============================================================================

MappingViewWidget::~MappingViewWidget()
{
}

//==============================================================================

void MappingViewWidget::retranslateUi()
{
    // Update ourself too since some widgets will have been reset following the
    // retranslation (e.g. mGui->fileNameValue)

    for(auto editingWidget : mEditingWidgets) {
        editingWidget->retranslateUi();
    }
}

//==============================================================================

void MappingViewWidget::initialize(const QString &pFileName)
{
    // Retrieve the editing widget associated with the given file, if any

    mEditingWidget = mEditingWidgets.value(pFileName);

    if (mEditingWidget == nullptr) {
        // No editing widget exists for the given file, so create one

        mEditingWidget = new MappingViewEditingWidget(pFileName, mMeshFileName,this);

        mEditingWidgets.insert(pFileName, mEditingWidget);
    }

    //mListWidgetVariables->setModel(mEditingWidget->listViewModelVariables()); //TODO set only when charging the plugin ?
    //mGui->outputList->setModel(mEditingWidget->listViewModelOutput());
    mVariableTree->setModel(mEditingWidget->getTreeViewModel());
}

//==============================================================================

void MappingViewWidget::finalize(const QString &pFileName)
{
    // Remove the editing widget, should there be one for the given file

    MappingViewEditingWidget *editingWidget = mEditingWidgets.value(pFileName);

    if (editingWidget != nullptr) {
        // There is an editing widget for the given file name, so delete it and
        // remove it from our list

        delete editingWidget;

        mEditingWidgets.remove(pFileName);

        // Reset our memory of the current editor, if needed

        if (editingWidget == mEditingWidget) {
            mEditingWidget = nullptr;
        }
    }
}

//==============================================================================

void MappingViewWidget::filePermissionsChanged(const QString &pFileName)
{
    // The given file has been un/locked, so enable/disable parts of our GUI,
    // should the given file be managed

    MappingViewEditingWidget *editingWidget = mEditingWidgets.value(pFileName);

    if (editingWidget != nullptr) {
        editingWidget->filePermissionsChanged();
    }
}

//==============================================================================

void MappingViewWidget::fileSaved(const QString &pFileName)
{
    Q_UNUSED(pFileName);
}

//==============================================================================

void MappingViewWidget::fileReloaded(const QString &pFileName)
{
    // The given file has been reloaded, so reload it, should it be managed

    if (mEditingWidgets.contains(pFileName)) {
        finalize(pFileName);
        initialize(pFileName);
    }
}

//==============================================================================

void MappingViewWidget::fileRenamed(const QString &pOldFileName, const QString &pNewFileName)
{
    // The given file has been renamed, so update our editing widgets mapping

    MappingViewEditingWidget *editingWidget = mEditingWidgets.value(pOldFileName);

    if (editingWidget != nullptr) {
        mEditingWidgets.insert(pNewFileName, editingWidget);
        mEditingWidgets.remove(pOldFileName);
    }
}

//==============================================================================

void MappingViewWidget::nodeSelection(int pId) {

    if (pId==-1) {
        mNodeValue->setText("");
        mVariableValue->setText("");
    } else {
        mNodeValue->setNum(pId);
        mVariableValue->setText(mEditingWidget->getVariableOfNode(pId));
    }
}

//==============================================================================

} // namespace MappingView
} // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
