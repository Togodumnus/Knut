#include "chgrp.h"
#include "../../LIB.h"

/**
 * Init
 *
 * S'enregistre dans le shell dans le cas d'un chargement de la librairie
 * dynamique
 */
void Init(EnregisterCommande enregisterCommande) {
    enregisterCommande("chgrp", chgrpLib);
}
