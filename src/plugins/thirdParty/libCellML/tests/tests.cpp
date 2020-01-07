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
// libCellML tests
//==============================================================================

#include "tests.h"

//==============================================================================

#include <QtTest/QtTest>

//==============================================================================

#include "libcellmlbegin.h"
    #include "libcellml/model.h"
    #include "libcellml/version.h"
#include "libcellmlend.h"

//==============================================================================

void Tests::basicTests()
{
    // Some very basic tests to make sure that we have access to libCellML

    // Check the version of libSBML

    QCOMPARE(libcellml::version(), 0x000200);
    QCOMPARE(libcellml::versionString(), std::string("0.2.0"));

    // Create a CellML document with a model inside it, then set the name of the
    // model and check that it has been properly set

    auto cellmlModel = libcellml::Model::create();

    static const std::string ModelName = "myModel";

    cellmlModel->setName(ModelName);

    QCOMPARE(cellmlModel->name(), ModelName);
}

//==============================================================================

QTEST_GUILESS_MAIN(Tests)

//==============================================================================
// End of file
//==============================================================================
