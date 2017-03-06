#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "symboles.h"
#include "analyseur_syntaxique.h"
#include "util.h"

extern int DISPLAY_XML;

/*******************************************************************************
 * Fonction simplifiant l'envoi d'une erreur.
 ******************************************************************************/
void err(char *attendu){
  char nom[100];
  char valeur[100];
  nom_token( uniteCourante, nom, valeur );

  char err[256];
  sprintf(err, "\n\tATTENDU : %s\n\tOBTENU : %s\t%s", attendu, nom, valeur);

  erreur(err);
}

/*******************************************************************************
 * Fonction simplifiant l'affichage d'une variable ou d'un symbole.
 ******************************************************************************/
void consommer(int symbole){

  char nom[100];
  char valeur[100];
  nom_token( symbole, nom, valeur );

  if(uniteCourante != symbole)
    err(nom);

  nom_token( uniteCourante, nom, valeur );

  affiche_element(nom, valeur, DISPLAY_XML);
  uniteCourante = yylex();
}


/*******************************************************************************
 * Grammaire non ambigüexpArith, non recursive et factoriséexpArith à gauche du langage L
 *
 * Conventions: symboles terminaux entre 'apostrophes' ou 100% MAJUSCULES
 *              symboles non terminaux commencent par lettre minuscule
 *              symbole initial (axiome) à gauche de la première production
 *              productions ont la forme nonTerminal -> seq1 | seq2 |...| seqN
 *              production vide représentéexpArith par barre verticale à la fin
 *              espaces obligatoires entre symboles, flèche -> et alternative |
 ******************************************************************************/

/*******************************************************************************
 * Fonction principale de la grammaire correspondant à la règle :
 * programme -> optDecVariables listeDecFonctions
 ******************************************************************************/
 n_prog *programme(void){
   n_l_dec *$1 = NULL;   n_l_dec *$2 = NULL;
   n_prog *$$ = NULL;

   affiche_balise_ouvrante(__FUNCTION__, DISPLAY_XML);

  uniteCourante = yylex();
  $1 = optDecVariables();
  $2 = listeDecFonctions();
  $$ = cree_n_prog($1, $2);

  affiche_balise_fermante(__FUNCTION__, DISPLAY_XML);

  return $$;
}


/*******************************************************************************
 *
 * Liste de déclaration de variables (ou paramètres d'une fonction)
 *
 ******************************************************************************/

/*******************************************************************************
 * Fonction de la grammaire correspondant à la règle :
 * optDecVariables -> listeDecVariables ';'
 *                  | vide
 ******************************************************************************/
n_l_dec *optDecVariables(void){
  n_l_dec *$1 = NULL;
  affiche_balise_ouvrante(__FUNCTION__, DISPLAY_XML);
  if(est_premier(_listeDecVariables_, uniteCourante)){
    $1 = listeDecVariables();
    consommer( POINT_VIRGULE );
  } else if(est_suivant(_optDecVariables_, uniteCourante)){
  } else {
    err("P(listeDecVariables) OU VIDE");
  }
  affiche_balise_fermante(__FUNCTION__, DISPLAY_XML);
  return $1;
}

/*******************************************************************************
 * Fonction de la grammaire correspondant à la règle :
 * listeDecVariables -> declarationVariable listeDecVariablesBis
 ******************************************************************************/
n_l_dec *listeDecVariables(void){
  n_dec *$1 = NULL; n_l_dec *$2 = NULL; n_l_dec *$$ = NULL;
  affiche_balise_ouvrante(__FUNCTION__, DISPLAY_XML);
  if(est_premier(_declarationVariable_, uniteCourante)){
    $1 = declarationVariable();
    $2 = listeDecVariablesBis();
    $$ = cree_n_l_dec($1, $2);
  } else {
    err("P(declarationVariable)");
  }
  affiche_balise_fermante(__FUNCTION__, DISPLAY_XML);
  return $$;
}

/*******************************************************************************
 * Fonction de la grammaire correspondant à la règle :
 * listeDecVariablesBis -> ',' declarationVariable listeDecVariablesBis
 *                       |
 ******************************************************************************/
n_l_dec *listeDecVariablesBis(){
  n_dec *$2 = NULL; n_l_dec *$3 = NULL;  n_l_dec *$$ = NULL;
  affiche_balise_ouvrante(__FUNCTION__, DISPLAY_XML);
  if(uniteCourante == VIRGULE){
    consommer( VIRGULE );
    $2 = declarationVariable();
    $3 = listeDecVariablesBis();
    $$ = cree_n_l_dec($2, $3);
  } else if(est_suivant(_listeDecVariablesBis_, uniteCourante)){
    // $$ = herite; On retourne NULL
  } else {
    err("VIRGULE ou VIDE");
  }
  affiche_balise_fermante(__FUNCTION__, DISPLAY_XML);
  return $$;
}

/*******************************************************************************
 * Fonction de la grammaire correspondant à la règle :
 * declarationVariable -> ENTIER ID_VAR optTailleTableau
 ******************************************************************************/
n_dec *declarationVariable(void){
  int *$3 = NULL; n_dec *$$ = NULL;
  char[100] nom;
  affiche_balise_ouvrante(__FUNCTION__, DISPLAY_XML);
  if(uniteCourante == ENTIER){
    consommer( ENTIER );
    nom_token( uniteCourante, nom, NULL );
    consommer( ID_VAR );
    $3 = optTailleTableau();
    if( $3 > 0 ){
      $$ = cree_n_dec_tab(nom, $3)
    } else {
      $$ = cree_n_dec_var(nom);
    }
  } else {
    err("POINT_VIRGULE");
  }
  affiche_balise_fermante(__FUNCTION__, DISPLAY_XML);
  return $$;
}

/*******************************************************************************
 * Fonction de la grammaire correspondant à la règle :
 * optTailleTableau -> '[' NOMBRE ']'
 *                   | vide
 ******************************************************************************/
int optTailleTableau(void){
  int $2 = 0;
  char[100] valeur;
  affiche_balise_ouvrante(__FUNCTION__, DISPLAY_XML);
  if(uniteCourante == CROCHET_OUVRANT){
    consommer( CROCHET_OUVRANT );
    nom_token( uniteCourante, NULL, valeur);
    $2 = atoi($2);
    consommer( NOMBRE );
    consommer( CROCHET_FERMANT );
  } else if(est_suivant(_optTailleTableau_, uniteCourante)){
    $2 = 0;
  } else {
    err("CROCHET_OUVRANT ou VIDE");
  }
  affiche_balise_fermante(__FUNCTION__, DISPLAY_XML);
  return $2;
}

/*******************************************************************************
 * Fonction de la grammaire correspondant à la règle :
 * listeDecFonctions -> declarationFonction listeDecFonctions
 *                    |
 ******************************************************************************/
 n_l_dec *listeDecFonctions(void){
   n_dec *$1 = NULL;  n_l_dec *$2 = NULL; n_l_dec *$$ = NULL;
   affiche_balise_ouvrante(__FUNCTION__, DISPLAY_XML);
   if(est_premier(_declarationFonction_, uniteCourante)){
     $1 = declarationFonction();
     $2 = listeDecFonctions();
     $$ = cree_n_l_dec($1, $2);
   } else if(est_suivant(_listeDecFonctions_, uniteCourante)){
     // return NULL
   } else {
     err("P(declarationFonction) ou VIDE");
   }
   affiche_balise_fermante(__FUNCTION__, DISPLAY_XML);
   return $$;
 }

/*******************************************************************************
 * Fonction de la grammaire correspondant à la règle :
 * declarationFonction -> ID_FCT listeParam optDecVariables instructionBloc
 ******************************************************************************/
n_dec *declarationFonction(void){
  n_l_dec *$2 = NULL; n_l_dec *$3 = NULL; n_instr *$4 = NULL; n_dec *$$ = NULL;
  char[100] nom;
  affiche_balise_ouvrante(__FUNCTION__, DISPLAY_XML);
  if(uniteCourante == ID_FCT){
    nom_token( uniteCourante, nom, NULL);
    consommer(ID_FCT);
    $2 = listeParam();
    $3 = optDecVariables();
    $4 = instructionBloc();
    $$ = cree_n_dec_fonc(nom, $2, $3, $4);
  } else {
    err("ID_FCT");
  }
  affiche_balise_fermante(__FUNCTION__, DISPLAY_XML);
  return $$;
}

/*******************************************************************************
 * Fonction de la grammaire correspondant à la règle :
 * listeParam -> '(' optListeDecVariables ')'
 ******************************************************************************/
n_l_dec *listeParam(void){
  n_l_dec *$2 = NULL; //n_l_dec *$$ = NULL;
  affiche_balise_ouvrante(__FUNCTION__, DISPLAY_XML);
  if(uniteCourante == PARENTHESE_OUVRANTE){
    consommer( PARENTHESE_OUVRANTE );
    $2 = optListeDecVariables();
    consommer( PARENTHESE_FERMANTE );
  } else {
    err("PARENTHESE_OUVRANTE");
  }
  affiche_balise_fermante(__FUNCTION__, DISPLAY_XML);
  return $2;
}

/*******************************************************************************
 * Fonction de la grammaire correspondant à la règle :
 * optListeDecVariables -> listeDecVariables
 *                       |
 ******************************************************************************/
n_l_dec *optListeDecVariables(void){
  n_l_dec *$1 = NULL;
  affiche_balise_ouvrante(__FUNCTION__, DISPLAY_XML);
  if(est_premier(_listeDecVariables_, uniteCourante)){
    $1 = listeDecVariables();
  } else if(est_suivant(_optListeDecVariables_, uniteCourante)){
    // return NULL
  } else {
    err("P(listeDecVariables) ou VIDE");
  }
  affiche_balise_fermante(__FUNCTION__, DISPLAY_XML);
  return $1;
}


/*******************************************************************************
 *
 * Instructions dans le corps d'une fonction (bloc)
 *
 ******************************************************************************/

/*******************************************************************************
 * Fonction de la grammaire correspondant à la règle :
 * instruction -> instructionAffect
 *          | instructionBloc
 *          | instructionSi
 *          | instructionTantque
 *          | instructionAppel
 *          | instructionRetour
 *          | instructionEcriture
 *          | instructionVide
 ******************************************************************************/
n_instr *instruction(void){
  affiche_balise_ouvrante(__FUNCTION__, DISPLAY_XML);
  if(est_premier(_instructionAffect_, uniteCourante)){
    return instructionAffect();
  } else if(est_premier(_instructionBloc_, uniteCourante)){
    return instructionBloc();
  } else if(est_premier(_instructionSi_, uniteCourante)){
    return instructionSi();
  } else if(est_premier(_instructionTantque_, uniteCourante)){
    return instructionTantque();
  } else if(est_premier(_instructionAppel_, uniteCourante)){
    return instructionAppel();
  } else if(est_premier(_instructionRetour_, uniteCourante)){
    return instructionRetour();
  } else if(est_premier(_instructionEcriture_, uniteCourante)){
    return instructionEcriture();
  } else if(est_premier(_instructionVide_, uniteCourante)){
    return instructionVide();
  } else if(est_premier(_instructionFaire_, uniteCourante)){
    return instructionFaire();
  } else {
    err("P(instruction...)");
  }
  affiche_balise_fermante(__FUNCTION__, DISPLAY_XML);
}

/*******************************************************************************
 * Fonction de la grammaire correspondant à la règle :
 * instructionAffect -> var '=' expression ';'
 ******************************************************************************/
 n_instr *instructionAffect(void){
   n_var *$1 = NULL;  n_exp *$3 = NULL; n_instr *$$ = NULL;
   affiche_balise_ouvrante(__FUNCTION__, DISPLAY_XML);
   if(est_premier(_var_, uniteCourante)){
     $1 = var();
     consommer( EGAL );
     $2 = expression();
     consommer( POINT_VIRGULE );
     $$ = cree_n_instr_affect($1, $2);
   } else {
     err("P(var)");
   }
   affiche_balise_fermante(__FUNCTION__, DISPLAY_XML);
   return $$;
 }

/*******************************************************************************
 * Fonction de la grammaire correspondant à la règle :
 * instructionBloc -> '{' listeInstructions '}'
 ******************************************************************************/
n_l_instr *instructionBloc(void) {
  n_l_dec *$2 = NULL; n_l_instr *$$ = NULL;
  affiche_balise_ouvrante(__FUNCTION__, DISPLAY_XML);
  if(uniteCourante == ACCOLADE_OUVRANTE){
    consommer( ACCOLADE_OUVRANTE );
    $2 = listeInstructions();
    $$ = cree_n_instr_bloc($2);
    consommer( ACCOLADE_FERMANTE );
  } else {
    err("ACCOLADE_OUVRANTE");
  }
  affiche_balise_fermante(__FUNCTION__, DISPLAY_XML);
  return $$;
}

/*******************************************************************************
 * Fonction de la grammaire correspondant à la règle :
 * listeInstructions -> instruction listeInstructions
 *                    |
 ******************************************************************************/
n_l_instr *listeInstructions(void) {
  n_instr *$1 = NULL; n_l_instr *$2 = NULL; n_l_instr *$$ = NULL;
  affiche_balise_ouvrante(__FUNCTION__, DISPLAY_XML);
  if(est_premier(_instruction_, uniteCourante)){
    $1 = instruction();
    $2 = listeInstructions();
    $$ = cree_n_l_instr($1, $2);
  } else if(est_suivant(_listeInstructions_, uniteCourante)){
  } else {
    err("P(instruction) ou VIDE");
  }
  affiche_balise_fermante(__FUNCTION__, DISPLAY_XML);
  return $$;
}

/*******************************************************************************
 * Fonction de la grammaire correspondant à la règle :
 * instructionAppel -> appelFct ';'
 ******************************************************************************/
n_instr *instructionAppel(void) {
  n_appel *$1 = NULL; n_instr *$$ = NULL;
  affiche_balise_ouvrante(__FUNCTION__, DISPLAY_XML);
  if(est_premier(_appelFct_, uniteCourante)){
    $1 = appelFct();
    $$ = cree_n_instr_appel($1);
    consommer( POINT_VIRGULE );
  } else {
    err("P(appelFct)");
  }
  affiche_balise_fermante(__FUNCTION__, DISPLAY_XML);
  return $$;
}

/*******************************************************************************
 * Fonction de la grammaire correspondant à la règle :
 * instructionSi -> SI expression ALORS instructionBloc optSinon
 ******************************************************************************/
n_instr *instructionSi(void) {
  n_instr *$$ = NULL;   n_exp *$2 = NULL;
  n_instr *$4 = NULL;   n_instr *$5 = NULL;
  affiche_balise_ouvrante(__FUNCTION__, DISPLAY_XML);
  if(uniteCourante == SI){
    consommer( SI );
    $2 =  expression();
    consommer( ALORS);
    $4 = instructionBloc();
    $5 = optSinon();
    $$ = cree_n_instr_si($2, $4, $5);
  } else {
    err("SI");
  }
  affiche_balise_fermante(__FUNCTION__, DISPLAY_XML);
  return $$;
}

/*******************************************************************************
 * Fonction de la grammaire correspondant à la règle :
 * optSinon -> SINON instructionBloc
 *          |
 ******************************************************************************/
n_instr *optSinon(void) {
  n_instr *$2 = NULL;
  affiche_balise_ouvrante(__FUNCTION__, DISPLAY_XML);
  if(uniteCourante == SINON){
    consommer( SINON );
    $2 = instructionBloc();

  } else if(est_suivant(_optSinon_, uniteCourante)){
  } else {
    err("SINON ou VIDE");
  }
  affiche_balise_fermante(__FUNCTION__, DISPLAY_XML);
  return $2;
}

/*******************************************************************************
 * Fonction de la grammaire correspondant à la règle :
 * instructionTantque -> TANTQUE expression FAIRE instructionBloc
 ******************************************************************************/
n_instr *instructionTantque(void) {
  n_exp *$2 = NULL; n_instr *$4 = NULL; n_instr *$$ = NULL;
  affiche_balise_ouvrante(__FUNCTION__, DISPLAY_XML);
  if(uniteCourante == TANTQUE){
    consommer( TANTQUE );
    $2 = expression();
    consommer( FAIRE );
    $4 = instructionBloc();
    $$ = cree_n_instr_tantque($2, $4);
  } else {
    err("TANTQUE");
  }
  affiche_balise_fermante(__FUNCTION__, DISPLAY_XML);
  return $$;
}

/*******************************************************************************
 * Fonction de la grammaire correspondant à la règle :
 * instructionRetour -> RETOUR expression ';'
 ******************************************************************************/
n_instr *instructionRetour(void) {
  n_exp *$2 = NULL; n_instr *$$ = NULL;
  affiche_balise_ouvrante(__FUNCTION__, DISPLAY_XML);
  if(uniteCourante == RETOUR){
    consommer( RETOUR );
    $2 = expression();
    consommer( POINT_VIRGULE );
    $$ = cree_n_instr_retour($2);
  } else {
    err("RETOUR");
  }
  affiche_balise_fermante(__FUNCTION__, DISPLAY_XML);
  return $$;
}

/*******************************************************************************
 * Fonction de la grammaire correspondant à la règle :
 * instructionEcriture -> ECRIRE '(' expression ')' ';'
 ******************************************************************************/
n_instr *instructionEcriture(void) {
  n_exp *$3 = NULL; n_instr *$$ = NULL;
  affiche_balise_ouvrante(__FUNCTION__, DISPLAY_XML);
  if(uniteCourante == ECRIRE){
    consommer( ECRIRE );
    consommer( PARENTHESE_OUVRANTE );
    $3 = expression();
    consommer( PARENTHESE_FERMANTE );
    consommer( POINT_VIRGULE );
    $$ = cree_n_instr_ecrire($3);
  } else {
    err("ECRIRE");
  }
  affiche_balise_fermante(__FUNCTION__, DISPLAY_XML);
  return $$;
}
/*******************************************************************************
 * Fonction de la grammaire correspondant à la règle :
 * instructionVide -> ';'
 ******************************************************************************/
n_instr *instructionVide(void) {
  n_instr *$$ = NULL;
  affiche_balise_ouvrante(__FUNCTION__, DISPLAY_XML);
  if(uniteCourante == POINT_VIRGULE){
    consommer( POINT_VIRGULE );
    $$ = cree_n_instr_vide();
  } else {
    err("POINT_VIRGULE");
  }
  affiche_balise_fermante(__FUNCTION__, DISPLAY_XML);
  return $$;
}

/*******************************************************************************
 *
 * Expressions (booléennes, arithmétiques) avec implémentation des priorités.
 * Ordre croissant de priorités comme suit (le | a la plus petite priorité):
 * 7. | (ou)
 * 6. & (et)
 * 5. < =
 * 4. + -
 * 3. * /
 * 2. ! (non)
 * 1. (expression) appelFct var[expression]
 *
 ******************************************************************************/

/*******************************************************************************
 * Fonction de la grammaire correspondant à la règle :
 * expression -> conjonction expressionBis
 ******************************************************************************/
n_exp *expression(void) { //TODO:
  n_exp *$1 = NULL; n_exp *$2 = NULL; n_exp *$$ = NULL;
    affiche_balise_ouvrante(__FUNCTION__, DISPLAY_XML);
    $1 = conjonction();
    $2 = expressionBis();
    //
    affiche_balise_fermante(__FUNCTION__, DISPLAY_XML);
    return $$;
}

/*******************************************************************************
 * Fonction de la grammaire correspondant à la règle :
 * expressionBis -> '|' conjonction expressionBis
 *                |
 ******************************************************************************/
 n_exp *expressionBis(void) {

   affiche_balise_ouvrante(__FUNCTION__, DISPLAY_XML);
   if(uniteCourante == OU){
     consommer( OU );

     if(est_premier(_conjonction_, uniteCourante)){
       conjonction();
       expressionBis();
     } else {
       err("P(conjonction)");
     }
   } else if(est_suivant(_expressionBis_, uniteCourante)){
   } else {
     err("OU ou VIDE");
   }
   affiche_balise_fermante(__FUNCTION__, DISPLAY_XML);
 }

/*******************************************************************************
 * Fonction de la grammaire correspondant à la règle :
 * conjonction -> comparaison conjonctionBis
 ******************************************************************************/
n_exp *conjonction(void) {
  n_exp *$1 = NULL; n_exp *$2 = NULL; n_esp *$$ = NULL;
  affiche_balise_ouvrante(__FUNCTION__, DISPLAY_XML);
  $1 = comparaison();
  $2 = conjonctionBis();
  $$ = cree_n
  affiche_balise_fermante(__FUNCTION__, DISPLAY_XML);
}

/*******************************************************************************
 * Fonction de la grammaire correspondant à la règle :
 * conjonctionBis -> '&' comparaison conjonctionBis
 *                |
 ******************************************************************************/
void conjonctionBis(void) {
  affiche_balise_ouvrante(__FUNCTION__, DISPLAY_XML);
  if(uniteCourante == ET){
    consommer( ET );
    comparaison();
    conjonctionBis();
  } else if(est_suivant(_conjonctionBis_, uniteCourante)){
  } else {
    err("ET ou VIDE");
  }
  affiche_balise_fermante(__FUNCTION__, DISPLAY_XML);
}

/*******************************************************************************
 * Fonction de la grammaire correspondant à la règle :
 * comparaison -> expArith comparaisonBis
 ******************************************************************************/
 void comparaison(void) {
     affiche_balise_ouvrante(__FUNCTION__, DISPLAY_XML);
     expArith();
     comparaisonBis();
     affiche_balise_fermante(__FUNCTION__, DISPLAY_XML);
 }

/*******************************************************************************
 * Fonction de la grammaire correspondant à la règle :
 * comparaisonBis -> '=' expArith comparaisonBis
 *                 | '<' expArith comparaisonBis
 *                 |
 ******************************************************************************/
 void comparaisonBis(void) {
   affiche_balise_ouvrante(__FUNCTION__, DISPLAY_XML);
   if(uniteCourante == EGAL){
     consommer( EGAL );
     expArith();
     comparaisonBis();
   } else if(uniteCourante == INFERIEUR){
     consommer( INFERIEUR );
     expArith();
     comparaisonBis();
   } else if(est_suivant(_comparaisonBis_, uniteCourante)){
   } else {
     err("EGAL ou INFERIEUR ou VIDE");
   }
   affiche_balise_fermante(__FUNCTION__, DISPLAY_XML);
 }

/*******************************************************************************
 * Fonction de la grammaire correspondant à la règle :
 * expArith -> terme expArithBis
 ******************************************************************************/
void expArith(void) {
    affiche_balise_ouvrante(__FUNCTION__, DISPLAY_XML);
    terme();
    expArithBis();
    affiche_balise_fermante(__FUNCTION__, DISPLAY_XML);
}

/*******************************************************************************
 * Fonction de la grammaire correspondant à la règle :
 * expArithBis -> '+' terme expArithBis
 *              | '-' terme expArithBis
 *              |
 ******************************************************************************/
void expArithBis(void) {
  affiche_balise_ouvrante(__FUNCTION__, DISPLAY_XML);
  if(uniteCourante == PLUS){
    consommer( PLUS );
    terme();
    expArithBis();
  } else if(uniteCourante == MOINS){
    consommer( MOINS );
    terme();
    expArithBis();
  } else if(est_suivant(_expArithBis_, uniteCourante)){

  } else {
    err("PLUS ou MOINS ou VIDE");
  }
  affiche_balise_fermante(__FUNCTION__, DISPLAY_XML);
}

/*******************************************************************************
 * Fonction de la grammaire correspondant à la règle :
 * terme -> negation termeBis
 ******************************************************************************/
void terme(void) {
  affiche_balise_ouvrante(__FUNCTION__, DISPLAY_XML);
  negation();
  termeBis();
  affiche_balise_fermante(__FUNCTION__, DISPLAY_XML);
}

/*******************************************************************************
 * Fonction de la grammaire correspondant à la règle :
 * termeBis -> '*' negation termeBis
 *           | '/' negation termeBis
 *           |
 ******************************************************************************/
n_exp *termeBis( n_exp *herite ) {
  n_exp *$2;    n_exp *$$;    n_exp *herite_fils;
  affiche_balise_ouvrante(__FUNCTION__, DISPLAY_XML);
  if(uniteCourante == FOIS){
    consommer( FOIS );
    $2 = negation();
    herite_fils = cree_n_exp_op(FOIS, herite, $2);
    $$ = termeBis( herite_fils );
  } else if(uniteCourante == DIVISE){
    consommer( DIVISE );
    $2 = negation();
    herite_fils = cree_n_exp_op(DIVISE, herite, $2);
    $$ = termeBis();
  } else if(est_suivant(_termeBis_, uniteCourante)){
    $$ = herite;
  } else {
    err("FOIS ou DIVISE ou VIDE");
  }
  affiche_balise_fermante(__FUNCTION__, DISPLAY_XML);
  return $$;
}

/*******************************************************************************
 * Fonction de la grammaire correspondant à la règle :
 * negation -> '!' negation
 *           | facteur
 ******************************************************************************/
void negation(void) {
  affiche_balise_ouvrante(__FUNCTION__, DISPLAY_XML);
  if(uniteCourante == NON){
    consommer( NON );
    negation();
  } else if(est_premier(_facteur_, uniteCourante)){
    facteur();
  } else {
    err("NON ou VIDE");
  }
  affiche_balise_fermante(__FUNCTION__, DISPLAY_XML);
}

/*******************************************************************************
 * Fonction de la grammaire correspondant à la règle :
 * facteur -> '(' expression ')'
 *          | NOMBRE
 *          | appelFct
 *          | var
 *          | LIRE '(' ')'
 ******************************************************************************/
void facteur(void) {
  affiche_balise_ouvrante(__FUNCTION__, DISPLAY_XML);
  if(uniteCourante == PARENTHESE_OUVRANTE){
    consommer( PARENTHESE_OUVRANTE );
    expression();
    consommer( PARENTHESE_FERMANTE );
  } else if(uniteCourante == NOMBRE) {
    consommer( NOMBRE );
  } else if(est_premier(_appelFct_, uniteCourante)) {
    appelFct();
  } else if(est_premier(_var_, uniteCourante)) {
    var();
  } else if(uniteCourante == LIRE) {
    consommer( LIRE );
    consommer( PARENTHESE_OUVRANTE );
    consommer( PARENTHESE_FERMANTE );
  } else {
    err("PARENTHESE_OUVRANTE ou NOMBRE ou P(appelFct) ou P(var) ou LIRE");
  }
  affiche_balise_fermante(__FUNCTION__, DISPLAY_XML);
}

/*******************************************************************************
 *
 * Accès à une variable, case d'un tableau ou appel à fonction
 *
 ******************************************************************************/

/*******************************************************************************
 * Fonction de la grammaire correspondant à la règle :
 * var -> ID_VAR optIndice
 ******************************************************************************/
void var(void) {
  affiche_balise_ouvrante(__FUNCTION__, DISPLAY_XML);
  if(uniteCourante == ID_VAR){
    consommer( ID_VAR );
    optIndice();
  } else {
    err("ID_VAR");
  }
  affiche_balise_fermante(__FUNCTION__, DISPLAY_XML);
}
/*******************************************************************************
 * Fonction de la grammaire correspondant à la règle :
 * optIndice -> '[' expression ']'
 *           |
 ******************************************************************************/
void optIndice(void) {
  affiche_balise_ouvrante(__FUNCTION__, DISPLAY_XML);
  if(uniteCourante == CROCHET_OUVRANT){
    consommer( CROCHET_OUVRANT );
    expression();
    consommer( CROCHET_FERMANT );
  } else if(est_suivant(_optIndice_, uniteCourante)){

  } else {
    err("CROCHET_OUVRANT ou VIDE");
  }
  affiche_balise_fermante(__FUNCTION__, DISPLAY_XML);
}
/*******************************************************************************
 * Fonction de la grammaire correspondant à la règle :
 * appelFct -> ID_FCT '(' listeExpressions ')'
 ******************************************************************************/
void appelFct(void) {
  affiche_balise_ouvrante(__FUNCTION__, DISPLAY_XML);
  if(uniteCourante == ID_FCT){
    consommer( ID_FCT );
    consommer( PARENTHESE_OUVRANTE );
    listeExpressions();
    consommer( PARENTHESE_FERMANTE );
  } else {
    err("ID_FCT");
  }
  affiche_balise_fermante(__FUNCTION__, DISPLAY_XML);
}
/*******************************************************************************
 * Fonction de la grammaire correspondant à la règle :
 * listeExpressions -> expression listeExpressionsBis
 *                  |
 ******************************************************************************/
void listeExpressions(void){
  affiche_balise_ouvrante(__FUNCTION__, DISPLAY_XML);
  if(est_premier(_expression_, uniteCourante)){
    expression();
    listeExpressionsBis();
  } else if(est_suivant(_listeExpressions_, uniteCourante)){

  } else {
    err("P(expression) ou VIDE");
  }
  affiche_balise_fermante(__FUNCTION__, DISPLAY_XML);
}

/*******************************************************************************
 * Fonction de la grammaire correspondant à la règle :
 * listeExpressionsBis -> ',' expression listeExpressionsBis
 *                     |
 ******************************************************************************/
void listeExpressionsBis(void) {
  affiche_balise_ouvrante(__FUNCTION__, DISPLAY_XML);
  if(uniteCourante == VIRGULE){
    consommer( VIRGULE );
    expression();
    listeExpressionsBis();
  } else if(est_suivant(_listeExpressionsBis_, uniteCourante)){

  } else {
    err("VIRGULE ou VIDE");
  }
  affiche_balise_fermante(__FUNCTION__, DISPLAY_XML);
}

/*******************************************************************************
 * Fonction de la grammaire correspondant à la règle :
 * instructionFaire -> faire IB tantque E;
 ******************************************************************************/
void instructionFaire(void) {
  affiche_balise_ouvrante(__FUNCTION__, DISPLAY_XML);
  if(uniteCourante == FAIRE){
    consommer( FAIRE );
    instructionBloc();
    consommer( TANTQUE );
    expression();
    consommer( POINT_VIRGULE );
  } else {
    err("FAIRE");
  }
  affiche_balise_fermante(__FUNCTION__, DISPLAY_XML);
}
