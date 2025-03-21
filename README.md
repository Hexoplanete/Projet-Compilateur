
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

Dans le dossier compiler, on fait make.
cd compiler && make

Pour supprimer tous les fichiers générés
cd compiler && make clean

Le programme fonctionne de la manière suivante : 
1. L'AST est analysé gramaticallement par l'SymbolMapVisitor. Cela va permettre d'établir la table de symboles qui va associer un nom de variable à une adresse mémoire.
2. L'AST est de nouveau analysé gramaticallement, mais cette fois ci par CodeGenVisitor qui, avec l'aide de la table de symboles, va construire le programme assembleur associé à l'arbre.

Dans la partie 1 : ANALYSE ET CONSTRUCTION DE LA TABLE DE SYMBOLES
- Le programme est visité, les enfants de cette déclaration sont également visités, et ça récursivement pour les enfants.
- La taille de la variable est déduite en fonction des types dans le cas des déclarations (+ définitions).
- L'adresse de stockage de la variable est calculée en fonction de la taille déterminée plus tôt.
- Le nom des variables est extraite du symbole VARNAME, dans le cas où il existe déjà dans la table de symbole, un erreur est renvoyée.
- Des variables temporaires @tmpX et @tmpY sont crées dans la table de symbole pour être en mesure de gérer des affectations ainsi que la division et le modulo.


Dans la partie 2 : CONSTRUCTION DU CODE ASSEMBLEUR
- Le programme est visité, dans cette même étape, le prologue est construit, les enfants du programme sont également visités de sorte à ce que le code assembleur puisse être construit, l'épilogue est construit.
- Pour les affectations de type TYPE IDENTIFIER = EXPRESSION, la partie EXPRESSION est visitée et sa valeur est copiée dans eax, la table de symboles est consultée pour récupérer l'adresse associée à la variable, la valeur d'eax est copiée à l'adresse de la variable (conformément à la table de symboles)
- Les valeurs CONST sont déchiffrées en valeurs entières
- Les expressions sont visitées pour déterminer en fonction de l'opération unaire si le signe d'eax doit changer. Dans le cas d'une soustraction, le signe d'eax est modifié ; Dansle cas d'une addition, rien n'est modifié.
- L'addition et la soustraction sont gérées de la manière suivante :
    A. La valeur de l'expression à gauche de l'expression arithmétique est copiée dans eax
    B. une nouvelle variable temporaire @tmpX est créée et est ajoutée dans la table de symboles
    C. l'expression de gauche (dans eax) est copiée dans la variable temporaire pour libérer eax
    D. La valeur de l'expression à droite de l'expression arithmétique est copiée dans eax
    E. En fonction de l'opérateur arithmétique, un left + right est effectué ou un left - right dans la variable temporaire @tmpX
    F. Le résultat est copié de la variable temporaire @tmpX dans eax

- Le produit, la division et le modulo sont gérés de la manière suivante :
    A. La valeur de l'expression à gauche de l'expression arithmétique est copiée dans eax
    B. une nouvelle variable temporaire @tmpX est créée et est ajoutée dans la table de symboles
    C. l'expression de gauche (dans eax) est copiée dans la variable temporaire pour libérer eax
    D. La valeur de l'expression à droite de l'expression arithmétique est copiée dans eax
    E. En fonction de s'il s'agit d'un produit ou d'une division, il faut gérer l'opération différemment.
        Pour le produit : 
        a. On place simplement le résultat de left * right dans eax

        Pour la division :
        a. On crée une variable temporaire @tmpY qu'on ajoute à la map des symboles
        b. L'expression de droite (dans eax) est copiée dans la variable temporaire @tmpY pour libérer eax
        c. L'expression de gauche (dans @tmpX) est copiée dans eax, ce sera le dividende
        d. On effectue la division left / right puis on place le quotient dans eax et le reste dans edx

        Information complémentaire : 
        Pour la division, le quotien est déjà dans eax tandis que pour le modulo, la valeur dans edx est copiée dans eax.

- L'expression est visiée et est assignée à un IDENTIFIER pour copier sa valeur dans eax, récupère dans la table des symboles la variable correspondante à l'IDENTIFIER, et copie la valeur de l'expression (dans eax) à l'adresse mémoire de la variable



#-#=#-#=#-#=#-#=#-#=#-#=#-# DESCRIPTION DE LA GESTION DE PROJET #-#=#-#=#-#=#-#=#-#=#-#=#-#

Le projet est réalisé selon la méthode AGILE et le TDD (Test Driven Development) c’est-à-dire qu'une première version fonctionnelle du projet est rendue à la fin de chaque sprint avec les tests correspondants. On effectuera deux sprints de durée égale (deux semaines de projet).

La répartition des rôles a été faite selon : 
L'équipe de développement du programme : Corentin JEANNE, Juliette PIERRE
L'équipe de réalisation de fichiers de test : Joris FELZINES, Harold MARTIN
Responsable organisationnel et rapport : Marine QUEMENER
Scrum master : Justine STEPHAN


L'outil de développement est Visual Studio Code et le code est stocké sur GitHub.

