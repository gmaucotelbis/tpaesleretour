#include "second_preim_48_fillme.h"

int main(int argc, char*argv[]){
  int ram=22;
  if(argc<2){
    printf(BLUE"%s\n"RESET,"You can precise ram available by adding RAM=X after make. \n\tEX: make ex2q2 RAM=24 (2^24 = 8GB)\n\tDEFAULT=4GB (22)" );
  }else{
    ram=atoi(argv[1]);
  }
  printf(MAGENTA"\n[EXERCICE 2 QUESTION 2]\n"RESET);
  //attack(ram);
  uint32_t *second_m = malloc(sizeof(uint32_t)*262144*4);

  for (int i = 0; i < (1 << 20); i+=4){
    second_m[i + 0] = i;
    second_m[i + 1] = 0;
    second_m[i + 2] = 0;
    second_m[i + 3] = 0;
  }

  uint64_t h = hs48(second_m,262144,1,0);
  printf("%s\n","HASH OF THE SECOND MESSAGE" );
  printf("\t%" PRIx64 "\n",h);


  return 0;
}
