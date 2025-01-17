#include "MinHydroPower.h"

void MinHydroPower::add(int pays)
{
    bool presenceHydro
      = problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable;
    bool TurbEntreBornes = problemeHebdo->CaracteristiquesHydrauliques[pays].TurbinageEntreBornes;
    if (presenceHydro
        && (TurbEntreBornes
            || problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDePompageModulable))
    {
        problemeHebdo->NumeroDeContrainteMinEnergieHydraulique[pays]
          = problemeHebdo->ProblemeAResoudre->NombreDeContraintes;

        const int NombreDePasDeTempsPourUneOptimisation
          = problemeHebdo->NombreDePasDeTempsPourUneOptimisation;

        ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes);
        namer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
        namer.UpdateTimeStep(problemeHebdo->weekInTheYear);
        namer.MinHydroPower(problemeHebdo->ProblemeAResoudre->NombreDeContraintes);
        for (int pdt = 0; pdt < NombreDePasDeTempsPourUneOptimisation; pdt++)
        {
            builder.updateHourWithinWeek(pdt);
            builder.HydProd(pays, 1.0);
        }

        problemeHebdo->NumeroDeContrainteMinEnergieHydraulique[pays]
          = problemeHebdo->ProblemeAResoudre->NombreDeContraintes;
        builder.greaterThan().build();
    }
    else
        problemeHebdo->NumeroDeContrainteMinEnergieHydraulique[pays] = -1;
}
