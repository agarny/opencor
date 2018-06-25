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

#include "cellmlfilecellml20exporter.h"
#include "corecliutils.h"
#include "xsltransformer.h"

//==============================================================================

namespace OpenCOR {
namespace CellMLSupport {

//==============================================================================

CellmlFileCellml20Exporter::CellmlFileCellml20Exporter(const QString &pOldFileName,
                                                       const QString &pNewFileName) :
    CellmlFileExporter()
{
    // Create our XSL transformer

    Core::XslTransformer *xslTransformer = new Core::XslTransformer();

    // Determine whether we are dealing with a local/remote file, and retrieve
    // its contents

    bool isLocalFile;
    QString fileNameOrUrl;

    Core::checkFileNameOrUrl(pOldFileName, isLocalFile, fileNameOrUrl);

    QString fileContents;

    if (!(isLocalFile?
              Core::readFileContentsFromFile(fileNameOrUrl, fileContents):
              Core::readFileContentsFromUrl(fileNameOrUrl, fileContents))) {
        mErrorMessage = tr("the contents of the CellML file could not be read");

        return;
    }

    // Convert the given file to CellML 2.0 through an XSL transformation

    static const QString Xsl = Core::resource(":/CellMLSupport/cellml1to2.xsl");

    xslTransformer->transform(fileContents, Xsl);

    // Wait for the transformation to be done

    QEventLoop eventLoop;

    connect(xslTransformer, &Core::XslTransformer::done,
            this, &CellmlFileCellml20Exporter::xslTransformationDone);
    connect(xslTransformer, &Core::XslTransformer::done,
            &eventLoop, &QEventLoop::quit);

    eventLoop.exec();

    // Stop our XSL transformer
    // Note: we don't need to delete it since it will be done as part of its
    //       thread being stopped...

    xslTransformer->stop();

    // Our CellML 2.0 conversion is done, so save/output the output

    if (pNewFileName.isEmpty()) {
        std::wcout << mOutput.toStdWString() << std::endl;

        mResult = true;
    } else {
        mResult = Core::writeFileContentsToFile(pNewFileName, mOutput);
    }
}

//==============================================================================

void CellmlFileCellml20Exporter::xslTransformationDone(const QString &pInput,
                                                       const QString &pOutput)
{
    Q_UNUSED(pInput);

    // Our CellML 2.0 conversion is done, so keep track of it

    mOutput = pOutput;
    mOutput = mOutput.remove(" xmlns=\"\"");
}

//==============================================================================

}   // namespace CellMLSupport
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
