/*******************************************************************************

Copyright (C) The University of Auckland

OpenCOR is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OpenCOR is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*******************************************************************************/

//==============================================================================
// CellML file CellML 2.0 exporter
//==============================================================================

#pragma once

//==============================================================================

#include "cellmlfileexporter.h"

//==============================================================================

namespace OpenCOR {
namespace CellMLSupport {

//==============================================================================

class CellmlFileCellml20Exporter : public CellmlFileExporter
{
    Q_OBJECT

public:
    explicit CellmlFileCellml20Exporter(const QString &pOldFileName,
                                        const QString &pNewFileName);

private:
    QString mOutput;

private slots:
    void xslTransformationDone(const QString &pInput, const QString &pOutput);
};

//==============================================================================

}   // namespace CellMLSupport
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
