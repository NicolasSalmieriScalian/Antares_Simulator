//
// Created by marechaljas on 11/05/23.
//

#pragma once

#include <functional>
#include <memory>
#include "BindingConstraint.h"

namespace Antares::Data {
class BindingConstraintsList final : public Yuni::NonCopyable<BindingConstraintsList>
{
public:
    using iterator = Data::BindingConstraint::Vector::iterator;
    using const_iterator = Data::BindingConstraint::Vector::const_iterator;

    //! \name Constructor && Destructor
    //@{
    /*!
    ** \brief Default constructor
    */
    BindingConstraintsList() = default;
    /*!
    ** \brief Destructor
    */
    ~BindingConstraintsList() = default;
    //@}

    /*!
    ** \brief Delete all constraints
    */
    void clear();

    //! \name Iterating through all constraints
    //@{
    /*!
    ** \brief Iterate through all constraints
    */
    template<class PredicateT>
    void each(const PredicateT& predicate);
    /*!
    ** \brief Iterate through all constraints (const)
    */
    template<class PredicateT>
    void each(const PredicateT& predicate) const;

    /*!
    ** \brief Iterate through all enabled constraints (const)
    */
    template<class PredicateT>
    void eachEnabled(const PredicateT& predicate) const;

    iterator begin();
    [[nodiscard]] const_iterator begin() const;

    iterator end();
    [[nodiscard]] const_iterator end() const;

    [[nodiscard]] bool empty() const;
    //@}

    /*!
    ** \brief Add a new binding constraint
    */
    std::shared_ptr<BindingConstraint> add(const AnyString& name);

    /*!
    ** Try to find a constraint from its id
    */
    std::shared_ptr<Data::BindingConstraint> find(const AnyString& id);

    /*!
    ** \brief Try to find a constraint from its id (const)
    */
    [[nodiscard]] std::shared_ptr<const Data::BindingConstraint> find(const AnyString& id) const;

    /*!
** \brief Try to find a constraint from its name
*/
    [[nodiscard]] Data::BindingConstraint* findByName(const AnyString& name);

    /*!
    ** \brief Try to find a constraint from its name (const)
    */
    [[nodiscard]] const Data::BindingConstraint* findByName(const AnyString& name) const;

    /*!
    ** \brief Load all binding constraints from a folder
    */
    [[nodiscard]] bool loadFromFolder(Data::Study& s, const Data::StudyLoadOptions& options, const AnyString& folder);

    /*!
    ** \brief Save all binding constraints into a folder
    */
    [[nodiscard]] bool saveToFolder(const AnyString& folder) const;

    /*!
    ** \brief Reverse the sign of the weight for a given interconnection or thermal cluster
    **
    ** This method is used when reverting an interconnection or thermal cluster
    */
    void reverseWeightSign(const Data::AreaLink* lnk);

    //! Get the number of binding constraints
    [[nodiscard]] uint size() const;

    /*!
    ** \brief Remove a binding constraint
    */
    void remove(const Data::BindingConstraint* bc);
    /*!
    ** \brief Remove any binding constraint linked with a given area
    */
    void remove(const Data::Area* area);
    /*!
    ** \brief Remove any binding constraint linked with a given interconnection
    */
    void remove(const Data::AreaLink* area);

    /*!
    ** \brief Remove any binding constraint whose name contains the string in argument
    */
    void removeConstraintsWhoseNameConstains(const AnyString& filter);

    /*!
    ** \brief Rename a binding constraint
    */
    bool rename(Data::BindingConstraint* bc, const AnyString& newname);

    /*!
    ** \brief Convert all weekly constraints into daily ones
    */
    void mutateWeeklyConstraintsIntoDailyOnes();

    /*!
    ** \brief Get the memory usage
    */
    [[nodiscard]] yuint64 memoryUsage() const;

    /*!
    ** \brief Estimate
    */
    void estimateMemoryUsage(Data::StudyMemoryUsage& u) const;

    /*!
    ** \brief Invalidate all matrices of all binding constraints
    */
    [[nodiscard]] bool forceReload(bool reload = false) const;

    /*!
    ** \brief Mark the constraint as modified
    */
    void markAsModified() const;

    [[nodiscard]] const std::map<std::string, Data::BindingConstraintTimeSeriesNumbers, std::less<>>& TimeSeriesNumbers() const {
        return time_series_numbers;
    }
    void resizeAllTimeseriesNumbers(unsigned nb_years);

    std::map<std::string, Data::BindingConstraintTimeSeriesNumbers, std::less<>> time_series_numbers;

    void fixTSNumbersWhenWidthIsOne();

    [[nodiscard]] unsigned int NumberOfTimeseries(const std::string& group_name) const;

    static std::vector<std::shared_ptr<BindingConstraint>> LoadBindingConstraint(EnvForLoading env, uint years);

private:
    bool internalSaveToFolder(Data::BindingConstraint::EnvForSaving& env) const;

    //! All constraints
    Data::BindingConstraint::Vector pList;
};

}
#include "BindingConstraintsList.hxx"