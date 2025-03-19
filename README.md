
#-#=#-#=#-#=#-#=#-#=#-#=#-# FONCTIONNALITES IMPLEMENTEES #-#=#-#=#-#=#-#=#-#=#-#=#-#

Type de données de base int
Variables
Constantes entières mais pas caractères
Opérations arithmétiques de base +, -, *
Implémentation du modulo
Implémentation de la division
Affectation multiple



#-#=#-#=#-#=#-#=#-#=#-#=#-# DESCRIPTION SUCCINTE DU PROGRAMME #-#=#-#=#-#=#-#=#-#=#-#=#-#

ETAPE 1 :
L’AST est généré par l’analyseur d’ANTLR

ETAPE 2 :
L’AST est visité grâce au visiteur SymbolMapVisitor qui remplit la table des symboles avec les différentes variables, qu’elles soient temporaires ou non.

ETAPE 3 :
La récupération des symboles intermédiaires est faite grâce au visiteur CodeGenVisitor, qui visite l’AST. En fonction des noeuds rencontrés et de la table des symboles, le visiteur génère le code assembleur associé. 



#-#=#-#=#-#=#-#=#-#=#-#=#-# STRUCTURE DES TESTS #-#=#-#=#-#=#-#=#-#=#-#=#-#

Les tests ont été regroupés par thèmes dans le dossier testfiles conformément au sujet du projet. On a donc :
0_invalid_c_prog
1-8_expressions
9_random_decl_var
10_affectations
11_fonctions_std_IO
12_fonctions_param
14_structure_blocs
17_return_anywhere
18_verif_var_decl
19_verif_var_decl_once
20_verif_var_decl_used

non_regression : chaque fois qu'on ajoute une fonctionnalité, cela ne doit pas casser ce qui a déjà été fait



#-#=#-#=#-#=#-#=#-#=#-#=#-# STRUCTURE DU CODE #-#=#-#=#-#=#-#=#-#=#-#=#-#








#-#=#-#=#-#=#-#=#-#=#-#=#-# DESCRIPTION DE LA GESTION DE PROJET #-#=#-#=#-#=#-#=#-#=#-#=#-#

Le projet est réalisé selon la méthode AGILE et le TDD (Test Driven Development) c’est-à-dire qu'une première version fonctionnelle du projet est rendue à la fin de chaque sprint avec les tests correspondants. On effectuera deux sprints de durée égale (deux semaines de projet).

La répartition des rôles a été faite selon : 
L'équipe de développement du programme : Corentin JEANNE, Juliette PIERRE
L'équipe de réalisation de fichiers de test : Joris FELZINES, Harold MARTIN
Responsable organisationnel : Marine QUEMENER
Scrum master : Justine STEPHAN


L'outil de développement est Visual Studio Code et le code est stocké sur GitHub.

