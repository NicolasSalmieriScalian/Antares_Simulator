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
#ifndef __ANTARES_LIBS_STUDY_LINKS_H__
#define __ANTARES_LIBS_STUDY_LINKS_H__

#include <yuni/yuni.h>
#include <yuni/core/noncopyable.h>
#include <yuni/core/string.h>
#include "../fwd.h"
#include <antares/array/matrix.h>
#include <antares/series/series.h>
#include <antares/writer/i_writer.h>
#include <set>

//! The minimal allowed value for hurdle costs when not null
#define LINK_MINIMAL_HURDLE_COSTS_NOT_NULL 0.005

namespace Antares
{
namespace Data
{
struct CompareLinkName;

/*!
** \brief Definition of a link between two areas (Interconnection)
**
** \ingroup area
*/
class AreaLink final : public Yuni::NonCopyable<AreaLink>
{
public:
    //! Vector of links
    using Vector = std::vector<AreaLink*>;
    //! Set of links
    using Set = std::set<AreaLink*, CompareLinkName>;
    //! Map of links
    using Map = std::map<AreaName, AreaLink*>;

    using NamePair = std::pair<Yuni::String, Yuni::String>;

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default Constructor
    */
    AreaLink();
    //! Destructor
    ~AreaLink();
    //@}

    bool loadTimeSeries(const Study& study, const AnyString& folder);

    void storeTimeseriesNumbers(Solver::IResultWriter& writer) const;

    //! \name Area
    //@{
    /*!
    ** \brief Detach the link from the areas
    */
    void detach();
    //@}

    //! \name Data management
    //@{
    /*!
    ** \brief Reverse the link
    **
    ** Be careful, this method does not invert the sign of the weight
    ** for binding constraints.
    */
    void reverse();

    void resetToDefaultValues();

    /*!
    ** \brief Invalidate all matrices
    **
    ** \param reload True to load all missing data
    */
    bool forceReload(bool reload = false) const;

    /*!
    ** \brief Mark the data associated to the link as modified
    */
    void markAsModified() const;
    //@}

    //! \name Memory management
    //@{
    /*!
    ** \brief Get the size (bytes) in memory occupied by a `AreaLink` structure
    */
    uint64_t memoryUsage() const;

    bool isVisibleOnLayer(const size_t& layerID) const;

    Yuni::String getName() const;

    bool isLinkPhysical() const;
    void overrideTransmissionCapacityAccordingToGlobalParameter(GlobalTransmissionCapacities tc);

private:
    bool linkLoadTimeSeries_for_version_below_810(const AnyString& folder);
    bool linkLoadTimeSeries_for_version_820_and_later(const AnyString& folder);
    NamePair getNamePair() const;

public:
    //! \name Graph
    //@{
    //! The orginal Area
    Area* from;
    //! The other area | Hash ID: with->id
    Area* with;
    //@}

    //! \name Data
    //@{
    /*!
    ** \brief Data related to the link
    **
    */
    Matrix<> parameters;
    TimeSeries directCapacities;
    TimeSeries indirectCapacities;

    //! Monte-Carlo
    Matrix<uint32_t> timeseriesNumbers;

    //! Flag for using loop flow
    bool useLoopFlow;

    //! Flag for using the phase shifter
    bool usePST;

    //! Flag for using hurdles cost
    bool useHurdlesCost;

    //! Flag for the transmission capacities (NTC +infinite)
    // previously called copper plate
    LocalTransmissionCapacities transmissionCapacities;
    //@}

    //! Flag for the asset type (AC/DC/Other)
    AssetType assetType;

    //! \name Indexes
    //@{
    /*!
    ** \brief Index of the link in the entire list if Interconnections
    **
    ** \internal This variable must only be used when runtime data are
    ** available
    */
    uint index;
    /*!
    ** \brief Index of the link in the list of interconnection of the main area
    **
    ** \internal This variable must only be used when runtime data are
    ** available
    */
    uint indexForArea;
    //@}

    //! \name Comments
    //@{
    //! Comments
    Yuni::String comments;
    //! Flag for displaying comments
    bool displayComments;
    //@}

    //! \name Output filtering
    //@{
    //! Print results for the area in the simulation synthesis
    uint filterSynthesis;
    //! Print results for the area in the year-by-year mode
    uint filterYearByYear;
    //@}

    //! Colors
    int color[3];
    //! Style
    StyleType style;
    //! link width
    int linkWidth;

    friend struct CompareLinkName;
}; // class AreaLink

struct CompareLinkName final
{
    inline bool operator()(const AreaLink* s1, const AreaLink* s2) const
    {
        return s1->getNamePair() < s2->getNamePair();
    }
};

} // namespace Data
} // namespace Antares

#endif // __ANTARES_LIBS_STUDY_LINKS_H__
