#include <stdio.h>
#include <stdlib.h>
#include "premiers.h"
#include "suivants.h"
#include "syntabs.h"
#include "affiche_arbre_abstrait.h"
#include "analyseur_syntaxique.h"
#include "analyse_semantique.h"

void open_file(char *arg);

char yytext[100];
FILE *yyin;
int DISPLAY_XML       = 0;
int DISPLAY_SYMBOL    = 0;
int DISPLAY_ABSTRACT  = 0;
int DISPLAY_NASM      = 0;
int DISPLAY_TABSYMBOL = 0;

int main(int argc, char **argv) {

  if(argc == 2){

    open_file(argv[1]);
  } else if(argc == 3){

    open_file(argv[2]);

    if(strcmp(argv[1], "-l") == 0){
      DISPLAY_SYMBOL = 1;
      test_yylex_internal(yyin);
    }
    if(strcmp(argv[1], "-s") == 0){
      DISPLAY_XML = 1;
    }
    if(strcmp(argv[1], "-a") == 0){
      DISPLAY_ABSTRACT = 1;
    }
    if(strcmp(argv[1], "-t") == 0){
      DISPLAY_TABSYMBOL = 1;
    }
    if(strcmp(argv[1], "-n") == 0){
      DISPLAY_NASM = 1;
    }
  } else {
    fprintf(stderr, "Usage : compilateur OPTION file_path\n");
  }

  initialise_premiers();
  initialise_suivants();

  n_prog *p = NULL;
  p = programme();
  affiche_n_prog(p);
  parcours_n_prog(p);

  return 0;
}

void open_file(char *arg){
  yyin = fopen(arg, "r");
  if(yyin == NULL){
    fprintf(stderr, "impossible d'ouvrir le fichier %s\n", arg);
    exit(1);
  }
}
