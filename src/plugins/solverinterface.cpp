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
// Solver interface
//==============================================================================

#include "solverinterface.h"

//==============================================================================

void doNonLinearSolve(char *pRuntime,
                      void (*pFunction)(double *, double *, void *),
                      double *pParameters, int pSize, void *pUserData)
{
    // Retrieve the NLA solver which we should use and solve our NLA system
    // Note: we should always have an NLA solver, but better be safe than
    //       sorry...

    OpenCOR::Solver::NlaSolver *nlaSolver = OpenCOR::Solver::nlaSolver(pRuntime);

    if (nlaSolver != nullptr) {
        nlaSolver->solve(pFunction, pParameters, pSize, pUserData);
    } else {
        qWarning("WARNING | %s:%d: no NLA solver could be found.", __FILE__, __LINE__);
    }
}

//==============================================================================

namespace OpenCOR {

//==============================================================================

extern "C" Q_DECL_EXPORT int solverInterfaceVersion()
{
    // Version of the solver interface

    return 2;
}

//==============================================================================

namespace Solver {

//==============================================================================

void Solver::setProperties(const Properties &pProperties)
{
    // Set a property's value, but only if it is a valid property

    mProperties = pProperties;
}

//==============================================================================

void Solver::emitError(const QString &pErrorMessage)
{
    // Let people know that an error occurred, but first reformat the error a
    // bit, if needed

    QString errorMessage;

    if (pErrorMessage.startsWith("Newton")) {
        errorMessage = pErrorMessage;
    } else if (!pErrorMessage.isEmpty()) {
        errorMessage = pErrorMessage[0].toLower()+pErrorMessage.right(pErrorMessage.size()-1);
    }

    static const QString Dot = ".";
    static const QString DotDotDot = "...";

    if (errorMessage.right(3) == DotDotDot) {
        emit error(errorMessage.left(errorMessage.size()-3));
    } else if (errorMessage.right(1) == Dot) {
        emit error(errorMessage.left(errorMessage.size()-1));
    } else {
        emit error(errorMessage);
    }
}

//==============================================================================

void OdeSolver::initialize(double pVoi, int pRatesStatesCount,
                           double *pConstants, double *pRates, double *pStates,
                           double *pAlgebraic,
                           ComputeRatesFunction pComputeRates)
{
    Q_UNUSED(pVoi)

    // Initialise the ODE solver

    mRatesStatesCount = pRatesStatesCount;

    mConstants = pConstants;
    mRates = pRates;
    mStates = pStates;
    mAlgebraic = pAlgebraic;

    mComputeRates = pComputeRates;
}

//==============================================================================

void OdeSolver::reinitialize(double pVoi)
{
    Q_UNUSED(pVoi)

    // Nothing to do by default...
}

//==============================================================================

NlaSolver::~NlaSolver() = default;

//==============================================================================

QString objectAddress(QObject *pObject)
{
    // Return the given object's address as a string

    return QString::number(quint64(pObject));
}

//==============================================================================

NlaSolver * nlaSolver(const QString &pObjectAddress)
{
    // Return the runtime's NLA solver

    QObject *object = reinterpret_cast<NlaSolver *>(QVariant(pObjectAddress).toULongLong());

    return reinterpret_cast<NlaSolver *>(QVariant(object->objectName()).toULongLong());
}

//==============================================================================

void setNlaSolver(QObject *pObject, NlaSolver *pNlaSolver)
{
    // Keep track of the runtime's NLA solver
    // Note: normally, we would use QObject::setProperty(), but it doesn't play
    //       well on Windows without a GUI (see https://bit.ly/2YCpzew). So,
    //       instead, we use QObject::setObjectName() (!!)...

    pObject->setObjectName(objectAddress(pNlaSolver));
}

//==============================================================================

Property::Property(Type pType, const QString &pId,
                   const Descriptions &pDescriptions,
                   const QStringList &pListValues,
                   const QVariant &pDefaultValue, bool pHasVoiUnit) :
    mType(pType),
    mId(pId),
    mDescriptions(pDescriptions),
    mListValues(pListValues),
    mDefaultValue(pDefaultValue),
    mHasVoiUnit(pHasVoiUnit)
{
}

//==============================================================================

Property::Type Property::type() const
{
    // Return our type

    return mType;
}

//==============================================================================

QString Property::id() const
{
    // Return our id

    return mId;
}

//==============================================================================

Descriptions Property::descriptions() const
{
    // Return our descriptions

    return mDescriptions;
}

//==============================================================================

QStringList Property::listValues() const
{
    // Return our list values

    return mListValues;
}

//==============================================================================

QVariant Property::defaultValue() const
{
    // Return our default value

    return mDefaultValue;
}

//==============================================================================

bool Property::hasVoiUnit() const
{
    // Return whether we have a VOI unit

    return mHasVoiUnit;
}

//==============================================================================

} // namespace Solver

//==============================================================================

SolverInterface::~SolverInterface() = default;

//==============================================================================

} // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
