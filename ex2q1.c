#include "second_preim_48_fillme.h"

int main(int argc, char* argv[]){
  printf(MAGENTA "\n[EXERCICE 2 QUESTION 1]\n" RESET);
  int ram=22;
  if(argc<2){
    printf(BLUE"%s\n"RESET,"You can precise ram available by adding after make. \n\tEX: make ex2q2 RAM=24 (2^24 = 8GB)\n\tDEFAULT=4GB (22)" );
  }else{
    ram=atoi(argv[1]);
  }
  if(test_em(ram)){
    printf(GREEN "\n\tEX2Q1 VALID.\n\n" RESET);
  }else{
    printf(RED "\n\tEX2Q1 FAIL.\n\n" RESET);
  }
  return 0;
}
