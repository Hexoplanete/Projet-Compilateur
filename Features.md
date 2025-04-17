# Fonctionnalités implémentées

| ✅   | Fonctionnalité                                                                                                       | Type | Priorité |
| --- | -------------------------------------------------------------------------------------------------------------------- | ---- | -------- |
| ✅   | Un seul fichier source sans pré-processing                                                                           | `L`    | DI       |
| ✅   | Les commentaires sont ignorés                                                                                        | `L`    | DI       |
| ✅   | Type de données de base int (un type 32 bits)                                                                        | `L`    | **O**    |
| ✅   | Expressions Variables                                                                                                | `L`    | **O**    |
| ✅   | Constantes entières et caractère (avec simple quote)                                                                 | `L`    | **O**    |
| ✅   | Opérations arithmétiques de base : +,-, *                                                                            | `L`    | **O**    |
| ✅   | Division et modulo                                                                                                   | `L`    | **O**    |
| ✅   | Opérations logiques bit-à-bit : \| , &,ˆ                                                                             | `L`    | **O**    |
| ✅   | Opérations de comparaison : ==, !=, <, >                                                                             | `L`    | **O**    |
| ✅   | Opérations unaires : ! et -                                                                                          | `L`    | **O**    |
| ✅   | Déclaration de variables n’importe où                                                                                | `L`    | **O**    |
| ✅   | Affectation (qui, en C, retourne aussi une valeur)                                                                   | `L`    | **O**    |
| ✅   | Utilisation des fonctions standard putchar et getchar pour les entrées-sorties                                       | `L`    | **O**    |
| ✅   | Définition de fonctions avec paramètres, et type de retour int ou void                                               | `L`    | **O**    |
| ✅   | Vérification de la cohérence des appels de fonctions et leurs paramètres                                             | `C`    | **O**    |
| ✅   | Structure de blocs grâce à { et }                                                                                    | `L`    | **O**    |
| ✅   | Support des portées de variables et du shadowing                                                                     | `L`    | **O**    |
| ✅   | Les structures de contrôle if, else, while                                                                           | `L`    | **O**    |
| ✅   | Support du return expression n’importe où                                                                            | `L`    | **O**    |
| ✅   | Vérification qu’une variable utilisée a été déclarée                                                                 | `C`    | **O**    |
| ✅   | Vérification qu’une variable n’est pas déclarée plusieurs fois                                                       | `C`    | **O**    |
| ✅   | Vérification qu’une variable déclarée est utilisée                                                                   | `C`    | **O**    |
| ⏺️   | Reciblage vers plusieurs architectures : x86, MSP430, ARM                                                            | `C`    | F        |
| ⬛   | Support des double avec toutes les conversions implicites                                                            | `L`    | F        |
| ⬛   | Propagation de constantes simple                                                                                     | `C`    | F        |
| ⬛   | Propagation de variables constantes (avec analyse du data-flow)                                                      | `C`    | F        |
| ⬛   | Tableaux (à une dimension)                                                                                           | `L`    | F        |
| ⬛   | Pointeurs                                                                                                            | `L`    | F        |
| ⬛   | break et continue                                                                                                    | `L`    | F        |
| ⬛   | Les chaînes de caractères représentées par des tableaux de char                                                      | `L`    | F        |
| ✅   | Possibilité d’initialiser une variable lors de sa déclaration                                                        | `L`    | F        |
| ⬛   | switch...case                                                                                                        | `L`    | F        |
| ✅   | Les opérateurs logiques paresseux \|\| , &&                                                                          | `L`    | F        |
| ✅   | Opérateurs d’affectation +=, -= etc., d’incrémentation ++ et décrémentation --                                       | `L`    | F        |
| ⬛   | Les variables globales                                                                                               | `L`    | _NP_     |
| ⬛   | Les autres types de inttypes.h, les float                                                                            | `L`    | _NP_     |
| ⏺️   | Le support dans les moindres détails de tous les autres opérateurs arithmétiques et logiques : <=, >=, << et >> etc. | `L`    | _NP_     |
| ⬛   | Les autres structures de contrôle : for, do...while                                                                  | `L`    | _NP_     |
| ⬛   | La possibilité de séparer dans des fichiers distincts les déclarations et les définitions                            | `L`    | _D_      |
| ⬛   | Le support du préprocesseur (#define, #include, #if, etc.)                                                           | `L`    | _D_      |
| ⬛   | Les structures et unions                                                                                             | `L`    | _D_      |
| ⏺️   | Support en largeur du type de données char (entier 8 bits)                                                           | `L`    | _D_      |

Le type `L` correspond à des éléments de support du langage, tandis que le type `C` correspond à une fonctionnalité du compilateur.

Les priorités sont les suivantes :
- DI : Déjà Implémenté
- **O** : Obligatoire
- F : Facultatif
- *NP* : Non Prioritaire
- *D* : Déconseillé
