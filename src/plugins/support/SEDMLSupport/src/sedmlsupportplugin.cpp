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
// SED-ML support plugin
//==============================================================================

#include "corecliutils.h"
#include "coreguiutils.h"
#include "filemanager.h"
#include "sedmlfilemanager.h"
#include "sedmlinterface.h"
#include "sedmlsupportplugin.h"

//==============================================================================

#include <QAction>
#include <QMainWindow>

//==============================================================================

namespace OpenCOR {
namespace SEDMLSupport {

//==============================================================================

PLUGININFO_FUNC SEDMLSupportPluginInfo()
{
    static const Descriptions descriptions = {
                                                 { "en", QString::fromUtf8(R"(a plugin to support <a href="https://sed-ml.org/">SED-ML</a>.)") },
                                                 { "fr", QString::fromUtf8(R"(une extension pour supporter <a href="https://sed-ml.org/">SED-ML</a>.)") }
                                             };

    return new PluginInfo(PluginInfo::Category::Support, false, false,
                          { "CellMLSupport", "libSEDML" },
                          descriptions);
}

//==============================================================================

SEDMLSupportPlugin::SEDMLSupportPlugin()
{
    // Keep track of our file type interface

    static SedmlInterfaceData data(qobject_cast<FileTypeInterface *>(this));

    Core::globalInstance(SedmlInterfaceDataSignature, &data);
}

//==============================================================================
// File interface
//==============================================================================

bool SEDMLSupportPlugin::isFile(const QString &pFileName) const
{
    // Return whether the given file is of the type that we support

    return SedmlFileManager::instance()->isFile(pFileName);
}

//==============================================================================

QString SEDMLSupportPlugin::mimeType() const
{
    // Return the MIME type we support

    return SedmlMimeType;
}

//==============================================================================

QString SEDMLSupportPlugin::fileExtension() const
{
    // Return the extension of the type of file we support

    return SedmlFileExtension;
}

//==============================================================================

QString SEDMLSupportPlugin::fileTypeDescription() const
{
    // Return the description of the type of file we support

    return tr("SED-ML File");
}

//==============================================================================

QStringList SEDMLSupportPlugin::fileTypeDefaultViews() const
{
    // Return the default views to use for the type of file we support

    return { "SimulationExperimentView", "RawSEDMLView", "RawTextView" };
}

//==============================================================================
// GUI interface
//==============================================================================

void SEDMLSupportPlugin::updateGui(Plugin *pViewPlugin, const QString &pFileName)
{
    Q_UNUSED(pViewPlugin)
    Q_UNUSED(pFileName)

    // We don't handle this interface...
}

//==============================================================================

Gui::Menus SEDMLSupportPlugin::guiMenus() const
{
    // We don't handle this interface...

    return {};
}

//==============================================================================

Gui::MenuActions SEDMLSupportPlugin::guiMenuActions() const
{
    // Return our menu actions

    return Gui::MenuActions() << Gui::MenuAction(Gui::MenuAction::Type::FileNew, mFileNewSedmlFileAction);
}

//==============================================================================
// I18n interface
//==============================================================================

void SEDMLSupportPlugin::retranslateUi()
{
    // Retranslate our different actions

    retranslateAction(mFileNewSedmlFileAction, tr("SED-ML File"),
                      tr("Create a new SED-ML file"));
}

//==============================================================================
// Plugin interface
//==============================================================================

bool SEDMLSupportPlugin::definesPluginInterfaces()
{
    // We don't handle this interface...

    return false;
}

//==============================================================================

bool SEDMLSupportPlugin::pluginInterfacesOk(const QString &pFileName,
                                            QObject *pInstance)
{
    Q_UNUSED(pFileName)
    Q_UNUSED(pInstance)

    // We don't handle this interface...

    return false;
}

//==============================================================================

void SEDMLSupportPlugin::initializePlugin()
{
    // Create our different actions

    mFileNewSedmlFileAction = Core::newAction(Core::mainWindow());

    // Some connections to handle our different actions

    connect(mFileNewSedmlFileAction, &QAction::triggered,
            this, &SEDMLSupportPlugin::newSedmlFile);
}

//==============================================================================

void SEDMLSupportPlugin::finalizePlugin()
{
    // We don't handle this interface...
}

//==============================================================================

void SEDMLSupportPlugin::pluginsInitialized(const Plugins &pLoadedPlugins)
{
    Q_UNUSED(pLoadedPlugins)

    // Make a call to the instance of the SED-ML file manager so that it gets
    // properly set up (and therefore can start managing SED-ML files) before it
    // actually gets used by other plugins
    // Note: we do it here rather than in initializePlugin() since we need the
    //       Core plugin to be initialised (so we can get access to our 'global'
    //       file manager)...

    SedmlFileManager::instance();
}

//==============================================================================

void SEDMLSupportPlugin::loadSettings(QSettings &pSettings)
{
    Q_UNUSED(pSettings)

    // We don't handle this interface...
}

//==============================================================================

void SEDMLSupportPlugin::saveSettings(QSettings &pSettings) const
{
    Q_UNUSED(pSettings)

    // We don't handle this interface...
}

//==============================================================================

void SEDMLSupportPlugin::handleUrl(const QUrl &pUrl)
{
    Q_UNUSED(pUrl)

    // We don't handle this interface...
}

//==============================================================================
// Plugin specific
//==============================================================================

void SEDMLSupportPlugin::newSedmlFile()
{
    // Create a new SED-ML file by asking our file manager to create one

    Core::FileManager *fileManagerInstance = Core::FileManager::instance();
#ifdef QT_DEBUG
    Core::FileManager::Status createStatus =
#endif
    fileManagerInstance->create({},
                                QString( "<?xml version='1.0' encoding='UTF-8'?>\n"
                                        R"(<sedML level="1" version="2" xmlns="http://sed-ml.org/sed-ml/level1/version2">)""\n"
                                         "    <!-- Your code goes here-->\n"
                                         "</sedML>\n"));

#ifdef QT_DEBUG
    // Make sure that the file has indeed been created

    if (createStatus != Core::FileManager::Status::Created) {
        qFatal("FATAL ERROR | %s:%d: the new SED-ML file could not be created.", __FILE__, __LINE__);
    }
#endif
}

//==============================================================================

} // namespace SEDMLSupport
} // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
