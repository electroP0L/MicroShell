/////// ATTENTION : LANCER AVEC LA COMMANDE SUIVANTE :
//////  valgrind --leak-check=full ./microshell



#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>

#include "fonctions.h"

int main(int argc, char **argv){
  chdir("/");
  char* globalcommand = (char *) malloc(MAX * sizeof(char));                    //On déclare la commande globale, une chaîne de caractères entrée par l'utilisateur qui contiendra les espaces

  while(true){
    char* command = NULL;                                                       //On déclare la variable command qui stockera la commande principale du shell
    char* argument_list[MAX];                                                   //On déclare la liste qui contiendra les arguments associés à la commande principale
    int ispipe = 0;
    int arglen = 0;
    int arrplan = 0;
    int launchout = 0;

    while (command == NULL){                                                    //Tant que la commande entrée n'est pas nulle
      getcommand_arguments(&command, argument_list, globalcommand, &arglen);    //On lance la fonction de récupération de la commande et des arguments
    }

    if (strcmp(command, "exit") == 0){                                          //On compare la commande avec la chaine "exit". Si elles sont identiques
      break;                                                                    //On sort de la boucle while, ce qui nous fait quitter le programme après la libération de la mémoire
    }

    if (strcmp(command, "cd") == 0){
      cd(command, argument_list);
    }

    else {
      for (int i = 0; i < arglen-1; i++){
        int assert = 0;

        if (strcmp(argument_list[i], "&") == 0){
          argument_list[i] = NULL;
          arrplan = i;
          assert =1;
        }

        if ((assert == 0) && (strcmp(argument_list[i], "|") == 0)){
          argument_list[i] = NULL;
          ispipe = i;
          assert =1;
        }

        if ((assert == 0) && (strcmp(argument_list[i], ">") == 0)){
          argument_list[i] = NULL;
          launchout = i;
        }
      }
      if (ispipe != 0){
        launchprocess_pipe(command, argument_list, arglen, ispipe, arrplan);
      }
      else{
        launchprocess(command, argument_list, arrplan, launchout);                 //On lance la fonction de lancement de processus
      }
    }
  }

  free(globalcommand);                                                          //On libère la commande globale
}
