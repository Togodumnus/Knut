Knut.sh ![build](https://gitlab.univ-nantes.fr/E132397K/Knut.sh/badges/master/build.svg)
=======

![img](http://g-ecx.images-amazon.com/images/G/01/dvd/afaddis/image/knut_1.jpg)

A bash-like tiny shell.

#Install

```bash
make
make clean
```

#Using KnutShell

```
./bin/knutShell connect [<addr>] <port>
./bin/knutShell [-m <mode>]
```

**Options :**

- `connect` : connection à un KnutShell distant
    - `addr` : (optionel) l'adresse ip
    - `port` : le port

- `-m <mode>` : Sous quelle forme utiliser nos commandes.
    - `exec` : utilsation des versions exécutables
    - `static` : utilsation des libraries statiques
    - `dynamic` : utilsation des libraries dynamiques
