/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
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
#ifndef __SOLVER_SIMULATION_COMMON_ECONOMY_ADEQUACY_H__
#define __SOLVER_SIMULATION_COMMON_ECONOMY_ADEQUACY_H__

#include <yuni/yuni.h>
#include <antares/study/study.h>
#include "../variable/variable.h"
#include "../optimisation/opt_fonctions.h"
#include "../variable/economy/all.h"
#include <yuni/core/bind.h>
#include "../variable/economy/dispatchable-generation-margin.h" // for OP.MRG

#include "solver.h" // for definition of type yearRandomNumbers

#include <vector>

namespace Antares
{
namespace Solver
{
namespace Simulation
{
// We use the namespace 'economy' here. That means it is mandatory
// that adequacy has the same variable (to get the same type)
typedef Solver::Variable::Economy::VCardBalance AvgExchangeVCardBalance;
typedef Variable::Storage<AvgExchangeVCardBalance>::ResultsType AvgExchangeResults;

/*!
** \brief Compute then random unserved energy cost and the new random hydro virtual cost for all
*areas
**
** This method must be called at the begining of each year even if no calculations
** are performed to be able to make a jump to a given year.
** (hot start)
*/
void PrepareRandomNumbers(Data::Study& study,
                          PROBLEME_HEBDO& problem,
                          yearRandomNumbers& randomForYear);

void BuildThermalPartOfWeeklyProblem(Data::Study& study,
                                     PROBLEME_HEBDO& problem,
                                     const int PasDeTempsDebut,
                                     double** thermalNoises,
                                     unsigned int year);

/*!
** \brief Prepare data from clusters in mustrun mode (eco+adq)
*/
void PrepareDataFromClustersInMustrunMode(Data::Study& study, uint numSpace, uint year);

/*!
** \brief Get if the quadratic optimization should be used according
**  to the input data (eco+adq)
**
** This method check for non-null impedances in links. If a non-null impedance
** is found, we have to launch the quadratic optimisation.
** \return True if the quadratic optimisation should be used, false otherwise
*/
bool ShouldUseQuadraticOptimisation(const Data::Study& study);

/*!
** \brief Perform the quadratic optimization (links) (eco+adq)
*/
void ComputeFlowQuad(Data::Study& study,
                     PROBLEME_HEBDO& problem,
                     const std::vector<AvgExchangeResults*>& balance,
                     unsigned int nbWeeks);

/*!
** \brief Hydro Remix
**
** \param areas : the areas of study
** \param problem The weekly problem, from the solver
** \param hourInYear The hour in the year of the first hour in the current week
*/
void RemixHydroForAllAreas(const Data::AreaList& areas,
                           PROBLEME_HEBDO& problem,
                           Data::SheddingPolicy sheddingPolicy,
                           Data::SimplexOptimization splxOptimization,
                           uint numSpace,
                           uint hourInYear);

/*
** \brief Computing levels from hydro generation, natural and pumping inflows
**
** If hydro remix was not done yet, levels are computed only for areas for which we do not use the
*heuristic
** If hydro remix was done, levels are computed only for areas for which we use the heuristic
*/
void computingHydroLevels(const Data::AreaList& areas,
                          PROBLEME_HEBDO& problem,
                          bool remixWasRun,
                          bool computeAnyway = false);


/*
** \brief Interpolates water values related to reservoir levels for outputs only
**
** \param areas : the areas of study
** \param problem The weekly problem, from the solver
*point of weekly simulation)
** \param hourInYear The hour in the year of the first hour in the current week
**
** For any hour, the computed water values are related to the beginning of the hour, not the end.
*/
void interpolateWaterValue(const Data::AreaList& areas,
                           PROBLEME_HEBDO& problem,
                           const Date::Calendar& calendar,
                           int hourInTheYear);

/*
** \brief Updating the weekly simulation final reservoir level, to be used as a start for the next
*week.
**
** \param areas : the areas of study
** \param problem The weekly problem, from the solver
*/
void updatingWeeklyFinalHydroLevel(const Data::AreaList& areas,
                                   PROBLEME_HEBDO& problem);

/*
** \brief Updating the year final reservoir level, to be used as a start for the year.
**
** \param areas : the areas of study
** \param problem The weekly problem, living over the whole simuation.
*/
void updatingAnnualFinalHydroLevel(const Data::AreaList& areas, PROBLEME_HEBDO& problem);

/*
** \brief Compute the weighted average NTC for a link
**
** \param areas : the areas of study
** \param link The link
** \param Weighted average NTC for the direct direction
** \param Weighted average NTC for the indirect direction
*/
int retrieveAverageNTC(const Data::Study& study,
                       const Matrix<>& capacities,
                       const Matrix<uint32_t>& tsNumbers,
                       std::vector<double>& avg);

void finalizeOptimizationStatistics(PROBLEME_HEBDO& problem,
                                    Antares::Solver::Variable::State& state);

OptimizationOptions createOptimizationOptions(const Data::Study& study);

} // namespace Simulation
} // namespace Solver
} // namespace Antares

#endif // __SOLVER_SIMULATION_COMMON_ECONOMY_ADEQUACY_H__
