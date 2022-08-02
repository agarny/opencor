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
// Sample plugin
//==============================================================================

#include "sampleplugin.h"

//==============================================================================

namespace OpenCOR {
namespace Sample {

//==============================================================================

PLUGININFO_FUNC SamplePluginInfo()
{
    static const Descriptions descriptions = {
                                                 { "en", QString::fromUtf8("a plugin that provides an addition function.") },
                                                 { "fr", QString::fromUtf8("une extension qui fournit une fonction d'addition.") }
                                             };

    return new PluginInfo(PluginInfo::Category::Sample, false, false,
                          {},
                          descriptions);
}

//==============================================================================

} // namespace Sample
} // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
