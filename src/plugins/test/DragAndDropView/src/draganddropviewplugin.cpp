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
// Drag & drop view plugin
//==============================================================================

#include "coreguiutils.h"
#include "draganddropviewplugin.h"
#include "draganddropviewwidget.h"

//==============================================================================

#include <QIcon>
#include <QMainWindow>

//==============================================================================

namespace OpenCOR {
namespace DragAndDropView {

//==============================================================================

PLUGININFO_FUNC DragAndDropViewPluginInfo()
{
    Descriptions descriptions;

    descriptions.insert("en", QString::fromUtf8("a plugin that provides a drag & drop test view."));
    descriptions.insert("fr", QString::fromUtf8("une extension qui fournit une vue de test copier & coller."));

    return new PluginInfo(PluginInfo::Category::Test, true, false,
                          { "Core" },
                          descriptions);
}

//==============================================================================
// I18n interface
//==============================================================================

void DragAndDropViewPlugin::retranslateUi()
{
    // We don't handle this interface...
    // Note: even though we don't handle this interface, we still want to
    //       support it since some other aspects of our plugin are
    //       multilingual...
}

//==============================================================================
// Plugin interface
//==============================================================================

bool DragAndDropViewPlugin::definesPluginInterfaces()
{
    // We don't handle this interface...

    return false;
}

//==============================================================================

bool DragAndDropViewPlugin::pluginInterfacesOk(const QString &pFileName,
                                               QObject *pInstance)
{
    Q_UNUSED(pFileName)
    Q_UNUSED(pInstance)

    // We don't handle this interface...

    return false;
}

//==============================================================================

void DragAndDropViewPlugin::initializePlugin()
{
    // Create our drag & drop view widget

    mViewWidget = new DragAndDropViewWidget(Core::mainWindow());

    // Hide our drag & drop view widget since it may not initially be shown in
    // our central widget

    mViewWidget->setVisible(false);
}

//==============================================================================

void DragAndDropViewPlugin::finalizePlugin()
{
    // We don't handle this interface...
}

//==============================================================================

void DragAndDropViewPlugin::pluginsInitialized(const Plugins &pLoadedPlugins)
{
    Q_UNUSED(pLoadedPlugins)

    // We don't handle this interface...
}

//==============================================================================

void DragAndDropViewPlugin::loadSettings(QSettings &pSettings)
{
    Q_UNUSED(pSettings)

    // We don't handle this interface...
}

//==============================================================================

void DragAndDropViewPlugin::saveSettings(QSettings &pSettings) const
{
    Q_UNUSED(pSettings)

    // We don't handle this interface...
}

//==============================================================================

void DragAndDropViewPlugin::handleUrl(const QUrl &pUrl)
{
    Q_UNUSED(pUrl)

    // We don't handle this interface...
}

//==============================================================================
// View interface
//==============================================================================

ViewInterface::Mode DragAndDropViewPlugin::viewMode() const
{
    // Return our mode

    return ViewInterface::Mode::Test;
}

//==============================================================================

QStringList DragAndDropViewPlugin::viewMimeTypes() const
{
    // Return the MIME types we support, i.e. any in our case

    return {};
}

//==============================================================================

QString DragAndDropViewPlugin::viewMimeType(const QString &pFileName) const
{
    Q_UNUSED(pFileName)

    // Return the MIME type for the given file

    return {};
}

//==============================================================================

QString DragAndDropViewPlugin::viewDefaultFileExtension() const
{
    // Return the default file extension we support

    return {};
}

//==============================================================================

QWidget * DragAndDropViewPlugin::viewWidget(const QString &pFileName)
{
    Q_UNUSED(pFileName)

    // Return our drag & drop view widget

    return mViewWidget;
}

//==============================================================================

void DragAndDropViewPlugin::removeViewWidget(const QString &pFileName)
{
    Q_UNUSED(pFileName)

    // We don't handle this interface...
}

//==============================================================================

QString DragAndDropViewPlugin::viewName() const
{
    // Return our drag & drop view's name

    return tr("Drag && Drop");
}

//==============================================================================

QIcon DragAndDropViewPlugin::fileTabIcon(const QString &pFileName) const
{
    Q_UNUSED(pFileName)

    // We don't handle this interface...

    return {};
}

//==============================================================================

} // namespace DragAndDropView
} // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
