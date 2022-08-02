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
// KINSOL solver plugin
//==============================================================================

#include "kinsolsolver.h"
#include "kinsolsolverplugin.h"

//==============================================================================

namespace OpenCOR {
namespace KINSOLSolver {

//==============================================================================

PLUGININFO_FUNC KINSOLSolverPluginInfo()
{
    static const Descriptions descriptions = {
                                                 { "en", QString::fromUtf8(R"(a plugin that uses <a href="https://computing.llnl.gov/projects/sundials/kinsol">KINSOL</a> to solve <a href="https://en.wikipedia.org/wiki/Nonlinear_system#Nonlinear_algebraic_equations">non-linear algebraic systems</a>.)") },
                                                 { "fr", QString::fromUtf8(R"(une extension qui utilise <a href="https://computing.llnl.gov/projects/sundials/kinsol">KINSOL</a> pour résoudre des <a href="https://en.wikipedia.org/wiki/Nonlinear_system#Nonlinear_algebraic_equations">systèmes algébriques non-linéaires</a>.)") }
                                             };

    return new PluginInfo(PluginInfo::Category::Solver, true, false,
                          { "SUNDIALS" },
                          descriptions);
}

//==============================================================================
// I18n interface
//==============================================================================

void KINSOLSolverPlugin::retranslateUi()
{
    // We don't handle this interface...
    // Note: even though we don't handle this interface, we still want to
    //       support it since some other aspects of our plugin are
    //       multilingual...
}

//==============================================================================
// Solver interface
//==============================================================================

Solver::Solver * KINSOLSolverPlugin::solverInstance() const
{
    // Create and return an instance of the solver

    return new KinsolSolver();
}

//==============================================================================

QString KINSOLSolverPlugin::id(const QString &pKisaoId) const
{
    // Return the id for the given KiSAO id

    static const QString Kisao0000282 = "KISAO:0000282";
    static const QString Kisao0000486 = "KISAO:0000486";
    static const QString Kisao0000477 = "KISAO:0000477";
    static const QString Kisao0000479 = "KISAO:0000479";
    static const QString Kisao0000480 = "KISAO:0000480";

    if (pKisaoId == Kisao0000282) {
        return solverName();
    }

    if (pKisaoId == Kisao0000486) {
        return MaximumNumberOfIterationsId;
    }

    if (pKisaoId == Kisao0000477) {
        return LinearSolverId;
    }

    if (pKisaoId == Kisao0000479) {
        return UpperHalfBandwidthId;
    }

    if (pKisaoId == Kisao0000480) {
        return LowerHalfBandwidthId;
    }

    return {};
}

//==============================================================================

QString KINSOLSolverPlugin::kisaoId(const QString &pId) const
{
    // Return the KiSAO id for the given id

    if (pId == solverName()) {
        return "KISAO:0000282";
    }

    if (pId == MaximumNumberOfIterationsId) {
        return "KISAO:0000486";
    }

    if (pId == LinearSolverId) {
        return "KISAO:0000477";
    }

    if (pId == UpperHalfBandwidthId) {
        return "KISAO:0000479";
    }

    if (pId == LowerHalfBandwidthId) {
        return "KISAO:0000480";
    }

    return {};
}

//==============================================================================

Solver::Type KINSOLSolverPlugin::solverType() const
{
    // Return the type of the solver

    return Solver::Type::Nla;
}

//==============================================================================

QString KINSOLSolverPlugin::solverName() const
{
    // Return the name of the solver

    return "KINSOL";
}

//==============================================================================

Solver::Properties KINSOLSolverPlugin::solverProperties() const
{
    // Return the properties supported by the solver

    static const Descriptions MaximumNumberOfIterationsDescriptions = {
                                                                          { "en", QString::fromUtf8("Maximum number of iterations") },
                                                                          { "fr", QString::fromUtf8("Nombre maximum d'itérations") }
                                                                      };
    static const Descriptions LinearSolverDescriptions = {
                                                             { "en", QString::fromUtf8("Linear solver") },
                                                             { "fr", QString::fromUtf8("Solveur linéaire") }
                                                         };
    static const Descriptions UpperHalfBandwidthDescriptions = {
                                                                   { "en", QString::fromUtf8("Upper half-bandwidth") },
                                                                   { "fr", QString::fromUtf8("Demi largeur de bande supérieure") }
                                                               };
    static const Descriptions LowerHalfBandwidthDescriptions = {
                                                                   { "en", QString::fromUtf8("Lower half-bandwidth") },
                                                                   { "fr", QString::fromUtf8("Demi largeur de bande inférieure") }
                                                               };
    static const QStringList LinearSolverListValues = {
                                                          DenseLinearSolver,
                                                          BandedLinearSolver,
                                                          GmresLinearSolver,
                                                          BiCgStabLinearSolver,
                                                          TfqmrLinearSolver
                                                      };

    return { Solver::Property(Solver::Property::Type::IntegerGt0, MaximumNumberOfIterationsId, MaximumNumberOfIterationsDescriptions, {}, MaximumNumberOfIterationsDefaultValue, false),
             Solver::Property(Solver::Property::Type::List, LinearSolverId, LinearSolverDescriptions, LinearSolverListValues, LinearSolverDefaultValue, false),
             Solver::Property(Solver::Property::Type::IntegerGe0, UpperHalfBandwidthId, UpperHalfBandwidthDescriptions, {}, UpperHalfBandwidthDefaultValue, false),
             Solver::Property(Solver::Property::Type::IntegerGe0, LowerHalfBandwidthId, LowerHalfBandwidthDescriptions, {}, LowerHalfBandwidthDefaultValue, false) };
}

//==============================================================================

QMap<QString, bool> KINSOLSolverPlugin::solverPropertiesVisibility(const QMap<QString, QString> &pSolverPropertiesValues) const
{
    // Return the visibility of our properties based on the given properties
    // values

    QMap<QString, bool> res;

    QString linearSolver = pSolverPropertiesValues.value(LinearSolverId);

    if (linearSolver == BandedLinearSolver) {
        // Banded linear solver

        res.insert(UpperHalfBandwidthId, true);
        res.insert(LowerHalfBandwidthId, true);
    } else {
        // Dense/GMRES/Bi-CGStab/TFQMR linear solver

        res.insert(UpperHalfBandwidthId, false);
        res.insert(LowerHalfBandwidthId, false);
    }

    return res;
}

//==============================================================================

} // namespace KINSOLSolver
} // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
