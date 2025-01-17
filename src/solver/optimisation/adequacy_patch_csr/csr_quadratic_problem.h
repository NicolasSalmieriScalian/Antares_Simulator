/*
** Copyright 2007-2023 RTE
** Authors: RTE-international / Redstork / Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/

#pragma once

class HourlyCSRProblem;

namespace Antares::Solver::Optimization
{

class CsrQuadraticProblem
{
public:
    CsrQuadraticProblem(const PROBLEME_HEBDO* p, PROBLEME_ANTARES_A_RESOUDRE& pa, HourlyCSRProblem& hourly) :
      problemeHebdo_(p), problemeAResoudre_(pa), hourlyCsrProblem_(hourly)
    {
    }

    void buildConstraintMatrix();

private:
    const PROBLEME_HEBDO* problemeHebdo_;
    PROBLEME_ANTARES_A_RESOUDRE& problemeAResoudre_;
    HourlyCSRProblem& hourlyCsrProblem_;

    void setConstraintsOnFlows(std::vector<double>& Pi, std::vector<int>& Colonne);
    void setNodeBalanceConstraints(std::vector<double>& Pi, std::vector<int>& Colonne);
    void setBindingConstraints(std::vector<double>& Pi, std::vector<int>& Colonne);
};

} //namespace Antares::Solver::Optimization
