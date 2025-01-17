#include "PMinDispatchableGeneration.h"

void PMinDispatchableGeneration::add(int pays,
                                     int cluster,
                                     int clusterIndex,
                                     int pdt,
                                     bool Simulation)
{
    if (!Simulation)
    {
        const PALIERS_THERMIQUES& PaliersThermiquesDuPays
          = problemeHebdo->PaliersThermiquesDuPays[pays];
        double pminDUnGroupeDuPalierThermique
          = PaliersThermiquesDuPays.pminDUnGroupeDuPalierThermique[clusterIndex];

        builder.updateHourWithinWeek(pdt)
          .DispatchableProduction(cluster, 1.0)
          .NumberOfDispatchableUnits(cluster, -pminDUnGroupeDuPalierThermique)
          .greaterThan();
        /*consider Adding naming constraint inside the builder*/
        if (builder.NumberOfVariables() > 0)
        {
            ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes);
            namer.UpdateArea(problemeHebdo->NomsDesPays[pays]);

            namer.UpdateTimeStep(problemeHebdo->weekInTheYear * 168 + pdt);
            namer.PMinDispatchableGeneration(
              problemeHebdo->ProblemeAResoudre->NombreDeContraintes,
              PaliersThermiquesDuPays.NomsDesPaliersThermiques[clusterIndex]);
        }
          builder.build();
    }
    else
    {
          problemeHebdo->NbTermesContraintesPourLesCoutsDeDemarrage += 2;
          problemeHebdo->ProblemeAResoudre->NombreDeContraintes++;
    }
}
