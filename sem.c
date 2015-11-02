/* 
 * Contributors: Jie Guo (jig26@pitt.edu)
 * This source plays semaphore. 
 *
 */
#include "thrd.h"
#include "sem.h"

int S;
int S_avail;
int S_fill;
/***********************************************************************
  release a semaphore     
 ***********************************************************************/
 
int V( int type, int s){
	  
	  thrd_t *temp = thrd_link.curr_thrd;
	  
    sigprocmask(SIG_BLOCK, &x, NULL);
    
    switch (type) { 	 
    	 case SEM_MUTEX:	  
	          if (S == 1){
              printf("no need to release S in V()!\n");
              exit(-1);	  	  
	          }
	          S++;
	          
	          if(thrd_link.sm_num > 0){
	            temp = thrd_link_op(NULL, THRD_SEM_LN, THRD_RMV_OP);
	            if(temp == NULL){
                 printf("err occurs in V()!\n");
                 exit(-1);      	
              }
              if(temp->curr_state != LWT_SEM){
                 printf("curr_state is NOT correct in V()!\n");
                 exit(-1);      	
              } 
              temp->prev_state = temp->curr_state;
              temp->curr_state = LWT_READY;
                 	
              thrd_link_op(temp, THRD_RDY_LN, THRD_ADD_OP); 	         	
	          }
	          break;
    	 case SEM_FULL:	  
	          if (S_avail == s){
              printf("no need to release S_avail in V()!\n");
              exit(-1);	  	  
	          }
            S_avail++;
	          if(thrd_link.sm_num_1 > 0){
	            temp = thrd_link_op(NULL, THRD_SEM_LN_1, THRD_RMV_OP);
	            if(temp == NULL){
                 printf("err occurs in V()!\n");
                 exit(-1);      	
              }
              if(temp->curr_state != LWT_SEM){
                 printf("curr_state is NOT correct in V()!\n");
                 exit(-1);      	
              } 
              temp->prev_state = temp->curr_state;
              temp->curr_state = LWT_READY;
                 	
              thrd_link_op(temp, THRD_RDY_LN, THRD_ADD_OP); 
                          	          	
	          }
	          break;	  
    	 case SEM_EMPTY:	  
	          if (S_fill == s){
              printf("no need to release S_fill in V()!\n");
              exit(-1); 	  
	          }
	          
	          S_fill++;
	          
	          if(thrd_link.sm_num_2 > 0){
	            temp = thrd_link_op(NULL, THRD_SEM_LN_2, THRD_RMV_OP);
	            if(temp == NULL){
                 printf("err occurs in V()!\n");
                 exit(-1);      	
              }
              if(temp->curr_state != LWT_SEM){
                 printf("curr_state is NOT correct in V()!\n");
                 exit(-1);      	
              } 
              temp->prev_state = temp->curr_state;
              temp->curr_state = LWT_READY;
                 	
              thrd_link_op(temp, THRD_RDY_LN, THRD_ADD_OP); 	          	
	          }
	          break;		                  
	  }
    sigprocmask(SIG_UNBLOCK, &x, NULL); 
    	 
	  return (1);
}
/***********************************************************************
  post a semaphore     
 ***********************************************************************/
int P(int type, int s){

	  thrd_t *temp = thrd_link.curr_thrd;
	  	  
	  repeat:
    sigprocmask(SIG_BLOCK, &x, NULL); 	 	  

    switch (type) { 	 
    	 case SEM_MUTEX:	     	 		  	
	          if (S == 1){
              S--;  	  
	          }	
	          else{                                                   //No semaphore available	          	
              temp->prev_state = temp->curr_state;
              temp->curr_state = LWT_SEM;
              thrd_link_op(temp, THRD_SEM_LN, THRD_ADD_OP);               
	          	lwt_switch(); goto repeat; 
	          }
	          break;
    	 case SEM_FULL:	     	 		  	
	          if (S_avail > 0){
              S_avail--;  	  
	          }	
	          else{                                                   //No available semaphore available
              temp->prev_state = temp->curr_state;
              temp->curr_state = LWT_SEM;
              thrd_link_op(temp, THRD_SEM_LN_1, THRD_ADD_OP);                
	          	lwt_switch(); goto repeat; 
	          }
	          break;
    	 case SEM_EMPTY:	     	 		  	
	          if (S_fill > 0){
              S_fill--;  	  
	          }	
	          else{                                                   //No filled semaphore available
              temp->prev_state = temp->curr_state;
              temp->curr_state = LWT_SEM;
              thrd_link_op(temp, THRD_SEM_LN_2, THRD_ADD_OP);                
	          	lwt_switch(); goto repeat; 
	          }
	          break;          
    }	          
    sigprocmask(SIG_UNBLOCK, &x, NULL); 
    	 	   
	  return(1); 	  
}

