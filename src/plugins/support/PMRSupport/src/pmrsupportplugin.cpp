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
// PMR support plugin
//==============================================================================

#include "coreguiutils.h"
#include "pmrsupportplugin.h"
#include "pmrsupportpreferenceswidget.h"

//==============================================================================

#include <QMainWindow>

//==============================================================================

namespace OpenCOR {
namespace PMRSupport {

//==============================================================================

PLUGININFO_FUNC PMRSupportPluginInfo()
{
    static const Descriptions descriptions = {
                                                 { "en", QString::fromUtf8(R"(a plugin to support <a href="https://models.physiomeproject.org/">PMR</a>.)") },
                                                 { "fr", QString::fromUtf8(R"(une extension pour supporter <a href="https://models.physiomeproject.org/">PMR</a>.)") }
                                             };

    return new PluginInfo(PluginInfo::Category::Support, false, false,
                          { "Core", "libgit2", "OAuth", "WebViewerWidget" },
                          descriptions);
}

//==============================================================================
// I18n interface
//==============================================================================

void PMRSupportPlugin::retranslateUi()
{
    // We don't handle this interface...
    // Note: even though we don't handle this interface, we still want to
    //       support it since some other aspects of our plugin are
    //       multilingual...
}

//==============================================================================
// Preferences interface
//==============================================================================

Preferences::PreferencesWidget * PMRSupportPlugin::preferencesWidget()
{
    // Return our preferences widget

    return new PmrSupportPreferencesWidget(Core::mainWindow());
}

//==============================================================================

void PMRSupportPlugin::preferencesChanged(const QStringList &pPluginNames)
{
    Q_UNUSED(pPluginNames)

    // We don't handle this interface...
}

//==============================================================================

} // namespace PMRSupport
} // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
