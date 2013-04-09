#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <gmp.h>
#include <pvm3.h>
#define DIE 1
#define DATA 2
#define PER_HOST_TASK_COUNT 20

void clearscreen()
{
  rewind(stdout);
  ftruncate(1,0);
  int ct=0;
  for (ct=0;ct<20;ct++)
    fputs("\33[A\33[2K",stdout);
//    printf("                                                                                                                                                 \n");
  rewind(stdout);
  ftruncate(1,0);
}

int main(void)
{
  
  // Counter whenever you need it
  int ct=0;

  /* Block user interrupts while doing other processing. */
  sigset_t block,pending;
  sigemptyset (&block);
  sigaddset (&block, SIGINT);
  sigaddset (&block, SIGTSTP);
  sigprocmask (SIG_SETMASK, &block, NULL);
  
  pvm_initsend(PvmDataDefault);

  // Spawn the pvm tasks
  // Hosts in PVM
  int mytid=pvm_mytid();
  int host_count;
  pvm_config(&host_count,NULL,NULL);
  int task_count=host_count*PER_HOST_TASK_COUNT;
  // For the statistics
  unsigned int statistics[task_count+1];
  for (ct=0;ct<task_count;ct++)
    statistics[ct]=0;
  // Task-IDs
  int tids[task_count];
  int r=pvm_spawn((char *)"montecarlo_slave",NULL,PvmTaskDefault,NULL,task_count,tids);
  if (r<task_count)
  {
    for(ct=0;ct<task_count;ct++)
      printf("Spawn error %d\n",tids[ct]);
  }
                                               
  // Init the arbitrary numbers  
  mpf_t hit;
  mpf_t nhit;
  mpf_t total;
  mpf_t ntotal;
  mpf_t pi_quart;
  mpf_t pi;
  mpf_init(hit);
  mpf_init(nhit);
  mpf_init(total);
  mpf_init(ntotal);
  mpf_init(pi_quart);
  mpf_init(pi);

  while(1)
  {    
    unsigned int thit=0;   
    unsigned int ttotal=0;
    int tid=0;
    // Get data
    int recbuf=pvm_recv(-1,DATA);
#ifdef DEBUG
    if (recbuf==0)
      printf ("Feuerfeuerfeuer\n");
    int bsize=0;
    pvm_bufinfo(recbuf,&bsize,NULL,NULL);
    printf("Buffer size %d\n",bsize);
#endif
    pvm_upkint(&tid,1,1);
    pvm_upkuint(&thit,1,1);
    pvm_upkuint(&ttotal,1,1);
    statistics[tid-mytid-1]++;
#ifdef DEBUG
    pvm_bufinfo(recbuf,&bsize,NULL,NULL);
    printf("Buffer size %d\n",bsize);
#endif

    // Do the computation stuff
    mpf_add_ui(nhit,hit,thit);
    mpf_add_ui(ntotal,total,ttotal);
    mpf_set(hit,nhit);
    mpf_set(total,ntotal);
    mpf_div(pi_quart,hit,total);
    mpf_mul_ui(pi,pi_quart,4);
    // Clear screen and print current results
    clearscreen();
    printf("Current data: %d hits of %d tries from host %d\n",thit, ttotal,tid-mytid-1);
    gmp_printf("Total tries: %.*Ff\n",0,total);
    gmp_printf("Total hits: %.*Ff\n",0,hit);
    gmp_printf("Approximation to Pi: %.*Ff\n\n",20,pi);
    for (ct=0;ct<task_count;ct++)     
	printf("Host %d: %d tasks finished\n",ct,statistics[ct]);
//    printf("\n\n\n\n\n");
    // Check for Sigint
    sigpending(&pending);
    if (sigismember(&pending,SIGINT))
      break;
  }
  printf("Closing down\n");
  // Kill all tasks
  for (ct=0;ct<task_count;ct++)
    pvm_kill(tids[ct]);
}
