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
// Forward Euler solver plugin
//==============================================================================

#include "forwardeulersolver.h"
#include "forwardeulersolverplugin.h"

//==============================================================================

namespace OpenCOR {
namespace ForwardEulerSolver {

//==============================================================================

PLUGININFO_FUNC ForwardEulerSolverPluginInfo()
{
    static const Descriptions descriptions = {
                                                 { "en", QString::fromUtf8(R"(a plugin that implements the <a href="https://en.wikipedia.org/wiki/Euler_method">Forward Euler method</a> to solve <a href="https://en.wikipedia.org/wiki/Ordinary_differential_equation">ODEs</a>.)") },
                                                 { "fr", QString::fromUtf8(R"(une extension qui implémente la <a href="https://en.wikipedia.org/wiki/Euler_method">méthode Forward Euler</a> pour résoudre des <a href="https://en.wikipedia.org/wiki/Ordinary_differential_equation">EDOs</a>.)") }
                                             };

    return new PluginInfo(PluginInfo::Category::Solver, true, false,
                          {},
                          descriptions);
}

//==============================================================================
// I18n interface
//==============================================================================

void ForwardEulerSolverPlugin::retranslateUi()
{
    // We don't handle this interface...
    // Note: even though we don't handle this interface, we still want to
    //       support it since some other aspects of our plugin are
    //       multilingual...
}

//==============================================================================
// Solver interface
//==============================================================================

Solver::Solver * ForwardEulerSolverPlugin::solverInstance() const
{
    // Create and return an instance of the solver

    return new ForwardEulerSolver();
}

//==============================================================================

QString ForwardEulerSolverPlugin::id(const QString &pKisaoId) const
{
    // Return the id for the given KiSAO id

    static const QString Kisao0000030 = "KISAO:0000030";
    static const QString Kisao0000483 = "KISAO:0000483";

    if (pKisaoId == Kisao0000030) {
        return solverName();
    }

    if (pKisaoId == Kisao0000483) {
        return StepId;
    }

    return {};
}

//==============================================================================

QString ForwardEulerSolverPlugin::kisaoId(const QString &pId) const
{
    // Return the KiSAO id for the given id

    if (pId == solverName()) {
        return "KISAO:0000030";
    }

    if (pId == StepId) {
        return "KISAO:0000483";
    }

    return {};
}

//==============================================================================

Solver::Type ForwardEulerSolverPlugin::solverType() const
{
    // Return the type of the solver

    return Solver::Type::Ode;
}

//==============================================================================

QString ForwardEulerSolverPlugin::solverName() const
{
    // Return the name of the solver

    return "Euler (forward)";
}

//==============================================================================

Solver::Properties ForwardEulerSolverPlugin::solverProperties() const
{
    // Return the properties supported by the solver

    static const Descriptions stepDescriptions = {
                                                     { "en", QString::fromUtf8("Step") },
                                                     { "fr", QString::fromUtf8("Pas") }
                                                 };

    return { Solver::Property(Solver::Property::Type::DoubleGt0, StepId, stepDescriptions, {}, StepDefaultValue, true) };
}

//==============================================================================

QMap<QString, bool> ForwardEulerSolverPlugin::solverPropertiesVisibility(const QMap<QString, QString> &pSolverPropertiesValues) const
{
    Q_UNUSED(pSolverPropertiesValues)

    // We don't handle this interface...

    return {};
}

//==============================================================================

} // namespace ForwardEulerSolver
} // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
