/* 
 * Contributors: Jie Guo (jig26@pitt.edu)
 * test.c
 * code for various consumers and producers for test purpose
 */
 
#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "lwt.h"
#include "test.h"
#include "sem.h"
                                          
unsigned int sto_pt = 0;                                           

/***********************************************************************
  PLAIN producer: no sleep, no sem wait; 
  Test purpose: thrd_wait; thrd_getnew, thrd_exit; lwt_exit; 
                lwt_switch; lwt_init(); lwt_create();
 ***********************************************************************/
 void test_pro_plain(){

 	   unsigned i = 0;
 	   int j = 0;
 	     
 	   while(i< THRESHOLD){
 	      P(SEM_FULL,PROD_BUF_SIZE); 	   	
 	      P(SEM_MUTEX,0);      
 	      if(test_buf[sto_pt] != TEST_EMPTY){
           printf("buf state is not correct in test_pro_plain()!\n");
           exit(-1); 	   	  
 	      } 	
                   
 	      test_buf[sto_pt] = TEST_FULL;
 	      sto_pt++;
 	      V(SEM_MUTEX,0); 
 	      V(SEM_EMPTY,PROD_BUF_SIZE);  	        
 	      i++;
 	   }
 	    	   	 	   
 	   printf("Producer (thread) %d finishes!\n",thrd_link.curr_thrd->fd,sto_pt-1);
 	   lwt_exit();
 }
 
/***********************************************************************
  PLAIN producer: no sleep, no sem wait; 
  Test purpose: thrd_wait; thrd_getnew, thrd_exit; lwt_exit; 
                lwt_switch; lwt_init(); lwt_create();
 ***********************************************************************/
 void test_con_plain(){
   
 	   unsigned i = 0;

 	   while(i< THRESHOLD){ 
 	      P(SEM_EMPTY,PROD_BUF_SIZE); 	   	
 	      P(SEM_MUTEX,0); 	           
        if(test_buf[sto_pt-1] != TEST_FULL){
           printf("buf state is not correct in test_con_plain()!\n");
           exit(-1); 	   	  
        } 	      	                    
 	      test_buf[sto_pt-1] = TEST_EMPTY;
 	      sto_pt--;
 	      V(SEM_MUTEX,0); 
 	      V(SEM_FULL,PROD_BUF_SIZE); 	      
 	      i++;
 	   }
 	    	   	 
 	   printf("Consumer (thread) %d finishes!\n",thrd_link.curr_thrd->fd,sto_pt-1);	   
 	   lwt_exit();
 }
