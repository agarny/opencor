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
// CellML file runtime
//==============================================================================

#pragma once

//==============================================================================

#include "cellmlfileissue.h"
#include "cellmlsupportglobal.h"

//==============================================================================

#include <QIcon>
#include <QList>
#ifdef Q_OS_WIN
    #include <QSet>
    #include <QVector>
#endif

//==============================================================================

#include "cellmlapibegin.h"
    #include "cellml-api-cxx-support.hpp"

    #include "IfaceCCGS.hxx"
    #include "IfaceCellML_APISPEC.hxx"
#include "cellmlapiend.h"

//==============================================================================

namespace OpenCOR {

//==============================================================================

namespace Compiler {
    class CompilerEngine;
}   // namespace Compiler

//==============================================================================

namespace CellMLSupport {

//==============================================================================

class CellmlFile;

//==============================================================================

class CELLMLSUPPORT_EXPORT CellmlFileRuntimeParameter
{
public:
    enum ParameterType {
        Voi,
        Constant,
        ComputedConstant,
        Rate,
        State,
        Algebraic,
        Floating,
        LocallyBound
    };

    explicit CellmlFileRuntimeParameter(const QString &pName,
                                        const int &pDegree,
                                        const QString &pUnit,
                                        const QStringList &pComponentHierarchy,
                                        const ParameterType &pType,
                                        const int &pIndex);

    static bool compare(CellmlFileRuntimeParameter *pParameter1,
                        CellmlFileRuntimeParameter *pParameter2);

    QString name() const;
    int degree() const;
    QString unit() const;
    QStringList componentHierarchy() const;
    ParameterType type() const;
    int index() const;

    QString formattedName() const;
    QString formattedComponentHierarchy() const;
    QString fullyFormattedName() const;

    QString formattedUnit(const QString &pVoiUnit) const;

    QIcon icon() const;

private:
    QString mName;
    int mDegree;
    QString mUnit;
    QStringList mComponentHierarchy;
    ParameterType mType;
    int mIndex;
};

//==============================================================================

typedef QList<CellmlFileRuntimeParameter *> CellmlFileRuntimeParameters;

//==============================================================================

class CELLMLSUPPORT_EXPORT CellmlFileRuntime : public QObject
{
    Q_OBJECT

public:
    enum ModelType {
        Ode,
        Dae
    };

    typedef void (*InitializeConstantsFunction)(double *CONSTANTS, double *RATES, double *STATES);
    typedef void (*ComputeComputedConstantsFunction)(double VOI, double *CONSTANTS, double *RATES, double *STATES, double *ALGEBRAIC);
    typedef void (*ComputeVariablesFunction)(double VOI, double *CONSTANTS, double *RATES, double *STATES, double *ALGEBRAIC, double *CONDVAR);

    typedef void (*ComputeOdeRatesFunction)(double VOI, double *CONSTANTS, double *RATES, double *STATES, double *ALGEBRAIC);

    typedef void (*ComputeDaeEssentialVariablesFunction)(double VOI, double *CONSTANTS, double *RATES, double *OLDRATES, double *STATES, double *OLDSTATES, double *ALGEBRAIC, double *CONDVAR);
    typedef void (*ComputeDaeResidualsFunction)(double VOI, double *CONSTANTS, double *RATES, double *OLDRATES, double *STATES, double *OLDSTATES, double *ALGEBRAIC, double *CONDVAR, double *resid);
    typedef void (*ComputeDaeRootInformationFunction)(double VOI, double *CONSTANTS, double *RATES, double *OLDRATES, double *STATES, double *OLDSTATES, double *ALGEBRAIC, double *CONDVAR);
    typedef void (*ComputeDaeStateInformationFunction)(double *SI);

    explicit CellmlFileRuntime(CellmlFile *pCellmlFile);
    ~CellmlFileRuntime();

    CellmlFile * cellmlFile();

    QString address() const;

    bool isValid() const;

    ModelType modelType() const;

    bool needOdeSolver() const;
    bool needDaeSolver() const;
    bool needNlaSolver() const;

    int constantsCount() const;
    int statesCount() const;
    int ratesCount() const;
    int algebraicCount() const;
    int condVarCount() const;

    InitializeConstantsFunction initializeConstants() const;
    ComputeComputedConstantsFunction computeComputedConstants() const;
    ComputeVariablesFunction computeVariables() const;

    ComputeOdeRatesFunction computeOdeRates() const;

    ComputeDaeEssentialVariablesFunction computeDaeEssentialVariables() const;
    ComputeDaeResidualsFunction computeDaeResiduals() const;
    ComputeDaeRootInformationFunction computeDaeRootInformation() const;
    ComputeDaeStateInformationFunction computeDaeStateInformation() const;

    CellmlFileIssues issues() const;

    CellmlFileRuntimeParameters parameters() const;

    void update();

    CellmlFileRuntimeParameter * variableOfIntegration() const;

private:
    CellmlFile *mCellmlFile;

    ModelType mModelType;
    bool mAtLeastOneNlaSystem;

    ObjRef<iface::cellml_services::CodeInformation> mOdeCodeInformation;
    ObjRef<iface::cellml_services::IDACodeInformation> mDaeCodeInformation;

    int mConstantsCount;
    int mStatesRatesCount;
    int mAlgebraicCount;
    int mCondVarCount;

    Compiler::CompilerEngine *mCompilerEngine;

    CellmlFileIssues mIssues;

    CellmlFileRuntimeParameter *mVariableOfIntegration;
    CellmlFileRuntimeParameters mParameters;

    InitializeConstantsFunction mInitializeConstants;
    ComputeComputedConstantsFunction mComputeComputedConstants;
    ComputeVariablesFunction mComputeVariables;

    ComputeOdeRatesFunction mComputeOdeRates;

    ComputeDaeEssentialVariablesFunction mComputeDaeEssentialVariables;
    ComputeDaeResidualsFunction mComputeDaeResiduals;
    ComputeDaeRootInformationFunction mComputeDaeRootInformation;
    ComputeDaeStateInformationFunction mComputeDaeStateInformation;

    void resetOdeCodeInformation();
    void resetDaeCodeInformation();

    void resetFunctions();

    void reset(const bool &pRecreateCompilerEngine, const bool &pResetIssues);

    void couldNotGenerateModelCodeIssue(const QString &pExtraInfo);
    void unknownProblemDuringModelCodeGenerationIssue();

    void checkCodeInformation(iface::cellml_services::CodeInformation *pCodeInformation);

    void retrieveOdeCodeInformation(iface::cellml_api::Model *pModel);
    void retrieveDaeCodeInformation(iface::cellml_api::Model *pModel);

    QString cleanCode(const std::wstring &pCode);
    QString methodCode(const QString &pCodeSignature, const QString &pCodeBody);
    QString methodCode(const QString &pCodeSignature,
                       const std::wstring &pCodeBody);

    QStringList componentHierarchy(iface::cellml_api::CellMLElement *pElement);
};

//==============================================================================

}   // namespace CellMLSupport
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
