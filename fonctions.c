#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>

#include "fonctions.h"                                                          //On include fonctions.h à fonctions.c afin de pouvoir utiliser de manière globale dans tout le programme la varible MAX définie dans fonctions.h

#define READ_END 0                                                              //Pour clarifier lorsque nous écrivions le code, nous avons défini des noms pour les deux côtés du pipe
#define WRITE_END 1

void getcommand_arguments(char** command, char* argument_list[MAX], char* globalcommand, int* arglen){
  char path[99];
  getcwd(path, 99);
  printf("\033[1;32m");                                                         //On met la couleur verte sur le printf
  printf("µShell");
  printf("\033[0m");                                                            //On reset la couleur du printf
  printf(":");
  printf("\033[1;34m");                                                         //On met la couleur bleue sur le printf
  printf("%s", path);
  printf("\033[0m");                                                            //On reset la couleur du printf
  printf("$ ");


  char* retourfgets = fgets(globalcommand, MAX, stdin);                         //On obtient globalcommand à partir du pointeur stdin, qui récupèrera ce qu'on tape dans le shell
  if (retourfgets == NULL){                                                     //Si le programme a rencontré une erreur
    perror("Une erreur de selection a eu lieu\t");                                           //La fonction perror indique la raison précise de l'erreur
  }

  globalcommand[strlen(globalcommand)-1] = 0;                                   //On enlève le dernier caractère, qui est un retour chariot
  const char* separateurs = " ";                                                //On déclare les séparateurs de la fonction strtok
  char* Token = strtok(globalcommand, separateurs);                             //On sépare le premier mot de la grande chaine de caractères
  *command = Token;                                                             //On le définit comme la commande principale envoyée au shell

  argument_list[0] = *command;                                                  //Le premier argument de la liste doit être le même que la commande principale
  *arglen = 1;
  while(Token != NULL){                                                         //Tant que l'on continue à rencontrer des nouveaux mots
    Token = strtok(NULL, separateurs);                                          //Token récupère la valeur de cet argument
    argument_list[*arglen] = Token;                                             //On place l'argument dans la liste d'arguments
    *arglen+=1;
  }
}

int status = 0;

void launchprocess_pipe(char* command, char* argument_list[MAX], int arglen, int ispipe, int arrplan){
  int retourexec1 = 0;                                        //On définit la variable de retour d'execution
  int retourexec2 = 0;                                        //On définit la variable de retour d'execution
  int stat_loc = 0;                                           //stat_loc est une variable utile pour la fonction waitpid
  //int arglen1 = arglen-2;                                   //Les longueurs des chaines d'arguments, On ne s'en sert pas dans le code
  //int arglen2 = 0;

  int fd[2];                                                  //On définit les file descriptors, les deux côtés du pipe

  char* command2 = argument_list[ispipe+1];                   //On Isole la commande de l'autre côté du "|"
  char* argument_list2[MAX];                                  //On déclare la liste d'arguments de l'autre côté du "|"

  for (int i=ispipe+1; i<arglen-1; i++){
    argument_list2[i-(ispipe+1)] = argument_list[i];          //On remplit la liste d'arguments secondaires
    argument_list2[i-(ispipe)] = NULL;                        //En prenant soin de rendre l'élément d'après le dernier argument de la liste secondaire NULL
    argument_list[i] = NULL;                                  //Et de vider la liste primaire en même temps
    //arglen2++;
    //arglen1--;
  }

if (arrplan != 0){                                            //On gère le cas où on souhaite que le programme se déroule à l'arrière plan
    signal(SIGCHLD, handler);
  }

  if(pipe(fd)==-1){                                           //On crée le pipe, tout en vérifiant s'il a bien été créé
      perror("Une erreur de pipe a eu lieu\t");               
  }

  pid_t pid = fork();                                         //Le premier fork, qui engendrera le fils qui executera la commande primaire (ex. ls)

  if (pid == 0){                                              //Dans le fils
    dup2(fd[WRITE_END], STDOUT_FILENO);                       //On écrit sur la sortie standard ce qui sort du pipe
    close (fd[READ_END]);                                     //On ferme la lecture du pipe
    close (fd[WRITE_END]);                                    //On ferme l'ecriture du pipe


    retourexec1 = execvp(command, argument_list);             //On execute la commande primaire
    if (retourexec1 == -1){                                   //Si le programme a rencontré une erreur
      perror("Une erreur d'execution du programme primaire a eu lieu\t");       //La fonction perror indique la raison précise de l'erreur
      exit(0);                                                //On quitte le programme
    }
    
    exit(1);                                                  //On quitte le programme
  }
  
  if (pid == -1){                                             //Si le fork rencontre une erreur
    perror("Une erreur de fork a eu lieu\t");                 //On le fait savoir à l'utilisateur
    exit(0);                                                  //On quitte
  }

  else{
    pid_t pid = fork();                                       //Le deuxième fork, qui engendrera le fils qui executera la commande secondaire (ex. wc)

    if (pid==0){
      dup2(fd[READ_END], STDIN_FILENO);                       //On lit sur l'entrée standard ce qui est entré dans le pipe
      close (fd[WRITE_END]);                                  //On ferme l'ecriture du pipe
      close (fd[READ_END]);                                   //On ferme la lecture du pipe

      retourexec2 = execvp(command2, argument_list2);         //On execute la commande secondaire
      if (retourexec2 == -1){                                 //Si le programme a rencontré une erreur
        perror("Une erreur d'execution du programme secondaire a eu lieu\t");       //La fonction perror indique la raison précise de l'erreur
        exit(0);                                              //On quitte le programme
      }
      
      exit(1);                                                //On quitte le programme
    }

  if (pid == -1){                                             //Si le fork rencontre une erreur
    perror("Une erreur de fork a eu lieu\t");                 //On le fait savoir à l'utilisateur
    exit(0);                                                  //On quitte
  }

    else{                                                     //si on se situe dans le processus parent
      close (fd[READ_END]);                                   //On ferme la lecture du pipe
      close (fd[WRITE_END]);                                  //On ferme l'écriture du pipe
      if(arrplan == 0){
        waitpid(pid, &status, 0);                               //On attend la fin de l'exécution des deux fils si on ne souhaite pas que le programme se déroule à l'arrière plan
      }
    }
  }
}

void launchprocess(char* command, char* argument_list[MAX], int arrplan, int launchout){
  int retourexec = 0;                                                           //On définit la variable de retour d'execution
  int stat_loc = 0;                                                             //stat_loc est une variable utile pour la fonction waitpid
  int save_stdout = dup(1);

  if (arrplan != 0){                                                            //On gère le cas où on souhaite que le programme se déroule à l'arrière plan
    signal(SIGCHLD, handler);
  }

  if (launchout != 0){                                                          //On gère le cas où on souhaite que la sortie du programme se fasse dans un document
    FILE* stream = fopen(argument_list[launchout+1], "w");                      //On ouvre le document donné
    if (stream == NULL){
      perror("Erreur lors de l'ouverture du fichier");
    }

    int file = fileno(stream);                                                  //On récupère son stream
    argument_list[launchout+1] = NULL;                                          //On supprime le nom du document dans les arguments

    dup2(file, 1);                                                              //On écrit dans le document
    fclose(stream);                                                             //On ferme le document
  }

  pid_t process = fork();                                                       //On crée un nouveau processus
  switch(process){
  case 0 :                                                                      //Si le fils a bien été créé
    retourexec = execvp(command, argument_list);                                //On lance la commande ordonnée et ses arguments, et la valeur du retour de la fonction

    if (retourexec == -1){                                                      //Si le programme a rencontré une erreur
      perror("Une erreur d'execution a eu lieu\t");                             //La fonction perror indique la raison précise de l'erreur
    }
    exit(0);

  case -1 :                                                                     //Si un enfant n'a pas été créé
    perror("Une erreur de fork a eu lieu\t");                                   //La fonction perror indique la raison précise de l'erreur
    exit(0);

  default :
    if (arrplan == 0){                                                          //On attend la fin de l'exécution des deux fils si on ne souhaite pas que le programme se déroule à l'arrière plan
      wait(&status);
    }
  }

  if (launchout != 0){                                                          //Si on a écrit dans le document
    dup2(save_stdout, 1);                                                       //On repasse en écriture dans le shell
    close(save_stdout);
  }
}

void handler(int sig){
  wait(&status);
}

void cd(char* command, char* argument_list[MAX]){
  //Se déplacer vers le répertoire parent (cd ..)
  if(argument_list[1]){                                                         //Si on a un argument après cd
    if (strcmp(argument_list[1] , "..") == 0){                                  //Si l'argument est ".."
      chdir("..");                                                              //On se déplace dans le répertoire parent (On s'est rendus compte plus tard qu'il n'était pas nécessaire de traiter ce cas à part mais bon)
    }

    //Se déplacer dans un sous-répertoire (Ex. cd /docs)
    else {                                                                      //Si l'argment n'est pas ".."
      int retouracces = 0;                                                      //On déclare retour accès, qui va stocker le résultat de l'execution de chdir
      char path[99];                                                            //On crée la variable qui va récupérer le chemin auquel on veut accéder (Max 99 caractères)
      getcwd(path, 99);                                                         //On récupère le chemin vers le répertoire actuel (Ex. /docs)
      strcat(path, argument_list[1]);                                           //On le concatère avec le chemin auquel on veut accéder (Ex. /home/alexpaul)
      retouracces = chdir(path);                                                //On accède au chemin complet (Ex. home/alexpaul/docs)
      if (retouracces == -1){                                                   //Si l'accès au chemin est impossible
        perror("Erreur cd ");                                                   //On affiche l'erreur
      }
    }
  }

  else {                                                                        //Si on n'a pas d'argument après cd
    chdir("/");                                                                 //On se déplace dans le répertoire racine, que l'on considère comme le répertoire par défaut
  }
}
