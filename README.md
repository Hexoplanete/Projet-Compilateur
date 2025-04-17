# Documentation utilisateur du compilateur IFCC - Hexanome 4213 

`ifcc` est un compilateur conçu pour traiter un sous-ensemble du langage C. Le compilateur produit du code assembleur `x86`, mais est capable d'être reciblé sur d'autres architectures dans le futur.
Le code source est disponible sur le [git Hexoplanète](https://github.com/Hexoplanete/Projet-Compilateur).

La liste des fonctionnalités implémentés est disponible dans le fichier [`Features.md`](Features.md).

La documentation développeur est disponible dans le fichier [`DevDoc.md`](DevDoc.md).

## Installation
Pour générer l'executable `ifcc` utilisez le target make par default.
```bash
make
```
L'executable du compilateur est généré à la racine.

## Utilisation
Le compilateur permet de générer l'assembleur d'un fichier source `.c` dans un fichier `.s`. Utilisez `gcc` pour générer l'executable final.
```bash
./ifcc file.c -o file.s
gcc file.s -o file
```

## Suite de test
Une suite de près de 200 tests est disponible dans le dossier [`tests/`](tests/).
Le target `test` permet d'executer les tests
```bash
make test
```
Les résultats des compilations et un résumé d’exécution sont sauvegardés dans le dossier `tests/ifcc-test-ifcc/` et dans un fichier `<date>-ifcc-test-ifcc.csv` respectivement.

Utilisez le ficher [`ifcc-test.py`](tests/ifcc-test.py) directement si vous souhaitez exécutez des tests precis.
```bash
python3 tests/ifcc-test-py --help
```

## Affichage de l'ast
Le target make `gui` permet d'afficher l'AST généré par la grammaire [`ifcc.g4`](compiler/ifcc.g4).
```bash
make gui FILE=<file>
```

## Nettoyage des fichiers générés
Utilisez les targets `clean` et `cleantest` pour supprimer les fichiers créés par la compilation et la suite de test.
```bash
make clean
make cleantest
```

## Composition de l'équipe
| Nom             | Rôle principal                         |
| --------------- | -------------------------------------- |
| Justine STEPHAN | Scrum Master                           |
| Marine QUEMENER | Responsable organisationnel et rapport |
| Corentin JEANNE | Développement des fonctionnalités      |
| Juliette PIERRE | Développement des fonctionnalités      |
| Joris FELZINES  | Ajout des tests                        |
| Harold MARTIN   | Ajout des tests                        |