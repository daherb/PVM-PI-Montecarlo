#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <pvm3.h>

#define DIE 1
#define DATA 2

int main(void)
{
  int master=pvm_parent();
  int ct;
  int tid=pvm_mytid();
  srand48(time(NULL));
//  pvm_joingroup("slave");
#ifdef DEBUG
  FILE *f=fopen("/tmp/slave.log","w");
  fprintf(f,"Tid %d Master %d\n",tid,master);
#endif
  while(!pvm_probe(-1,DIE))
  {
    unsigned int total=0;
    unsigned int hit=0;
    int ct=0;
    for (ct=0;ct<10000;ct++)
    {
      double x=drand48();
      double y=drand48();
      double dist=x*x+y*y;

      if (dist<=1.0)
        hit++;
      total++;
#ifdef DEBUG
      fprintf(f,"x %f y %f dist %f\n",x,y,dist);
      fprintf(f,"%d hits of %d total\n",hit,total);
      fflush(f);
#endif
    }
    pvm_initsend(PvmDataDefault);
    pvm_pkint(&tid,1,1);
    pvm_pkuint(&hit,1,1);
    pvm_pkuint(&total,1,1);
    int sbuf=pvm_send(master,DATA);
#ifdef DEBUG
    fprintf(f,"%d hits of %d total\n",hit,total);
    fflush(f);
#endif
  }
#ifdef DEBUG
  fclose(f);
#endif
  pvm_recv(-1, DIE);
  pvm_exit();
  return 0;
}
