#include "second_preim_48_fillme.h"

int main(int argc, char*argv[]){
  int ram=22;
  if(argc<2){
      printf(BLUE"%s\n"RESET,"You can precise ram available by adding after make. \n\tEX: make ex2q2 RAM=24 (2^24 = 16GB)\n\tDEFAULT=4GB (22)" );
  }else{
    ram=atoi(argv[1]);
  }
  printf(MAGENTA"\n[EXERCICE 2 QUESTION 2]\n"RESET);
  attack(ram);
  // uint32_t m1[4]= {0x00933982,	0x00ca9038,	0x0046b88b,	0x0045bd5a};
  // uint32_t m2[4]= {0x00365e10,	0x00cfd64a,	0x004a7191,	0x0013ef44};
  // uint32_t fp[4]= {0x00036207,	0x0083f589,	0x00db9988,	0x008b7535};
  // uint32_t nb_block = 245774;
  // uint32_t *mf = malloc(sizeof(uint32_t)*(1<<18)*4);
  //
  // for (int i = 0; i < 4; i++)
  //   mf[i] = m1[i];
  //
  // //ADD nb_block FIXED POINT
  // for(int i=1; i<nb_block; i++){
  //   mf[i*4] = fp[0];
  //   mf[i*4+1] = fp[1];
  //   mf[i*4+2] = fp[2];
  //   mf[i*4+3] = fp[3];
  // }
  //
  // mf[nb_block*4] = m2[0];
  // mf[nb_block*4+1] = m2[1];
  // mf[nb_block*4+2] = m2[2];
  // mf[nb_block*4+3] = m2[3];
  // //84e1f3b01e0c
  // // //ADD THE REMAINING OF THE MESSAGE
  // for (int i = (nb_block+1)*4; i < (1<<20); i+=4){
  //   mf[i + 0] = i;
  //   mf[i + 1] = 0;
  //   mf[i + 2] = 0;
  //   mf[i + 3] = 0;
  // }
  //
  // printf("%s\n","HASH" );
  // uint64_t h=hs48(mf,(1<<18),1,0);
  // printf("\t%" PRIx64 "\n",h);

  // for (int i = 0; i < nb_block; i+=4){
  //   mf[i + 0] = i;
  //   mf[i + 1] = 0;
  //   mf[i + 2] = 0;
  //   mf[i + 3] = 0;
  // }

  // h = hs48(mf,nb_block,0,0);
  // printf("%s\n","HASH OF THE SECOND MESSAGE" );
  // printf("\t%" PRIx64 "\n",h);
  return 0;
}
