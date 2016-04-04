Knut.sh ![build](https://gitlab.univ-nantes.fr/E132397K/Knut.sh/badges/master/build.svg)
=======

![img](http://g-ecx.images-amazon.com/images/G/01/dvd/afaddis/image/knut_1.jpg)

A bash-like shell.

*Pour tous les détails d'implémentation et les possibles améliorations,
se référer au rapport fourni avec le projet.*

# Installation

```bash
make #make dev pour l'affichage de tous les messages de débug :
make clean
```

# Utilisation de KnutShell

## Lancer le shell

Le shell peut être lancé en 3 modes différents afin de mettre en oeuvre
l'utilisation des librairies statiques et dynamiques. Voir l'option `mode`.

```
./bin/knutShell [-m <mode>]
./bin/knutShell connect [<addr>] <port>
```

**Options :**

- `-m <mode>` : Sous quelle forme utiliser nos commandes.
    - `exec` : utilsation des versions exécutables
    - `static` : utilsation des libraries statiques
    - `dynamic` : utilsation des libraries dynamiques

- `connect` : connection à un (KnutShell distant)(#le-shell-distant)
    - `addr` : (optionel) l'adresse ip
    - `port` : le port

## La syntaxe

Le shell supporte les opérateurs `;`, `|`, `&&`, `||` pour séparer les
commandes.

Un commande peut aussi être mise en background avec `&`.

Enfin, les redirections `>`, `>>`, `<`, et `<<` (voir `man bash`,
Here documents) sont en places.

KnutShell comprend également les parenthèses.

Par exemple, essayez :

```bash
#dans un KnutShell
ls -all | wc -l > resultat.txt
echo start; (cp toto.txt / || echo error)
vim
cat < monFichier.txt && echo fin cat
cat <<stop
echo start; (sleep 4 && echo Coucou) &
```

## Le shell distant

Pour se connecter à un KnutShell distant via TCP :

```bash
#serveur
./bin/knutShell #va afficher "Listenning on port XXXX"
```

```bash
#client
./bin/knutShell connect XXX
./bin/knutShell connect 192.168.0.1 XXXX #l'ip est par défaut 127.0.0.1
```

# Commandes disponibles

//TODO

