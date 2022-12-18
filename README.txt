
/////////////               READ ME         \\\\\\\\\\\\\\\\\
////////////          Projet Minishell       \\\\\\\\\\\\\\\\\

-------------------------------
Lancement du programme : 

Placer l'ensemble des fichiers de l'archive dans un dossier
Ouvrir le dossier code_microshell dans un terminal
Taper "make"
Taper "./microshell"

Vous êtes dans le microshell !

-------------------------------
Exercices traités : 

Fonctions de base
1.1 Éxecution de programmes sans arguments au premier plan (ex : ls)
1.2 Exécution de programmes avec des arguments (ex : ls -l)
1.3 Builtin cd (ex : cd)

Fonctions avancées
2.1 Commandes en arrière plan (ex : who &)
2.2 Tuyaux (ex : ls - la | wc)
2.3 Redirection de stdout (ex : ls > "test.txt")

-> Nous ne rencontrons pas d'erreur de segmentation lors d'un utilisation normale (tests avec Valgrind)
-> Nous avons travaillé pour avoir un code propre

------------------------------
Exercices non traités : 

Bonus
3.1 Chaîne de commande
3.2 Chaîne de tuyaux
