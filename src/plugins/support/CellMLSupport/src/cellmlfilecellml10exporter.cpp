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
// CellML file CellML 1.0 exporter
//==============================================================================
// Note: this exporter is adapted from David Nickerson's code which is an
//       updated version of Jonathan Cooper's CellML 1.1 to 1.0 converter. The
//       update was to make Jonathan's code work with version 1.12 of the CellML
//       API. In all cases, the original limitations apply...
//       Original code: https://cellml.org/tools/jonathan-cooper-s-cellml-1-1-to-1-0-converter/versionconverter-tar.bz2/view
//       Updated code: https://github.com/nickerso/flattenCellML
//==============================================================================

#include "cellmlfile.h"
#include "cellmlfilecellml10exporter.h"
#include "corecliutils.h"

//==============================================================================

#include <QApplication>

//==============================================================================

#include "cellmlapibegin.h"
    #include "AnnoToolsBootstrap.hpp"
    #include "CellMLBootstrap.hpp"
    #include "CeVASBootstrap.hpp"
#include "cellmlapiend.h"

//==============================================================================

namespace OpenCOR {
namespace CellMLSupport {

//==============================================================================

CellmlFileCellml10Exporter::CellmlFileCellml10Exporter(iface::cellml_api::Model *pModel,
                                                       const QString &pFileName) :
    mModel(pModel)
{
    // Create an empty CellML 1.0 model

    mExportedModel = CreateCellMLBootstrap()->createModel(L"1.0");

    // Set the model's name and cmeta:id, if any

    mExportedModel->name(pModel->name());

    std::wstring cmetaId = pModel->cmetaId();

    if (!cmetaId.empty()) {
        mExportedModel->cmetaId(validCmetaId(cmetaId));
    }

    // Create an annotation set to manage annotations

    mAnnotationSet = CreateAnnotationToolService()->createAnnotationSet();

    // Create a CellML Variable Association Service object so that we can find
    // relevant components

    ObjRef<iface::cellml_services::CeVAS> cevas = CreateCeVASBootstrap()->createCeVASForModel(pModel);

    std::wstring cevasError = cevas->modelError();

    if (!cevasError.empty()) {
        mErrorMessage = tr("CeVAS error: %1").arg(QString::fromStdWString(cevasError));

        return;
    }

    // Copy all (i.e. both local and imported) model-level units to our exported
    // model

    copyUnitsSet(pModel->allUnits(), mExportedModel);

    // Annotate imported components since they may have been renamed

    annotateImportedComponents(mModel);

    // Copy all needed components to our exported model

    copyComponents(cevas);

    // Copy all groups to our exported model

    copyGroups(mModel);

    // Copy all connections to our exported model

    copyConnections(mModel);

    // Deal with 'initial_value="var_name"' occurrences

    propagateInitialValues();

    // Save our exported model

    mResult = saveModel(mExportedModel, pFileName);
}

//==============================================================================

std::wstring CellmlFileCellml10Exporter::validCmetaId(const std::wstring pCmetaId)
{
    // Check whether the given id already exists and make it unique if needed

    QString res = QString::fromStdWString(pCmetaId);

    if (mCmetaIds.contains(res)) {
        do {
            res = QString("%1").arg(QString::number(mCmetaIdCounter, 16));

            ++mCmetaIdCounter;
        } while (mCmetaIds.contains(res));
    }

    mCmetaIds << res;

    return res.toStdWString();
}

//==============================================================================

void CellmlFileCellml10Exporter::copyUnitsSet(iface::cellml_api::UnitsSet *pUnitsSet,
                                              iface::cellml_api::CellMLElement *pElement)
{
    // Copy all the units to the model/component element
    // Note: the element is in a different model to the units, so we can't just
    //       clone them...

    ObjRef<iface::cellml_api::Model> model = pElement->modelElement();
    ObjRef<iface::cellml_api::UnitsIterator> unitsIter = pUnitsSet->iterateUnits();

    for (ObjRef<iface::cellml_api::Units> units = unitsIter->nextUnits();
         units != nullptr; units = unitsIter->nextUnits()) {
        // Copy the units to the model/component element, but only if it isn't
        // already in this model

        ObjRef<iface::cellml_api::Model> unitsModel = units->modelElement();

        if (CDA_objcmp(unitsModel, model) == 0) {
            continue;
        }

        // Only copy units which haven't already been copied

        std::wstring unitsName = units->name();
        QPair<QString, QString> unitsPair(QString::fromStdWString(unitsName),
                                          QString::fromStdWString(unitsModel->name()));

        if (mCopiedUnits.contains(unitsPair)) {
            continue;
        }

        // Keep track of the units pair

        mCopiedUnits << unitsPair;

        // Create a new units

        ObjRef<iface::cellml_api::Units> newUnits = model->createUnits();

        newUnits->name(unitsName);

        bool newUnitsIsBaseUnits = units->isBaseUnits();

        if (newUnitsIsBaseUnits) {
            newUnits->isBaseUnits(newUnitsIsBaseUnits);
        }

        // Copy the units' unit references to the new units

        ObjRef<iface::cellml_api::UnitIterator> unitIter = units->unitCollection()->iterateUnits();

        for (ObjRef<iface::cellml_api::Unit> unit = unitIter->nextUnit();
             unit != nullptr; unit = unitIter->nextUnit()) {
            // Create a new unit reference

            ObjRef<iface::cellml_api::Unit> newUnit = model->createUnit();

            newUnit->prefix(unit->prefix());
            newUnit->multiplier(unit->multiplier());
            newUnit->offset(unit->offset());
            newUnit->exponent(unit->exponent());
            newUnit->units(unit->units());

            // Add the unit reference to the new units

            newUnits->addElement(newUnit);
        }

        // Add the new units to the element

        pElement->addElement(newUnits);
    }
}

//==============================================================================

iface::cellml_api::CellMLComponent * CellmlFileCellml10Exporter::findRealComponent(iface::cellml_api::CellMLComponent *pComponent)
{
    // Find the 'real' component for the given component
    // Note #1: we use CeVAS's algorithm for this, i.e. if the component is an
    //          ImportComponent, then we return the template CellMLComponent
    //          that it is based on in an imported model, otherwise we return
    //          the given component. If no 'real' component exists, then we
    //          return 0...
    // Note #2: we require that a component is not imported more than once in
    //          the same import element, as then multiple components share one
    //          real component...

    ObjRef<iface::cellml_api::CellMLComponent> component = pComponent;

    for (ObjRef<iface::cellml_api::ImportComponent> importComponent = QueryInterface(component);
         importComponent != nullptr; importComponent = QueryInterface(component)) {
        ObjRef<iface::cellml_api::CellMLImport> import = QueryInterface(importComponent->parentElement());

        component = import->importedModel()->modelComponents()->getComponent(importComponent->componentRef());

        if (component == nullptr) {
            // No 'real' component exists

            break;
        }
    }

    if (component != nullptr) {
        component->add_ref();
    }

    return component;
}

//==============================================================================

void CellmlFileCellml10Exporter::annotateImportedComponents(iface::cellml_api::Model *pModel)
{
    // Recursively annotate all imported components with the name they are given
    // by the importing model

    ObjRef<iface::cellml_api::CellMLImportIterator> importsIter = pModel->imports()->iterateImports();

    for (ObjRef<iface::cellml_api::CellMLImport> import = importsIter->nextImport();
         import != nullptr; import = importsIter->nextImport()) {
        // Recursively annotate the components imported by the given model
        // Note: depth first processing means that imported components renamed
        //       twice end up with the right name...

        annotateImportedComponents(import->importedModel());

        // Go through the imported components and annotate them with the name
        // they are given by the importing model

        ObjRef<iface::cellml_api::ImportComponentIterator> importComponentsIter = import->components()->iterateImportComponents();

        for (ObjRef<iface::cellml_api::ImportComponent> importComponent = importComponentsIter->nextImportComponent();
             importComponent != nullptr; importComponent = importComponentsIter->nextImportComponent()) {
            mAnnotationSet->setStringAnnotation(findRealComponent(importComponent), L"renamed", importComponent->name());
        }
    }
}

//==============================================================================

void CellmlFileCellml10Exporter::ensureComponentNameUniqueness(std::wstring &pComponentName)
{
    // Make sure that the component name is unique within our exported model
    // Note: if the given component name is already used, then we append the
    //       string "_n" to it, where n is the least natural number giving an
    //       unused name...

    QString componentName = QString::fromStdWString(pComponentName);
    QString suffix;
    unsigned n = 0;

    while (mComponentNames.contains(componentName+suffix)) {
        // The current component name is already used, so generate a new suffix

        suffix = QString("_%1").arg(++n);
    }

    // We have got a valid suffix for our component name, so update the given
    // component name and keep track of it

    componentName += suffix;

    mComponentNames << componentName;

    pComponentName = componentName.toStdWString();
}

//==============================================================================

iface::dom::Element * CellmlFileCellml10Exporter::copyDomElement(iface::dom::Element *pDomElement)
{
    // Create and return a (manual) deep copy of the given DOM element
    // Note #1: if the DOM element is in the MathML namespace, then it will be
    //          converted to an iface::cellml_api::MathMLElement instance prior
    //          to being returned...
    // Note #2: it appears that when adding a child element or text node, which
    //          we are subsequently going to release_ref() ourselves, then we
    //          must also release_ref() the parent, or its refcount never
    //          reaches zero...

    // Create a blank DOM element

    iface::dom::Element *res = mExportedModel->createExtensionElement(pDomElement->namespaceURI(),
                                                                      pDomElement->localName());

    // Copy the DOM element attributes

    ObjRef<iface::dom::Document> document = res->ownerDocument();
    ObjRef<iface::dom::NamedNodeMap> attributes = pDomElement->attributes();

    for (uint i = 0, iMax = attributes->length(); i < iMax; ++i) {
        ObjRef<iface::dom::Attr> attr = QueryInterface(attributes->item(i));
        iface::dom::Attr *newAttr = document->createAttributeNS(Cellml_1_0_Namespace.toStdWString(), attr->name());

        newAttr->value(attr->value());

        ObjRef<iface::dom::Attr> dummyAttr = res->setAttributeNodeNS(newAttr);

        Q_UNUSED(dummyAttr)
    }

    // Copy the child element and text nodes

    ObjRef<iface::dom::NodeList> childNodes = pDomElement->childNodes();

    for (uint i = 0, iMax = childNodes->length(); i < iMax; ++i) {
        ObjRef<iface::dom::Node> childNode = childNodes->item(i);

        switch (childNode->nodeType()) {
        case iface::dom::Node::ELEMENT_NODE: {
            ObjRef<iface::dom::Element> elementNode = QueryInterface(childNode);

            res->appendChild(copyDomElement(elementNode))->release_ref();

            break;
        }
        case iface::dom::Node::TEXT_NODE:
            res->appendChild(document->createTextNode(childNode->nodeValue()))->release_ref();

            break;
        default:
            // Not a relevant type, so do nothing

            ;
        }
    }

    // We are all done, so return our copied DOM element

    res->add_ref();

    return res;
}

//==============================================================================

void CellmlFileCellml10Exporter::copyExtensionElements(iface::cellml_api::CellMLElement *pFromElement,
                                                       iface::cellml_api::CellMLElement *pToElement)
{
    // Copy any extension elements

    ObjRef<iface::cellml_api::ExtensionElementList> elements = pFromElement->extensionElements();
    ObjRef<iface::cellml_api::CellMLDOMElement> cellmlDomElement = QueryInterface(pToElement);
    ObjRef<iface::dom::Document> document = cellmlDomElement->domElement()->ownerDocument();

    for (uint i = 0, iMax = elements->length(); i < iMax; ++i) {
        ObjRef<iface::dom::Element> nodeElement = QueryInterface(document->importNode(elements->getAt(i), true));

        pToElement->appendExtensionElement(nodeElement);
    }
}

//==============================================================================

void CellmlFileCellml10Exporter::copyComponent(iface::cellml_api::CellMLComponent *pComponent)
{
    // Copy the given component into our exported model
    // Note: we create a new component, and manually copy the contents of the
    //       given component into it...

    // Make sure that the component hasn't already been copied

    ObjRef<iface::cellml_api::CellMLComponent> newComponent = QueryInterface(mAnnotationSet->getObjectAnnotation(pComponent, L"copy"));

    if (newComponent != nullptr) {
        return;
    }

    // Create a new component and make sure that the original component knows
    // about its existence

    newComponent = mExportedModel->createComponent();

    mAnnotationSet->setObjectAnnotation(pComponent, L"copy", newComponent);

    // Check whether the component has been renamed

    std::wstring newComponentName = pComponent->name();
    std::wstring componentRenamedName = mAnnotationSet->getStringAnnotation(pComponent, L"renamed");

    if (!componentRenamedName.empty()) {
        // The component was imported, so it may have been renamed

        newComponentName = componentRenamedName;
    }

    // Make sure that the name of our new component is unique within our
    // exported model

    ensureComponentNameUniqueness(newComponentName);

    // Set both the name and cmeta:id, if any, of our new component

    newComponent->name(newComponentName);

    std::wstring newComponentCmetaId = pComponent->cmetaId();

    if (!newComponentCmetaId.empty()) {
        newComponent->cmetaId(validCmetaId(newComponentCmetaId));
    }

    // Copy the units to our new component

    copyUnitsSet(pComponent->units(), newComponent);

    // Copy the variables to our new component

    ObjRef<iface::cellml_api::CellMLVariableIterator> componentVariablesIter = pComponent->variables()->iterateVariables();

    for (ObjRef<iface::cellml_api::CellMLVariable> componentVariable = componentVariablesIter->nextVariable();
         componentVariable != nullptr; componentVariable = componentVariablesIter->nextVariable()) {
        // Create a new variable and copy its attributes from the original
        // variable

        ObjRef<iface::cellml_api::CellMLVariable> newComponentVariable = mExportedModel->createCellMLVariable();

        newComponentVariable->name(componentVariable->name());

        std::wstring componentVariableCmetaId = componentVariable->cmetaId();

        if (!componentVariableCmetaId.empty()) {
            newComponentVariable->cmetaId(validCmetaId(componentVariableCmetaId));
        }

        newComponentVariable->initialValue(componentVariable->initialValue());
        newComponentVariable->unitsName(componentVariable->unitsName());
        newComponentVariable->publicInterface(componentVariable->publicInterface());
        newComponentVariable->privateInterface(componentVariable->privateInterface());

        // Copy our new variable to our new component

        newComponent->addElement(newComponentVariable);
    }

    // Copy the math nodes to our new component

    ObjRef<iface::cellml_api::MathMLElementIterator> componentMathIter = pComponent->math()->iterate();

    for (ObjRef<iface::mathml_dom::MathMLElement> componentMathNode = componentMathIter->next();
         componentMathNode != nullptr; componentMathNode = componentMathIter->next()) {
        // Copy a clone of the math node into our new component

        ObjRef<iface::mathml_dom::MathMLElement> newComponentMathNode = QueryInterface(copyDomElement(componentMathNode));

        newComponent->addMath(newComponentMathNode);
    }

    // Copy the extension elements to our new component

    copyExtensionElements(pComponent, newComponent);

    // Add the new component to our exported model

    mExportedModel->addElement(newComponent);
}

//==============================================================================

void CellmlFileCellml10Exporter::copyComponents(iface::cellml_services::CeVAS *pCevas)
{
    // Copy the relevant components into our exported model, using the given
    // CeVAS to find components to copy

    ObjRef<iface::cellml_api::CellMLComponentIterator> componentsIter = pCevas->iterateRelevantComponents();

    for (ObjRef<iface::cellml_api::CellMLComponent> component = componentsIter->nextComponent();
         component != nullptr; component = componentsIter->nextComponent()) {
        copyComponent(component);
    }
}

//==============================================================================

void CellmlFileCellml10Exporter::copyGroup(iface::cellml_api::Model *pModel,
                                           iface::cellml_api::ComponentRefSet *pComponentReferences,
                                           iface::cellml_api::ComponentRef *pToComponentReference)
{
    // Iterate this level of the tree

    ObjRef<iface::cellml_api::CellMLComponentSet> components = pModel->modelComponents();
    ObjRef<iface::cellml_api::ComponentRefIterator> componentReferencesIter = pComponentReferences->iterateComponentRefs();

    for (ObjRef<iface::cellml_api::ComponentRef> componentReference = componentReferencesIter->nextComponentRef();
         componentReference != nullptr; componentReference = componentReferencesIter->nextComponentRef()) {
        // Find the referenced component

        ObjRef<iface::cellml_api::CellMLComponent> referencedComponent = components->getComponent(componentReference->componentName());

        if (referencedComponent == nullptr) {
            continue;
        }

        // Find the real component object and check whether it has been copied

        ObjRef<iface::cellml_api::CellMLComponent> copiedComponent = QueryInterface(mAnnotationSet->getObjectAnnotation(findRealComponent(referencedComponent), L"copy"));

        if (copiedComponent == nullptr) {
            continue;
        }

        // Create a component reference for the copied component

        ObjRef<iface::cellml_api::ComponentRef> newComponentReference = mExportedModel->createComponentRef();

        newComponentReference->componentName(copiedComponent->name());

        if (pToComponentReference != nullptr) {
            // Add our new component reference to the given group

            pToComponentReference->addElement(newComponentReference);
        } else {
            // Create a new root group, if needed

            if (mRootGroup == nullptr) {
                mRootGroup = mExportedModel->createGroup();

                mExportedModel->addElement(mRootGroup);

                ObjRef<iface::cellml_api::RelationshipRef> relationshipReference = mExportedModel->createRelationshipRef();

                relationshipReference->setRelationshipName(L"", L"encapsulation");

                mRootGroup->addElement(relationshipReference);
            }

            // Add our new component reference to our root group

            mRootGroup->addElement(newComponentReference);
        }

        // Copy any children of our component reference

        copyGroup(pModel, componentReference->componentRefs(), newComponentReference);
    }
}

//==============================================================================

void CellmlFileCellml10Exporter::copyGroups(iface::cellml_api::Model *pModel)
{
    // Iterate only groups defining an encapsulation hierarchy and copy them
    // into our exported model

    ObjRef<iface::cellml_api::GroupIterator> encapsulationGroupsIter = pModel->groups()->subsetInvolvingEncapsulation()->iterateGroups();

    for (ObjRef<iface::cellml_api::Group> encapsulationGroup = encapsulationGroupsIter->nextGroup();
         encapsulationGroup != nullptr; encapsulationGroup = encapsulationGroupsIter->nextGroup()) {
        copyGroup(pModel, encapsulationGroup->componentRefs());
    }

    // Recursively process imported models

    ObjRef<iface::cellml_api::CellMLImportIterator> importsIter = pModel->imports()->iterateImports();

    for (ObjRef<iface::cellml_api::CellMLImport> import = importsIter->nextImport();
         import != nullptr; import = importsIter->nextImport()) {
        copyGroups(import->importedModel());
    }
}

//==============================================================================

void CellmlFileCellml10Exporter::copyConnection(iface::cellml_api::Connection *pConnection)
{
    // Copy a connection, possibly involving imported components, into our
    // exported model
    // Note: the connection will only be copied if both components involved have
    //       been copied across first, and hence have a "copy" annotation...

    ObjRef<iface::cellml_api::MapComponents> componentMapping = pConnection->componentMapping();

    // Check that we have copied the components involved, and get the copies

    ObjRef<iface::cellml_api::CellMLComponent> newFirstComponent = QueryInterface(mAnnotationSet->getObjectAnnotation(componentMapping->firstComponent(), L"copy"));

    if (newFirstComponent == nullptr) {
        return;
    }

    ObjRef<iface::cellml_api::CellMLComponent> newSecondComponent = QueryInterface(mAnnotationSet->getObjectAnnotation(componentMapping->secondComponent(), L"copy"));

    if (newSecondComponent == nullptr) {
        return;
    }

    // Create a new connection

    ObjRef<iface::cellml_api::Connection> newConnection = mExportedModel->createConnection();

    mExportedModel->addElement(newConnection);

    // Create a new component mapping

    ObjRef<iface::cellml_api::MapComponents> newComponentMapping = newConnection->componentMapping();

    newComponentMapping->firstComponentName(newFirstComponent->name());
    newComponentMapping->secondComponentName(newSecondComponent->name());

    // Add the variable mappings

    ObjRef<iface::cellml_api::MapVariablesIterator> variableMappingsIter = pConnection->variableMappings()->iterateMapVariables();

    for (ObjRef<iface::cellml_api::MapVariables> variableMapping = variableMappingsIter->nextMapVariables();
         variableMapping != nullptr; variableMapping = variableMappingsIter->nextMapVariables()) {
        ObjRef<iface::cellml_api::MapVariables> newVariableMapping = mExportedModel->createMapVariables();

        newVariableMapping->firstVariableName(variableMapping->firstVariableName());
        newVariableMapping->secondVariableName(variableMapping->secondVariableName());

        newConnection->addElement(newVariableMapping);
    }
}

//==============================================================================

void CellmlFileCellml10Exporter::copyConnections(iface::cellml_api::Model *pModel)
{
    // Copy local connections into our exported model

    ObjRef<iface::cellml_api::ConnectionIterator> connectionsIter = pModel->connections()->iterateConnections();

    for (ObjRef<iface::cellml_api::Connection> connection = connectionsIter->nextConnection();
         connection != nullptr; connection = connectionsIter->nextConnection()) {
        copyConnection(connection);
    }

    // Recursively process imported models

    ObjRef<iface::cellml_api::CellMLImportIterator> importsIter = pModel->imports()->iterateImports();

    for (ObjRef<iface::cellml_api::CellMLImport> import = importsIter->nextImport();
         import != nullptr; import = importsIter->nextImport()) {
        copyConnections(import->importedModel());
    }
}

//==============================================================================

void CellmlFileCellml10Exporter::propagateInitialValues()
{
    // Try to make all 'initial_value' attributes valid CellML 1.0
    // Note: where a variable is specified, we look at its source variable. If
    //       it has a numeric initial value, the we use that. If not, then it's
    //       an unavoidable error condition...

    ObjRef<iface::cellml_api::CellMLComponentIterator> componentsIter = mExportedModel->localComponents()->iterateComponents();

    for (ObjRef<iface::cellml_api::CellMLComponent> component = componentsIter->nextComponent();
         component != nullptr; component = componentsIter->nextComponent()) {
        // Go through the component's variables and update their initial value

        ObjRef<iface::cellml_api::CellMLVariableSet> componentVariables = component->variables();
        ObjRef<iface::cellml_api::CellMLVariableIterator> componentVariablesIter = componentVariables->iterateVariables();

        for (ObjRef<iface::cellml_api::CellMLVariable> componentVariable = componentVariablesIter->nextVariable();
             componentVariable != nullptr; componentVariable = componentVariablesIter->nextVariable()) {
            // Check whether the variable has an initial value

            std::wstring initialValue = componentVariable->initialValue();

            if (initialValue.empty()) {
                continue;
            }

            // Is the initial value a double?

            bool doubleInitialValueValid;

            QString::fromStdWString(initialValue).toDouble(&doubleInitialValueValid);

            if (!doubleInitialValueValid) {
                // The variable is initialised with the initial value of another
                // variable, so retrieve that 'initial' variable, find its
                // source and copy its initial value

                componentVariable->initialValue(componentVariables->getVariable(initialValue)->sourceVariable()->initialValue());
            }
        }
    }
}

//==============================================================================

bool CellmlFileCellml10Exporter::saveModel(iface::cellml_api::Model *pModel,
                                           const QString &pFileName)
{
    // Save the given model or output it to the console, if no file name has
    // been provided, and this after having reformatted the given model
    // Note: for some reasons, the MathML type of e-notation based numbers gets
    //       moved to the CellML namespace, so we revert that behaviour...

    QDomDocument domDocument;

    domDocument.setContent(QString::fromStdWString(pModel->serialisedText()));

    QByteArray serialisedDomDocument = Core::serialiseDomDocument(domDocument);

    serialisedDomDocument.replace(" cellml:xmlns=\"http://www.w3.org/1998/Math/MathML\"", "");
    serialisedDomDocument.replace("cellml:type=\"e-notation\"", "type=\"e-notation\"");
    serialisedDomDocument.replace("cellml:id", "id");

    if (pFileName.isEmpty()) {
        std::cout << QString(serialisedDomDocument).toStdString() << std::endl;

        return true;
    }

    return Core::writeFile(pFileName, serialisedDomDocument);
}

//==============================================================================

} // namespace CellMLSupport
} // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
