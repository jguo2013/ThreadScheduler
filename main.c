#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include "test.h"
#include "lwt.h"
#include "sem.h"

/***********************************************************************
  main function starts!   
 ***********************************************************************/
 
 int main (int argc, char **argv)
{
	int err;
	int fd;
	int i;	
  	
  act.sa_handler = lwt_switch;                   		//use lwt_switch as handler
  sigaction (SIGALRM, & act, 0);

  err = lwt_init();                              		//initialize the thrd_link data structure 
  if(err == -1){
     printf("lwt_init() fails!\n");
     exit(-1);
  }
  
  S = 1; 
  S_avail = PROD_BUF_SIZE;                       		//initialize the available semaphore
  S_fill  = 0;                                   		//initialize the filled semaphore       	  
  memset ( test_buf, TEST_EMPTY, PROD_BUF_SIZE); 		//initialize producer& consumer buffer
  
  ualarm (lwt_quantum, lwt_quantum);             
 
  lwt_stime(thrd_link.curr_thrd->fd,0);

  for(i=0; i<5;i++){ 
     fd = lwt_create(test_pro_plain);                //create n threads for producer
     if ( fd  == -1){                               
         printf("thread creation fails!\n");
         exit(-1);
     }
     else{
     	  printf("Successufully create thread %d!\n",fd); 
     }
  }

  for(i=1; i<3;i++){  
   err = lwt_sleep(i,100);     
  } 

  for(i=0; i<5;i++){    
    fd = lwt_create(test_con_plain);
    if ( fd  == -1){                               //create n threads for consumer
        printf("thread creation fails!\n");
        exit(-1);
    }
    else{
    	  printf("Successufully create thread %d!\n",fd); 
    }
  }
   
  lwt_exit();

  printf("main process successfully exits!\n");  
  return 0;
}


