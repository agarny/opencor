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
// Simulation Experiment view information simulation widget
//==============================================================================

#pragma once

//==============================================================================

#include "propertyeditorwidget.h"

//==============================================================================

namespace OpenCOR {

//==============================================================================

namespace SimulationSupport {
    class Simulation;
} // namespace SimulationSupport

//==============================================================================

namespace ZincSimulationExperimentView {

//==============================================================================

class SimulationExperimentViewInformationSimulationWidget : public Core::PropertyEditorWidget
{
    Q_OBJECT

public:
    explicit SimulationExperimentViewInformationSimulationWidget(QWidget *pParent);

    void retranslateUi() override;

    void initialize(SimulationSupport::Simulation *pSimulation);

    Core::Property * startingPointProperty() const;
    Core::Property * endingPointProperty() const;
    Core::Property * pointIntervalProperty() const;

    double startingPoint() const;
    double endingPoint() const;
    double pointInterval() const;

private:
    Core::Property *mStartingPointProperty;
    Core::Property *mEndingPointProperty;
    Core::Property *mPointIntervalProperty;

    SimulationSupport::Simulation *mSimulation = nullptr;

    void updateToolTips();

private slots:
    void updatePoints();
};

//==============================================================================

} // namespace ZincSimulationExperimentView
} // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
