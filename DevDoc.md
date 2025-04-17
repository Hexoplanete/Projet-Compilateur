# Documentation développeurs


## Fonctionnement du compilateur

La compilation s'effectue en 3 étapes : 
1. [Parcours de l'AST](#parcours-de-last)
2. [Génération de IR](#génération-de-ir)
3. [Génération de l'assembleur](#génération-de-lassembleur)

## 1. Parcours de l'AST
L'AST (Abstract Syntax Tree) est généré par `antlr` en utilisant la grammaire [`ifcc.g4`](compiler/ifcc.g4). On peut le visualiser avec la commande [`make gui`](README.md#affichage-de-last).
L'arbre est ensuite parcouru par le visiteur [`SymbolMapVisitor`](`compiler/SymbolMapVisitor.h`) pour détecter les erreurs d’utilisation des variables : variable non déclarée, non utilisée ou redéfinie.

## 2. Génération de IR
Afin de permettre une redirection vers différentes architectures et de faciliter la génération d'assembleur, un [control flow graph](compiler/ir/ControlFlowGraph.h) contenant des [instructions génériques](compiler/ir/Instruction.h) (l'IR) est généré grâce à un parcours de l'AST par le [`CFGVisitor`](compiler/CFGVisitor.cpp).

Lors de la conception des instructions, nous avons décidé que toute expression donnant un résultat le stockera dans un registre commun (`eax` sous `x68`).

Les statements tels que `if` et `while` créent des blocs dédiés afin de gérer le flux d’exécution du code. Chaque pair de `{}` nous fait entrer puis sortir d'un contexte afin de gérer la portée des variables et le shadowing.

## 3. Génération de l'assembleur
La dernière étape pour obtenir notre assembleur est de parcourir le cfg et son contenu : chaque blocw et chaque instruction génère son assembleur dans le fichier final.

Grâce à l'IR, il suffit de créer un nouveau fichier `Instruction_arm.cpp` et l'utiliser à la place de `Instruction.cpp` pour générer de l'assembleur pour l'architecture `ARM` au lieu de `x86`.
