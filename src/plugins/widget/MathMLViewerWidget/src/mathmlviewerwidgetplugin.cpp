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
// MathML Viewer widget plugin
//==============================================================================

#include "mathmlviewerwidgetplugin.h"

//==============================================================================

namespace OpenCOR {
namespace MathMLViewerWidget {

//==============================================================================

PLUGININFO_FUNC MathMLViewerWidgetPluginInfo()
{
    static const Descriptions descriptions = {
                                                 { "en", QString::fromUtf8(R"(a plugin to visualise <a href="https://w3.org/Math">MathML</a> documents.)") },
                                                 { "fr", QString::fromUtf8(R"(une extension pour visualiser des documents <a href="https://w3.org/Math">MathML</a>.)") }
                                             };

    return new PluginInfo(PluginInfo::Category::Widget, false, false,
                          { "Core", "Qwt" },
                          descriptions);
}

//==============================================================================
// I18n interface
//==============================================================================

void MathMLViewerWidgetPlugin::retranslateUi()
{
    // We don't handle this interface...
    // Note: even though we don't handle this interface, we still want to
    //       support it since some other aspects of our plugin are
    //       multilingual...
}

//==============================================================================

} // namespace MathMLViewerWidget
} // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
