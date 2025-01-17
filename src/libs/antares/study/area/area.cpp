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

#include <yuni/yuni.h>
#include <cassert>
#include "../study.h"
#include "area.h"
#include "ui.h"
#include "scratchpad.h"
#include "antares/study/parts/load/prepro.h"

using namespace Yuni;

namespace Antares::Data
{
void Area::internalInitialize()
{
    // Make sure we have
    if (JIT::usedFromGUI)
        ui = new AreaUI();
}

Area::Area() :
    reserves(fhrMax, HOURS_PER_YEAR),
    miscGen(fhhMax, HOURS_PER_YEAR)
{
    internalInitialize();
}

Area::Area(const AnyString& name) :
    reserves(fhrMax, HOURS_PER_YEAR),
    miscGen(fhhMax, HOURS_PER_YEAR)
{
    internalInitialize();
    this->name = name;
    Antares::TransformNameIntoID(this->name, this->id);
}

Area::Area(const AnyString& name, const AnyString& id) :

    reserves(fhrMax, HOURS_PER_YEAR),
    miscGen(fhhMax, HOURS_PER_YEAR)
{
    internalInitialize();
    this->name = name;
    AreaName givenID = id;
    Antares::TransformNameIntoID(givenID, this->id);
}

Area::~Area()
{
    logs.debug() << "  :: destroying area " << name;

    // Delete all links
    clearAllLinks();

    reserves.clear();
    miscGen.clear();

    delete ui;
    ui = nullptr;
}

void Area::clearAllLinks()
{
    if (not links.empty())
    {
        // Delete all links
        {
            auto end = links.end();
            for (auto i = links.begin(); i != end; ++i)
                delete i->second;
        }
        // Empty the container
        links.clear();
    }
}

void Area::detachAllLinks()
{
    while (not links.empty())
        AreaLinkRemove((links.begin())->second);
}

AreaLink* Area::findExistingLinkWith(Area& with)
{
    if (&with == this)
        return nullptr;

    if (not links.empty())
    {
        const AreaLink::Map::iterator end = links.end();
        for (AreaLink::Map::iterator i = links.begin(); i != end; ++i)
        {
            if (i->second->from == &with or i->second->with == &with)
                return i->second;
        }
    }
    if (!with.links.empty())
    {
        for (auto i = with.links.begin(); i != with.links.end(); ++i)
        {
            if (i->second->from == this or i->second->with == this)
                return i->second;
        }
    }
    return nullptr;
}

const AreaLink* Area::findExistingLinkWith(const Area& with) const
{
    if (&with != this)
    {
        if (not links.empty())
        {
            const auto end = links.end();
            for (auto i = links.begin(); i != end; ++i)
            {
                if (i->second->from == &with or i->second->with == &with)
                    return i->second;
            }
        }
        if (!with.links.empty())
        {
            const auto end = with.links.end();
            for (auto i = with.links.begin(); i != end; ++i)
            {
                if (i->second->from == this or i->second->with == this)
                    return i->second;
            }
        }
    }
    return nullptr;
}

uint64_t Area::memoryUsage() const
{
    uint64_t ret = 0;

    // Misc gen. (previously called Fatal hors hydro)
    ret += miscGen.valuesMemoryUsage();
    // Reserves
    ret += reserves.valuesMemoryUsage();

    ret += sizeof(Area);
    // Load
    ret += load.memoryUsage();
    // Solar
    ret += solar.memoryUsage();
    // Wind
    ret += wind.memoryUsage();

    // Hydro
    ret += PreproHydroMemoryUsage(hydro.prepro);
    if (hydro.series)
        ret += hydro.series->memoryUsage();

    // Thermal
    ret += thermal.list.memoryUsage();

    // Renewable
    ret += renewable.list.memoryUsage();

    // UI
    if (ui)
        ret += ui->memoryUsage();

    // links
    auto end = links.end();
    for (auto i = links.begin(); i != end; ++i)
        ret += (i->second)->memoryUsage();

    return ret;
}

void Area::createMissingData()
{
    createMissingTimeSeries();
    createMissingPrepros();
}

void Area::createMissingTimeSeries()
{
    if (!hydro.series)
        hydro.series = new DataSeriesHydro();
}
void Area::createMissingPrepros()
{
    if (!load.prepro)
        load.prepro = new Data::Load::Prepro();
    if (!solar.prepro)
        solar.prepro = new Data::Solar::Prepro();
    if (!wind.prepro)
        wind.prepro = new Data::Wind::Prepro();
    if (!hydro.prepro)
        hydro.prepro = new PreproHydro();
    thermal.list.ensureDataPrepro();
}

void Area::resetToDefaultValues()
{
    // Nodal optimization
    nodalOptimization = anoAll;

    // Spread
    spreadUnsuppliedEnergyCost = 0.;
    spreadSpilledEnergyCost = 0.;

    // Filtering
    filterSynthesis = (uint)filterAll;
    filterYearByYear = (uint)filterAll;

    // Load
    load.resetToDefault();
    // Solar
    solar.resetToDefault();
    // Wind
    wind.resetToDefault();
    // Hydro
    hydro.reset();
    hydro.allocation.fromArea(id, 1.);
    // Thermal
    thermal.reset();
    // Renewable
    renewable.reset();
    // Fatal hors hydro
    miscGen.reset(fhhMax, HOURS_PER_YEAR, true);
    // reserves
    reserves.reset(fhrMax, HOURS_PER_YEAR, true);

    // invalidate the whole area
    invalidateJIT = true;
}

void Area::resizeAllTimeseriesNumbers(uint n)
{
    assert(n < 200000); // arbitrary number

    // asserts
    assert(hydro.series and "series must not be nullptr !");

    if (!n)
    {
        load.series.timeseriesNumbers.clear();
        solar.series.timeseriesNumbers.clear();
        wind.series.timeseriesNumbers.clear();
        hydro.series->timeseriesNumbers.clear();
        for (auto& namedLink : links)
        {
            AreaLink* link = namedLink.second;
            link->timeseriesNumbers.clear();
        }
    }
    else
    {
        load.series.timeseriesNumbers.resize(1, n);
        solar.series.timeseriesNumbers.resize(1, n);
        wind.series.timeseriesNumbers.resize(1, n);
        hydro.series->timeseriesNumbers.resize(1, n);
        for (auto& namedLink : links)
        {
            AreaLink* link = namedLink.second;
            link->timeseriesNumbers.resize(1, n);
        }
    }
    thermal.resizeAllTimeseriesNumbers(n);
    renewable.resizeAllTimeseriesNumbers(n);
}

bool Area::thermalClustersMinStablePowerValidity(std::vector<YString>& output) const
{
    bool noErrorMinStabPow = true;
    for (uint l = 0; l != thermal.clusterCount(); ++l)
    {
        auto& cluster = thermal.clusters[l];
        logs.debug() << "cluster : " << cluster->name();
        if ((not cluster->checkMinStablePower())
            || (cluster->minStablePower
                > cluster->nominalCapacity * (1 - cluster->spinning / 100.)))
        {
            output.push_back(cluster->name());
            noErrorMinStabPow = false;
        }
    }
    return noErrorMinStabPow;
}

bool Area::forceReload(bool reload) const
{
    // To not break the entire constness design of the library
    // this method should remain const event if the operations
    // performed are obviously not const
    auto& self = *(const_cast<Area*>(this));

    bool ret = true;
    invalidateJIT = false;

    // Misc Gen
    ret = self.miscGen.forceReload(reload) and ret;
    // Reserves
    ret = self.reserves.forceReload(reload) and ret;

    // Load
    ret = self.load.forceReload(reload) and ret;
    // Solar
    ret = self.solar.forceReload(reload) and ret;
    // Hydro
    ret = self.hydro.forceReload(reload) and ret;
    // Wind
    ret = self.wind.forceReload(reload) and ret;
    // Thermal
    ret = self.thermal.forceReload(reload) and ret;
    // Renewable
    ret = self.renewable.forceReload(reload) and ret;
    if (not links.empty())
    {
        auto end = self.links.end();
        for (auto i = self.links.begin(); i != end; ++i)
            ret = (i->second)->forceReload(reload) and ret;
    }

    if (ui)
        self.ui->markAsModified();

    return ret;
}

void Area::markAsModified() const
{
    // Misc Gen
    miscGen.markAsModified();
    // Reserves
    reserves.markAsModified();

    // Load
    load.markAsModified();
    // Solar
    solar.markAsModified();
    // Hydro
    hydro.markAsModified();
    // Wind
    wind.markAsModified();
    // Thermal
    thermal.markAsModified();
    // Renewable
    renewable.markAsModified();

    if (not links.empty())
    {
        auto end = links.end();
        for (auto i = links.begin(); i != end; ++i)
            (i->second)->markAsModified();
    }
    if (ui)
        ui->markAsModified();
}

void Area::detachLinkFromID(const AreaName& id)
{
    auto i = links.find(id);
    if (i != links.end())
        links.erase(i);
}

void Area::detachLink(const AreaLink* lnk)
{
    assert(lnk);
    assert(lnk->from);
    assert(lnk->with);

    lnk->from->detachLinkFromID(lnk->with->id);
}

AreaLink* Area::findLinkByID(const AreaName& id)
{
    auto i = links.find(id);
    return (i != links.end()) ? i->second : nullptr;
}

const AreaLink* Area::findLinkByID(const AreaName& id) const
{
    auto i = links.find(id);
    return (i != links.end()) ? i->second : nullptr;
}

void Area::buildLinksIndexes()
{
    uint areaIndx = 0;

    auto end = links.end();
    for (auto i = links.begin(); i != end; ++i)
    {
        auto* link = i->second;
        link->indexForArea = areaIndx;
        ++areaIndx;
    }
}

} // namespace Antares
