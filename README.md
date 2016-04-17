Knut.sh ![build](https://gitlab.univ-nantes.fr/E132397K/Knut.sh/badges/master/build.svg)
=======

A bash-like shell.

*Pour tous les détails d'implémentation et les possibles améliorations,
se référer au rapport fourni avec le projet.*

# Installation

```bash
make #make dev pour l'affichage des messages de debug
make clean
```

# Utilisation de KnutShell

Architecture du projet :

```
.
├── Makefile
├── README.md       #vous êtes ici
├── bin/            #les binaires compilés par le Makefile
├── obj/            #les fichiers objet
└── src/            #les sources
    ├── libs/
    └── shell/
```

## Compiler

**Dépendences :**

Il peut être nécessaire d'installer les librairies PAM pour su :

```
sudo apt-get install libpam0g-dev
```

**Les commandes make disponibles :**

```
make            #compilation du shell et des commandes
make dev        #idem, avec messages de debug
make clean      #nettoyage du répertoire obj/
make distclean  #nettoyage du répertoire bin/
```

Pour utiliser `su` :

```
sudo chown root ./bin/libs/su
sudo chgrp root ./bin/libs/su
sudo chmod 4711 ./bin/libs/su   #setuid bit
```

## Lancer le shell

Le shell peut être lancé en 3 modes différents afin de mettre en oeuvre
l'utilisation des librairies statiques et dynamiques. Voir l'option `mode`.

```
./bin/knutShell [-m <mode>]
./bin/knutShell connect [<addr>] <port>
```

**Options :**

- `-m <mode>` : Sous quelle forme utiliser nos commandes.
    - `exec` : utilisation des versions exécutables (défaut)
    - `static` : utilisation des libraries statiques
    - `dynamic` : utilisation des libraries dynamiques

- `connect` : connection à un [KnutShell distant](#le-shell-distant)
    - `addr` : (optionel) l'adresse ip
    - `port` : le port

## La syntaxe

Le shell supporte les opérateurs `;`, `|`, `&&`, `||` pour séparer les
commandes.

Un commande peut aussi être mise en background avec `&`.

KnutShell comprend également les parenthèses.

Enfin, les redirections `>`, `>>`, `<`, et `<<` (voir `man bash`,
*Here documents*) sont en places.

**Par exemple, essayez :**

```
#dans un KnutShell
ls -all | wc -l > resultat.txt
echo start; (cp toto.txt / || echo error)
vim
cat < monFichier.txt && echo fin cat
echo start; (sleep 4 && echo Coucou) &
cat <<stop
exit
```

## Le shell distant

Pour se connecter à un KnutShell distant via TCP :

```bash
#serveur
./bin/knutShell #va afficher "Listenning on port XXXX"
```

```bash
#client
./bin/knutShell connect XXXX
./bin/knutShell connect 192.168.0.1 XXXX #l'ip est par défaut 127.0.0.1
```

# Équipe
* Quentin Billaud
* Guillaume Clochard
* Jimmy Doré
* Tom Marrucci
