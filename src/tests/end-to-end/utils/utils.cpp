#define WIN32_LEAN_AND_MEAN
#include "utils.h"
#include "simulation/simulation.h"

using namespace Antares::Data;

void prepareStudy(Study::Ptr pStudy, int nbYears)
{
    //Define study parameters
    pStudy->parameters.reset();
    pStudy->parameters.resetPlaylist(nbYears);

    //Prepare parameters for simulation
    Data::StudyLoadOptions options;
    pStudy->parameters.prepareForSimulation(options);

    // Logical cores
    // -------------------------
    // Getting the number of logical cores to use before loading and creating the areas :
    // Areas need this number to be up-to-date at construction.
    pStudy->getNumberOfCores(false, 0);

    // Define as current study
    Data::Study::Current::Set(pStudy);
}

std::shared_ptr<BindingConstraint> addBindingConstraints(Study::Ptr study, std::string name, std::string group) {
    auto bc = study->bindingConstraints.add(name);
    bc->group(group);
    bc->type();
    auto ts = study->bindingConstraints.timeSeriesNumbers[group]; //Create the tsNumbers
    return bc;
}

Antares::Data::ScenarioBuilder::Rules::Ptr createScenarioRules(Study::Ptr pStudy)
{
    ScenarioBuilder::Rules::Ptr pRules;

    pStudy->scenarioRulesCreate();
    ScenarioBuilder::Sets* p_sets = pStudy->scenarioRules;
    if (p_sets && !p_sets->empty())
    {
        pRules = p_sets->createNew("Custom");

        pStudy->parameters.useCustomScenario  = true;
        pStudy->parameters.activeRulesScenario = "Custom";
    }

    return pRules;
}

float defineYearsWeight(Study::Ptr pStudy, const std::vector<float>& yearsWeight)
{
    pStudy->parameters.userPlaylist = true;

    for (uint i = 0; i < yearsWeight.size(); i++)
    {
        pStudy->parameters.setYearWeight(i, yearsWeight[i]);
    }

    return pStudy->parameters.getYearsWeightSum();
}

void cleanSimulation(Study::Ptr pStudy, Solver::Simulation::ISimulation< Solver::Simulation::Economy >* simulation)
{
    // simulation
    SIM_DesallocationTableaux();

    delete simulation;

    // release all reference to the current study held by this class
    pStudy->clear();

    pStudy = nullptr;
    // removed any global reference
    Data::Study::Current::Set(nullptr);
}

void NoOPResultWriter::addEntryFromBuffer(const std::string&, Clob&)
{

}
void NoOPResultWriter::addEntryFromBuffer(const std::string&, std::string&)
{

}
void NoOPResultWriter::addEntryFromFile(const std::string&, const std::string&)
{

}
bool NoOPResultWriter::needsTheJobQueue() const
{
    return false;
}
void NoOPResultWriter::finalize(bool)
{

}