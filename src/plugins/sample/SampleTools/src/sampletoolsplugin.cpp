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
// Sample tools plugin
//==============================================================================

#include "coreguiutils.h"
#include "sampletoolsplugin.h"
#include "sampleutilities.h"

//==============================================================================

#include <QAction>
#include <QInputDialog>
#include <QMainWindow>

//==============================================================================

#include <iostream>

//==============================================================================

namespace OpenCOR {
namespace SampleTools {

//==============================================================================

PLUGININFO_FUNC SampleToolsPluginInfo()
{
    static const Descriptions descriptions = {
                                                 { "en", QString::fromUtf8("a plugin that provides an addition tool.") },
                                                 { "fr", QString::fromUtf8("une extension qui fournit un outil d'addition.") }
                                             };

    return new PluginInfo(PluginInfo::Category::Sample, true, true,
                          { "Core", "Sample" },
                          descriptions);
}

//==============================================================================
// CLI interface
//==============================================================================

bool SampleToolsPlugin::executeCommand(const QString &pCommand,
                                       const QStringList &pArguments, int &pRes)
{
    Q_UNUSED(pRes)

    // Run the given CLI command

    static const QString Help = "help";
    static const QString Add  = "add";

    if (pCommand == Help) {
        // Display the commands that we support

        runHelpCommand();

        return true;
    }

    if (pCommand == Add) {
        // Add some numbers

        return runAddCommand(pArguments);
    }

    // Not a CLI command that we support, so show our help and leave

    runHelpCommand();

    return false;
}

//==============================================================================
// GUI interface
//==============================================================================

void SampleToolsPlugin::updateGui(Plugin *pViewPlugin, const QString &pFileName)
{
    Q_UNUSED(pViewPlugin)
    Q_UNUSED(pFileName)

    // We don't handle this interface...
}

//==============================================================================

Gui::Menus SampleToolsPlugin::guiMenus() const
{
    // We don't handle this interface...

    return {};
}

//==============================================================================

Gui::MenuActions SampleToolsPlugin::guiMenuActions() const
{
    // Return our menu actions

    return Gui::MenuActions() << Gui::MenuAction(Gui::MenuAction::Type::Tools, mAddTwoNumbersAction)
                              << Gui::MenuAction(Gui::MenuAction::Type::Tools, Core::newSeparator(Core::mainWindow()));
}

//==============================================================================
// I18n interface
//==============================================================================

void SampleToolsPlugin::retranslateUi()
{
    // Retranslate our different Tools actions

    retranslateAction(mAddTwoNumbersAction, tr("Add Two Numbers..."), tr("Add two numbers together"));
}

//==============================================================================
// Plugin interface
//==============================================================================

bool SampleToolsPlugin::definesPluginInterfaces()
{
    // We don't handle this interface...

    return false;
}

//==============================================================================

bool SampleToolsPlugin::pluginInterfacesOk(const QString &pFileName,
                                           QObject *pInstance)
{
    Q_UNUSED(pFileName)
    Q_UNUSED(pInstance)

    // We don't handle this interface...

    return false;
}

//==============================================================================

void SampleToolsPlugin::initializePlugin()
{
    // Create our Add Two Numbers action

    mAddTwoNumbersAction = new QAction(Core::mainWindow());

    // A connection to handle our Add Two Numbers action

    connect(mAddTwoNumbersAction, &QAction::triggered,
            this, &SampleToolsPlugin::addTwoNumbers);
}

//==============================================================================

void SampleToolsPlugin::finalizePlugin()
{
    // We don't handle this interface...
}

//==============================================================================

void SampleToolsPlugin::pluginsInitialized(const Plugins &pLoadedPlugins)
{
    Q_UNUSED(pLoadedPlugins)

    // We don't handle this interface...
}

//==============================================================================

void SampleToolsPlugin::loadSettings(QSettings &pSettings)
{
    Q_UNUSED(pSettings)

    // We don't handle this interface...
}

//==============================================================================

void SampleToolsPlugin::saveSettings(QSettings &pSettings) const
{
    Q_UNUSED(pSettings)

    // We don't handle this interface...
}

//==============================================================================

void SampleToolsPlugin::handleUrl(const QUrl &pUrl)
{
    Q_UNUSED(pUrl)

    // We don't handle this interface...
}

//==============================================================================
// Plugin specific
//==============================================================================

void SampleToolsPlugin::runHelpCommand()
{
    // Output the commands we support

    std::cout << "Commands supported by the SampleTools plugin:" << std::endl;
    std::cout << " * Display the commands supported by the SampleTools plugin:" << std::endl;
    std::cout << "      help" << std::endl;
    std::cout << " * Add two numbers:" << std::endl;
    std::cout << "      add <nb1> <nb2>" << std::endl;
}

//==============================================================================

bool SampleToolsPlugin::runAddCommand(const QStringList &pArguments)
{
    // Make sure that we have the correct number of arguments

    if (pArguments.count() != 2) {
        runHelpCommand();

        return false;
    }

    // Make sure that the two arguments are valid numbers

    bool ok;

    double nb1 = pArguments.first().toDouble(&ok);

    if (!ok) {
        std::cout << "Sorry, but " << qPrintable(pArguments.first()) << " is not a valid number." << std::endl;

        return false;
    }

    double nb2 = pArguments.last().toDouble(&ok);

    if (!ok) {
        std::cout << "Sorry, but " << qPrintable(pArguments.last()) << " is not a valid number." << std::endl;

        return false;
    }

    // Add the two numbers and output the result

    std::cout << qPrintable(pArguments.first()) << " + " << qPrintable(pArguments.last()) << " = " << Sample::add(nb1, nb2) << std::endl;

    return true;
}

//==============================================================================

void SampleToolsPlugin::addTwoNumbers()
{
    bool ok;
    double nb1 = QInputDialog::getDouble(Core::mainWindow(), tr("Add Two Numbers"), tr("First number:"),
                                         0, -2147483647, 2147483647, 1, &ok);
    double nb2;

    if (ok) {
        nb2 = QInputDialog::getDouble(Core::mainWindow(), tr("Add Two Numbers"), tr("Second number:"),
                                      0, -2147483647, 2147483647, 1, &ok);

        if (ok) {
            Core::informationMessageBox(tr("Add Two Numbers"),
                                        QString::number(nb1)+" + "+QString::number(nb2)+" = "+QString::number(Sample::add(nb1, nb2)));
        }
    }
}

//==============================================================================

} // namespace SampleTools
} // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
