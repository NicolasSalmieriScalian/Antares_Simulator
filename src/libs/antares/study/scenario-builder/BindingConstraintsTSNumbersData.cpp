//
// Created by marechaljas on 22/03/23.
//
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

#include <sstream>
#include <iomanip>
#include "scBuilderUtils.h"
#include "TSnumberData.h"

namespace Antares::Data::ScenarioBuilder
{
bool BindingConstraintsTSNumberData::reset(const Study& study)
{
    const uint nbYears = study.parameters.nbYears;
    std::set<std::string> group_names;
    std::for_each(study.bindingConstraints.begin(), study.bindingConstraints.end(), [&group_names](const auto& bc) {
        group_names.insert(bc->group());
    });
    rules_.clear();
    std::for_each(group_names.begin(), group_names.end(), [&](const auto& group_name) {
        MatrixType& ts_numbers = rules_[group_name];
        ts_numbers.resize(1, nbYears);
        ts_numbers.fillColumn(0, 0);
    });
    return true;
}

void BindingConstraintsTSNumberData::saveToINIFile(const Study& study, Yuni::IO::File::Stream& file) const
{
    // Turning values into strings (precision 4)
    std::ostringstream value_into_string;
    value_into_string << std::setprecision(4);

    for (const auto& [group_name, ts_numbers]: rules_) {
        for (unsigned year = 0; year < ts_numbers.height; ++year) {
            auto value = ts_numbers[0][year];
            if (value != 0) {
                file << get_prefix() << group_name << "," << year << "=" << value << "\n";
            }
        }
    }
}

void BindingConstraintsTSNumberData::setData(const std::string& group_name, const uint year, uint value) {
    auto& group_ts_numbers = rules_[group_name];
    group_ts_numbers[0][year] = value;
}

} // namespace Antares