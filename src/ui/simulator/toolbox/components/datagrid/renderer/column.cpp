/*
** Copyright 2007-2018 RTE
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

#include "column.h"
#include "cell.h"

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{
// -------------------
// Base column class
// -------------------
Column::Column(Antares::Data::TimeSeries ts, wxString c) : tsKind_(ts), caption_(c)
{
}

Cell* Column::getLine(int y) const
{
    return cells_[y];
}

wxString Column::getCaption() const
{
    return caption_;
}

int Column::getNumberOfLines() const
{
    return cells_.size();
}

Column::~Column()
{
    for (auto& c : cells_)
        delete c;
}
// -------------------- -
// Classic column
// ---------------------
classicColumn::classicColumn(TimeSeries ts, wxString c) : Column(ts, c)
{
    cells_ = {new blankCell(),
              new readyMadeTSstatus(tsKind_),
              new blankCell(),
              new generatedTSstatus(tsKind_),
              new NumberTsCell(tsKind_),
              new RefreshTsCell(tsKind_),
              new RefreshSpanCell(tsKind_),
              new SeasonalCorrelationCell(tsKind_),
              new storeToInputCell(tsKind_),
              new storeToOutputCell(tsKind_),
              new blankCell(),
              new intraModalCell(tsKind_),
              new interModalCell(tsKind_)};
}
// -------------------------------
// Column renewable clusters
// -------------------------------
ColumnRenewableClusters::ColumnRenewableClusters() : Column(timeSeriesRenewable, "   Renewable   ")
{
    cells_ = {new blankCell(),
              new inactiveRenewableClusterCell(wxT("On")),
              new inactiveRenewableClusterCell(wxT("-")),
              new inactiveRenewableClusterCell(wxT("-")),
              new inactiveRenewableClusterCell(wxT("-")),
              new inactiveRenewableClusterCell(wxT("-")),
              new inactiveRenewableClusterCell(wxT("-")),
              new inactiveRenewableClusterCell(wxT("-")),
              new inactiveRenewableClusterCell(wxT("-")),
              new inactiveRenewableClusterCell(wxT("-")),
              new blankCell(),
              new intraModalCell(tsKind_),
              new interModalCell(tsKind_)};
}
} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares