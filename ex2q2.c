#include "second_preim_48_fillme.h"

int main(int argc, char*argv[]){
  int ram=22;
  if(argc<2){
    printf(BLUE"%s\n"RESET,"You can precise ram available by adding RAM=X after make. \n\tEX: make ex2q2 RAM=24 (2^24 = 8GB)\n\tDEFAULT=4GB (22)" );
  }else{
    ram=atoi(argv[1]);
  }
  printf(MAGENTA"\n[EXERCICE 2 QUESTION 2]\n"RESET);
  attack(ram);
  return 0;
}
