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
// Preferences dialog
//==============================================================================

#pragma once

//==============================================================================

#include "guiutils.h"
#include "plugininfo.h"

//==============================================================================

class QLabel;
class QSettings;
class QStandardItem;
class QStandardItemModel;

//==============================================================================

namespace Ui {
    class PreferencesDialog;
} // namespace Ui

//==============================================================================

namespace OpenCOR {

//==============================================================================

class PluginManager;
class PreferencesInterface;

//==============================================================================

namespace Preferences {
    class PreferencesWidget;
} // namespace Preferences

//==============================================================================

class PreferencesPluginCategoryWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PreferencesPluginCategoryWidget(QWidget *pParent);

    void setCategory(const QString &pCategory);
    void setDescription(const QString &pDescription);

private:
    QLabel *mCategoryValue;
    QLabel *mDescriptionValue;

    QLabel * label(const QString &pLabel);
};

//==============================================================================

class PreferencesItemDelegate : public StyledItemDelegate
{
public:
    explicit PreferencesItemDelegate(QObject *pParent);

    void paint(QPainter *pPainter, const QStyleOptionViewItem &pOption,
               const QModelIndex &pIndex) const override;
};

//==============================================================================

class PreferencesDialog : public Dialog
{
    Q_OBJECT

public:
    explicit PreferencesDialog(PluginManager *pPluginManager,
                               const QString &pPluginName, QWidget *pParent);
    ~PreferencesDialog() override;

    QStringList pluginNames() const;

private:
    Ui::PreferencesDialog *mGui;

    PluginManager *mPluginManager;

    QStandardItemModel *mModel;

    QMap<PluginInfo::Category, QStandardItem *> mCategoryItems;
    QHash<QStandardItem *, PluginInfo::Category> mItemCategories;
    QHash<QStandardItem *, Preferences::PreferencesWidget *> mItemPreferencesWidgets;
    QHash<Preferences::PreferencesWidget *, QString> mPreferencesWidgetPluginNames;

    PreferencesPluginCategoryWidget *mPluginCategoryWidget;

    QPushButton *mResetButton;
    QPushButton *mResetAllButton;

    QStringList mPluginNames;

    QStandardItem * pluginCategoryItem(PluginInfo::Category pCategory);

private slots:
    void treeViewCollapsed(const QModelIndex &pIndex);

    void buttonBoxAccepted();

    void updatePreferencesWidget(const QModelIndex &pNewIndex,
                                 const QModelIndex &pOldIndex);

    void reset();
    void resetAll();
};

//==============================================================================

} // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
