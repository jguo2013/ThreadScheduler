/* 
 * Contributors: Jie Guo (jig26@pitt.edu)
 * lwt.c
 * code for light weight thread
 */
 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include "lwt.h"
#include "thrd.h"      
     
/***********************************************************************
  initialize thread_mng (thrd_link) link table   
 ***********************************************************************/
int   lwt_init(){

       int i; 
       thrd_t *temp = NULL;
       thrd_t *temp1 = NULL;  
       int err = -1;     
                              
       thrd_link.usd_num   = MAX_FD_SIZE;                                        
       thrd_link.av_num    = 0; 
       thrd_link.curr_thrd = NULL;                                          
       thrd_link.rdy_ln    = NULL;                                     
       thrd_link.sl_ln     = NULL;                                     
       thrd_link.wt_ln     = NULL;                                     
       thrd_link.sm_ln     = NULL;
       thrd_link.sm_ln_1   = NULL;
       thrd_link.sm_ln_2   = NULL;              
       thrd_link.rdy_num   = 0;                                     
       thrd_link.sl_num    = 0;                                     
       thrd_link.wt_num    = 0;                                     
       thrd_link.sm_num    = 0;
       thrd_link.sm_num_1  = 0;
       thrd_link.sm_num_2  = 0;
                     
       for(i=0;i<MAX_FD_SIZE;i++){                                      //initialize thread entries
           temp = (thrd_t *)malloc(sizeof(thrd_t));
           if(!temp) return(err);
           temp->fd = i;                                          
           temp->rt = NULL;                                         
           temp->sp_pt = NULL;                                                                                      
           temp->prev_state = LWT_NONE;
           temp->curr_state = LWT_NONE;
           temp->pthrd = NULL;                                        
           temp->cthrd_num = 0;                                       
           temp->cthrd_head = 0;
           temp->atime = 0.0; 
           temp->sltime = 0.0;                                                      
           temp->timeout_set = 0; 
           temp1 = thrd_link_op(temp, THRD_AVA_LN, THRD_ADD_OP);           
       } 
       
       if(thrd_link.av_num != MAX_FD_SIZE || thrd_link.usd_num != 0){
          printf("initialization of available link table fails in lwt_init()!\n");
          exit(-1);
       }
       temp = thrd_getnew(NULL, NULL);                       					 
       thrd_link.curr_thrd = temp; 
       thrd_link.curr_thrd->sp_pt = (unsigned char *)1;      					                     
       return(0);                                          
}
/***********************************************************************
  create light weight thread
 ***********************************************************************/
int    lwt_create(void (*f) ()){
       
       int fd;                                                      	
       thrd_t *parent = thrd_link.curr_thrd;                                                             
       thrd_t *child  = thrd_getnew(f, parent);                     	

    	 sigprocmask(SIG_BLOCK, &x, NULL);
    	  	                               
       if(!child){
          printf("Child thread cannot be found in lwt_create()!\n");
          return(-1);
       }
       if(!parent){
          printf("No parent thread is found in lwt_create()!\n");
          exit(-1);
       } 
       fd = child->fd;       
                                                                       //The parent and child thread require sp operation, turn off alarm                                                                       //cancel the the alarm to guarantee the operation integrity
       lwt_create_jmp = setjmp(parent->regs);
       if(!lwt_create_jmp){ 	  	 	                    
    	    thrd_setr(thrd_link.curr_thrd,THRD_SAVE_ALL);                //save register for current thread and put the thread in the ready queue
          thrd_setr(child,THRD_SET_ALL);
          //set stack pointer: 
          switch_sp = child->sp_pt + STACK_FD_SIZE/4 - 1;
          //set frame pointer: 
          #ifdef __x86_64__        
	        __asm__ ( "mov %0, %%rdx\n\t"
	                  "mov %%rdx, %%rsp"
 	                 : 
 	                 : "r" (switch_sp)
 	                 : "%rdx"
 	                 );         
	        __asm__  ( "mov %0, %%rcx\n\t"
	                   "mov %%rcx, %%rbp"
 	                 : 
 	                 : "r" (switch_sp)
 	                 : "%rcx","%rbp"
 	                 );
 	       #else           
	        __asm__ ( "movl %0, %%edx\n\t"
	                  "movl %%edx, %%esp"
 	                 : 
 	                 : "r" (switch_sp)
 	                 : "%edx"
 	                 );         
	        __asm__  ( "movl %0, %%ecx\n\t"
	                   "movl %%ecx, %%ebp"
 	                 : 
 	                 : "r" (switch_sp)
 	                 : "%ecx","%ebp"
 	                 );	                 
 	        #endif
 	         	                 
          sigemptyset(&x);
          sigaddset(&x,SIGALRM);	
          sigprocmask(SIG_UNBLOCK, &x, NULL);
           	                  	                     	                 	  	 	                                                    
          thrd_link.curr_thrd->rt();     	
       }
       else{
            sigprocmask(SIG_UNBLOCK, &x, NULL); 	 
       }
       return (fd);
}

/***********************************************************************
  context switching for light weight thread
 ***********************************************************************/

void   lwt_switch(){
    
    int flag;
    thrd_t * temp = NULL;
    int prev_fd = thrd_link.curr_thrd->fd;
       
    sigprocmask(SIG_BLOCK, &x, NULL); 	       
      	 	              
    thrd_awk();                                                    			//check sleep thread
    
    lwt_switch_jmp = setjmp((thrd_link.curr_thrd)->regs); 
                            
    if(!lwt_switch_jmp){

    	 if(thrd_link.curr_thrd->sp_pt != NULL){                       		//in case of performing lwt_switch for lwt_exit
    	    thrd_setr(thrd_link.curr_thrd,THRD_SAVE_ALL);              		//save register for current thread and put the thread in the ready queue
       }
       if(thrd_link.rdy_num != 0){
          temp = thrd_getrdy();                                      		//select a ready thread to continue, removed from the rdy link
          if(!temp){                                                    //putting main() in the loop to keep their is always a ready thread
              printf("getting a ready thread fails in lwt_switch1()!\n");
              exit(-1);
          }
       }
       else if(thrd_link.sl_num != 0){  
       	  while(!thrd_link.rdy_num){thrd_awk();}
       	  temp = thrd_getrdy();
          if(!temp){                                                    
              printf("getting a ready thread fails in lwt_switch2()!\n");
              exit(-1);
          }       	  
       }
    
       thrd_setr(temp,THRD_SET_ALL);                                 //set current register    
	                                                                 	       	     	 
       if(prev_fd != thrd_link.curr_thrd->fd)  
         longjmp(thrd_link.curr_thrd->regs,2);                       //set stack pointer for the children thread & set it current thread
       sigemptyset(&x);
       sigaddset(&x,SIGALRM);	
       sigprocmask(SIG_UNBLOCK, &x, NULL); 
    }
    else{
       sigemptyset(&x);
       sigaddset(&x,SIGALRM);	
       sigprocmask(SIG_UNBLOCK, &x, NULL);     	
    }  
}

/***********************************************************************
  show time
 ***********************************************************************/
void lwt_stime (int thrd_no, int flag) {

  double curr_time;
  struct timeval tv;
  struct timezone tz;
      
  gettimeofday (&tv ,   &tz);
  curr_time = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0; //ms
  if(flag == MAX_FD_SIZE+1)
  	printf("lwt_switch() handles thread %d: at time %lf ms\n", thrd_no, curr_time);
  else
    printf("thread %d: runs....at time %lf\n", thrd_no, curr_time);
} 

/***********************************************************************
  exit light weight thread
 ***********************************************************************/
int lwt_exit(){
	
	int curr_state;
	int err = 1;

  sigemptyset(&x);
  sigaddset(&x,SIGALRM);	
  sigprocmask(SIG_BLOCK, &x, NULL); //guarantee atomity	 
                    
  repeat:  		           	
         curr_state = thrd_rmv();
          
         switch(curr_state){
                case LWT_WAIT:  
                               lwt_switch(); goto repeat;
                case LWT_NONE: 
                	             if(thrd_link.curr_thrd->fd == 0) break;  
                	             else
                	             	  lwt_switch();                               
                default: 
                     printf("cannot identify current state in lwt_exit()!\n");
                     exit(-1);   	
         };

  return(err);
}

/***********************************************************************
  put thread in sleep state
 ***********************************************************************/
int lwt_sleep(int fd, double sltime){ //sltime:ms
    
    int err = 1;
    sigemptyset(&x);
    sigaddset(&x,SIGALRM);	    
    sigprocmask(SIG_BLOCK, &x, NULL); //guarantee atomity	
    
    err = thrd_sleep(fd,sltime);    
    if(fd == (thrd_link.curr_thrd)->fd)
    	lwt_switch();
   	
    sigprocmask(SIG_UNBLOCK, &x, NULL); 	                                                                 	       	     	 
    	
    return (err);
}
