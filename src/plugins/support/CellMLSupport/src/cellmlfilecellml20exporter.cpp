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

#include "cellmlfile.h"
#include "cellmlfilecellml20exporter.h"
#include "corecliutils.h"
#include "xsltransformer.h"

//==============================================================================

namespace OpenCOR {
namespace CellMLSupport {

//==============================================================================

CellmlFileCellml20Exporter::CellmlFileCellml20Exporter(const QString &pOldFileName,
                                                       const QString &pNewFileName) :
    mOutput(QString())
{
    // Create our XSL transformer

    auto xslTransformer = new Core::XslTransformer();

    // Determine whether we are dealing with a local/remote file, and retrieve
    // its contents

    bool isLocalFile;
    QString fileNameOrUrl;

    Core::checkFileNameOrUrl(pOldFileName, isLocalFile, fileNameOrUrl);

    QString fileContents;

    if (!Core::readFile(fileNameOrUrl, fileContents)) {
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

    // Our CellML 2.0 conversion is done, so save/output the output

    if (pNewFileName.isEmpty()) {
        std::wcout << mOutput.toStdWString() << std::endl;

        mResult = true;
    } else {
        mResult = Core::writeFile(pNewFileName, mOutput);
    }
}

//==============================================================================

void cleanUnitsAttributes(QDomElement *pDomElement, QDomElement *pDomElementNs,
                          bool *pAtLeastOneUnitsAttribute)
{
    // Clean up the current CellML element by ensuring that its "units"
    // attribute, if any, is in the CellML namespace

    if (pDomElementNs->namespaceURI().compare(MathmlNamespace) == 0) {
        static const QString CellmlAttributeName  = "cellml:%1";
        static const QString CellmlUnitsAttribute = "units";

        QDomNamedNodeMap attributes = pDomElement->attributes();

        for (int i = 0, iMax = attributes.count(); i < iMax; ++i) {
            QDomNode attribute = attributes.item(i);

            if (attribute.nodeName().compare(CellmlUnitsAttribute) == 0) {
                QString attributeValue = attribute.nodeValue();

                pDomElement->removeAttribute(CellmlUnitsAttribute);
                pDomElement->setAttributeNS(Cellml_2_0_Namespace, CellmlAttributeName.arg(CellmlUnitsAttribute), attributeValue);

                *pAtLeastOneUnitsAttribute = true;

                break;
            }
        }
    }

    // Do the same for the element's child elements

    for (QDomElement childElement = pDomElement->firstChildElement(),
                     childElementNs = pDomElementNs->firstChildElement();
         !childElement.isNull() && !childElementNs.isNull();
         childElement = childElement.nextSiblingElement(),
         childElementNs = childElementNs.nextSiblingElement()) {
        cleanUnitsAttributes(&childElement, &childElementNs, pAtLeastOneUnitsAttribute);
    }
}

//==============================================================================

void cleanCellmlNamespace(QDomElement *pDomElement, QDomElement *pDomElementNs)
{
    // Clean up the current CellML element by removing its CellML namespace
    // information, if any

    QDomNamedNodeMap attributes = pDomElement->attributes();

    if (pDomElementNs->namespaceURI().compare(MathmlNamespace) == 0) {
        static const QString CellmlNamespaceAttribute = "xmlns:cellml";

        pDomElement->removeAttribute(CellmlNamespaceAttribute);
    }

    // Do the same for the element's child elements

    for (QDomElement childElement = pDomElement->firstChildElement(),
                     childElementNs = pDomElementNs->firstChildElement();
         !childElement.isNull() && !childElementNs.isNull();
         childElement = childElement.nextSiblingElement(),
         childElementNs = childElementNs.nextSiblingElement()) {
        cleanCellmlNamespace(&childElement, &childElementNs);
    }
}

//==============================================================================

void CellmlFileCellml20Exporter::xslTransformationDone(const QString &pInput,
                                                       const QString &pOutput)
{
    Q_UNUSED(pInput);

    // Our CellML 2.0 conversion is done, so keep track of it and clean it a bit

    mOutput = "<?xml version='1.0' encoding='UTF-8'?>\n"+pOutput;
    mOutput = mOutput.remove(" xmlns=\"\"");

    // Clean up the resulting CellML document
    // Note #1: indeed, our XSL transformation (because of Qt) is not perfect,
    //          so we need to patch things up by 1) ensuring that the "units"
    //          attribute, if any, of all MathML elements is in the CellML
    //          namespace, 2) adding the "xmlns:cellml" attribute to our model
    //          element, if needed, and 3) removing the CellML namespace
    //          information, if any, from all MathML elements (we need to do
    //          this because step #1 automatically adds this to our DOM!)...
    // Note #2: we are doing this using two QDomDocument objects, one that
    //          processes namespaces and another that doesn't. Ideally, we would
    //          only use the one that processes namespaces, but this results in
    //          having namespace information all over the place. So, in the end,
    //          we use the first one to determine what needs to be done to the
    //          second one...

    QDomDocument domDocument;
    QDomDocument domDocumentNs;

    if (   domDocument.setContent(mOutput)
        && domDocumentNs.setContent(mOutput, true)) {
        bool atLeastOneUnitsAttribute = false;

        for (QDomElement childElement = domDocument.firstChildElement(),
                         childElementNs = domDocumentNs.firstChildElement();
             !childElement.isNull() && !childElementNs.isNull();
             childElement = childElement.nextSiblingElement(),
             childElementNs = childElementNs.nextSiblingElement()) {
            cleanUnitsAttributes(&childElement, &childElementNs, &atLeastOneUnitsAttribute);
        }

        if (atLeastOneUnitsAttribute) {
            domDocument.documentElement().setAttribute("xmlns:cellml", Cellml_2_0_Namespace);
        }

        mOutput = Core::serialiseDomDocument(domDocument);

        if (   domDocument.setContent(mOutput)
            && domDocumentNs.setContent(mOutput, true)) {
            for (QDomElement childElement = domDocument.firstChildElement(),
                             childElementNs = domDocumentNs.firstChildElement();
                 !childElement.isNull() && !childElementNs.isNull();
                 childElement = childElement.nextSiblingElement(),
                 childElementNs = childElementNs.nextSiblingElement()) {
                cleanCellmlNamespace(&childElement, &childElementNs);
            }

            mOutput = Core::serialiseDomDocument(domDocument);
        }
    }
}

//==============================================================================

}   // namespace CellMLSupport
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
