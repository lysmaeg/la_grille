# LA GRILLE

## INSTALLATION

### LINUX

Paquets nécéssaires : 
cmake, make - apt install cmake  make

Deux solutions possibles : 

1- Exécuter le fichier install.sh

2- Exécuter les commandes suivantes une par une :

mkdir build \
cd build\
cmake ..\
make


### Windows


1- Exécuter le fichier install.bat

2- Exécuter les commandes une par une :

mkdir build\
cd build\
cmake ..\
make

## UTILISATION

Il existe deux utilisations du fichier exécutable : Résoudre des grilles ou la génération de grilles



Options communes : 

-o : Spécifie le fichier de sortie dans lequelle la grille doit être écrite, si n'est pas fourni la sortie ne sera que dans le terminal\
-t : Spécifie le temps durant lequel le programme s'exécutera (défaut : 60 secondes)\
-h / --help : Affiche ce message\
-v / --verbose : Mode verbose affiche plus de choses que le mode normal

### SOLVEUR DE GRILLES

./theGrid \[-h | o | t | w] FICHIER_ENTRER

-w / --write-all : Écrit toutes les grilles trouvées dans des fichiers différents

### GÉNÉRATEUR DE GRILLES

./theGrid __-g / --generate-numbers__ \[-h | p | s | m ] FICHIER_DE_SORTIE

-s / --size : Spécifie la taille de la grille\
-p / --penality : Spécifie la pénalité de la grille\
-m / --minmax : Spécifie le minimum et le maximum des valeurs de la grille (-m MIN MAX)



