# Marchons ensemble

Génère la publication mensuelle de St-Adrien "Marchons ensemble" sous format
PDF.

Statut: pas terminé

## Dépendances

* pango
* cairo
* pkg-config
* make

Sous ubuntu, c'est:

    apt install libcairo2-dev libpango1.0-dev libpangocairo-1.0-0

## Compiler

Pour compiler, il suffit de rouler `make`. Un exécutable `marchons` sera
généré.

## Utilisation

Rouler `./marchons source.txt dest.pdf` pour générer un PDF dans le fichier
indiqué. Exemple:

    ./marchons "mois/Décembre 2019" dest.pdf

## Format du fichier source

Le format du fichier source est en développement. Pour l'instant, c'est un
titre par ligne.
