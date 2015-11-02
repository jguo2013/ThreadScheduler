/* 
 * Contributors: Jie Guo (jig26@pitt.edu)
 * thrd.c
 * code for thread link table manipulation
 */
 
#include "thrd.h"
#include <unistd.h>
#include <sys/time.h>

/***********************************************************************
  Get a new thread info from the afd link table    
 ***********************************************************************/
thrd_t* thrd_getnew(void (*f_pt) (), thrd_t *parent){
   
   thrd_t* temp   = NULL;
   cthrd_t*   ctemp  = NULL;
      
   if(!thrd_link.av_num && !thrd_link.afd_head)  {							//if no available thread
      return(temp);
   }
   
   temp = thrd_link_op(NULL,THRD_AVA_LN, THRD_RMV_OP); 
   
   if(temp == NULL){ 
      printf("Error occurs in thrd_getnew()!\n");
      exit(-1);
   }
   
   if(temp->prev_state != LWT_NONE || temp->curr_state != LWT_NONE) {		//update the thread info
      printf("prev_state is NOT correct in thrd_getnew()!\n");
      exit(-1);  
   } 
   temp->rt = f_pt;                                                
   temp->curr_state = LWT_READY;                                    
   if(f_pt != NULL){
     temp->sp_pt = (unsigned char*)malloc(STACK_FD_SIZE);          			//allocate stack space for thread not including main()
   }
   if(f_pt == NULL){                                                
     temp->pthrd = NULL;}
   else{
     temp->pthrd = parent;}        	
   temp->cthrd_num = 0;
   temp->cthrd_head = NULL;
   temp->next = NULL; 
   temp->prev = NULL;  
   
   if(f_pt != NULL){																										//update the parent link	                                                		 

       ctemp = (cthrd_t*)malloc(sizeof(cthrd_t));                   		
       ctemp->cthrd_add = (unsigned int*)temp;                      		   
       ctemp->prev = NULL;
       ctemp->next = NULL;   
       thrd_link_op((thrd_t*)ctemp,THRD_CTH_LN, THRD_ADD_OP);       		
       
   }
   return(temp);       	 
}

/***********************************************************************
  search the sleep thread out of all used link table (rdy, sleep, sem & curr)    
 ***********************************************************************/
thrd_t* thrd_gets(int curr_fd){
   
   thrd_t* temp  = NULL;
   
   if(!thrd_link.usd_num)  {
      printf("no used thread in thrd_getfd()!\n");
      exit(-1);
   }
   
   //search for ready link table
   temp = _sch_link(&(thrd_link.rdy_ln),THRD_SCH_FD,curr_fd,&(thrd_link.rdy_num));
   if(temp != NULL) return(temp);

   //search for sleep link table
   temp = thrd_link.sl_ln;
   while(temp){
   	 if(temp->fd == curr_fd){
      return(NULL);
     }
     temp = temp->next;
   }    	
   
   //search for thread wait link table
   temp = _sch_link(&(thrd_link.wt_ln),THRD_SCH_FD,curr_fd,&(thrd_link.wt_num));   
   if(temp != NULL) return(temp);

   //search for sem link table
   temp = _sch_link(&(thrd_link.sm_ln),THRD_SCH_FD,curr_fd,&(thrd_link.sm_num));   
   if(temp != NULL) return(temp);  

   //search for sem_1 link table
   temp = _sch_link(&(thrd_link.sm_ln_1),THRD_SCH_FD,curr_fd,&(thrd_link.sm_num_1));   
   if(temp != NULL) return(temp);  

   //search for sem_2 link table
   temp = _sch_link(&(thrd_link.sm_ln_2),THRD_SCH_FD,curr_fd,&(thrd_link.sm_num_2));   
   if(temp != NULL) return(temp);  
   	   	   
   if((thrd_link.curr_thrd)->fd == curr_fd)
   	 temp = thrd_link.curr_thrd;
      
   return(temp);       	 
}

/***********************************************************************
  get the first entry in the rdy link table    
 ***********************************************************************/
thrd_t* thrd_getrdy(){
   
   thrd_t* temp  = NULL;
   
   if(!thrd_link.usd_num)  {
      printf("no used thread in thrd_getrdy()!\n");
      exit(-1);
   }
   
   temp = thrd_link_op(NULL,THRD_RDY_LN,THRD_RMV_OP);
   return(temp);       	 
}

/***********************************************************************
  Remove the current thread out of the rdy table
 ***********************************************************************/
int  thrd_rmv(){                            
   
   thrd_t* temp  = thrd_link.curr_thrd; 
   thrd_t* temp1; unsigned char *sp_temp; 
   int err = 1;
     
   if((thrd_link.curr_thrd)->curr_state != LWT_READY)  {
      printf("curr_state is NOT correct in thrd_rmv()!\n");
      exit(-1);
   }
   
   //check the children thread
   if(temp->cthrd_num != 0 && temp->cthrd_head == NULL){
      printf("cthrd_num and cthrd_head are not consistent in thrd_rmv()!\n");
      exit(-1); 
   }
   
   if(temp->cthrd_num == 0 && temp->cthrd_head != NULL){
      printf("cthrd_num and cthrd_head are NOT consistent in thrd_rmv()!\n");
      exit(-1); 
   }     
	   
   //if still have children thread, return after state change
   if(temp->cthrd_num != 0){
      err = thrd_wait(temp);
      if(err == -1){
         printf("error occurs in thrd_rmv()!\n");
         exit(-1);      	
      }
      return(temp->curr_state);
   }

  //check parent thread, if there is any thread waiting for current thread, move it to the ready line.
  if(temp->pthrd != NULL){
	   temp1 = thrd_rmvp(temp); 

	   if(temp1 == NULL || temp1 != temp) {
		   printf("error of thrd_rmvp() occurs in thrd_rmv()!\n");
       exit(-1);   
	   }
  }  
                                        
  //clean all entries in data structure                                              
   temp->rt = NULL;                                                                                                                                     
   temp->prev_state = LWT_NONE;
   temp->curr_state = LWT_NONE;
   temp->pthrd = NULL;                                        
   temp->cthrd_num = 0;                                                                             
   temp->cthrd_head = NULL;                                     
   temp->next = NULL;
   temp->prev = NULL;  
   temp->atime = 0;                                           
   temp->sltime = 0;                                          
   temp->timeout_set = 0; 
 
   if(temp->fd != 0){
   	  if(!temp->sp_pt){
		   printf("temp->sp_pt is pointed to zero in thrd_rmv()!\n");
       exit(-1);   	  	
   	  }  
   	  sp_temp = temp->sp_pt;
   }
   temp->sp_pt = NULL;
   thrd_link_op(temp,THRD_AVA_LN,THRD_ADD_OP);
    
   return(temp->curr_state);       	 
}
/***********************************************************************
  link operation 
  ***********************************************************************/
thrd_t* thrd_link_op(thrd_t *thrd,int type, int op){
	
	thrd_t* temp = NULL;
			
	switch(type){
    case THRD_AVA_LN:   	              	
         if(op == THRD_RMV_OP){
            temp = _remove_link(&(thrd_link.afd_head),NULL,&(thrd_link.av_num));
            if(!temp){
              printf("no available thread in THRD_AVA_LN thrd_link_op()!\n");
              exit(-1);            	
            }
            (thrd_link.usd_num)++; 
            if(thrd_link.usd_num > MAX_FD_SIZE){
               printf("thrd_link.usd_num is out of bound in thrd_link_op()!\n");
               exit(-1);   	
            }             
         }
         else{
         	   _add_link(&(thrd_link.afd_head),thrd,&(thrd_link.av_num));
         	   if(thrd_link.usd_num == 0){
              printf("thrd_link.usd_num is not correct in thrd_link_op()!\n");
              exit(-1);            	
            }         	   	
         	  (thrd_link.usd_num)--;
         }
         break;
    case THRD_RDY_LN:
         if(op == THRD_RMV_OP){
            temp = _remove_link(&(thrd_link.rdy_ln),NULL,&(thrd_link.rdy_num));    	  
            if(!temp){
              printf("no available thread in THRD_RDY_LN thrd_link_op()!\n");
              exit(-1);            	
            }            
         }
         else{
         	   _add_link(&(thrd_link.rdy_ln),thrd,&(thrd_link.rdy_num));         	   
         }
         break;         
    case THRD_WAIT_LN:
         if(op == THRD_RMV_OP){
            temp = _remove_link(&(thrd_link.wt_ln),NULL,&(thrd_link.wt_num));    	  
            if(!temp){
              printf("no available thread in THRD_WAIT_LN thrd_link_op()!\n");
              exit(-1);            	
            }            
         }
         else{
         	   _add_link(&(thrd_link.wt_ln),thrd,&(thrd_link.wt_num));
         } 
         break;   
    case THRD_SLP_LN: 
         if(op == THRD_RMV_OP){
            temp = _remove_link(&(thrd_link.sl_ln),NULL,&(thrd_link.sl_num));    	  
            if(!temp){
              printf("no available thread in THRD_SLP_LN thrd_link_op()!\n");
              exit(-1);             	
            }            
         }
         else{
         	    _add_link(&(thrd_link.sl_ln),thrd,&(thrd_link.sl_num));
         } 
         break;    	
    case THRD_SEM_LN:	
         if(op == THRD_RMV_OP){
            temp = _remove_link(&(thrd_link.sm_ln),NULL,&(thrd_link.sm_num));    	  
            if(!temp){
              printf("no available thread in THRD_SEM_LN thrd_link_op()!\n");
              exit(-1);            	
            }            
         }
         else{
         	    _add_link(&(thrd_link.sm_ln),thrd,&(thrd_link.sm_num));
         } 
         break; 
    case THRD_SEM_LN_1:	
         if(op == THRD_RMV_OP){
            temp = _remove_link(&(thrd_link.sm_ln_1),NULL,&(thrd_link.sm_num_1));    	  
            if(!temp){
              printf("no available thread in THRD_SEM_LN_2 thrd_link_op()!\n");
              exit(-1);            	
            }            
         }
         else{
         	    _add_link(&(thrd_link.sm_ln_1),thrd,&(thrd_link.sm_num_1));
         } 
         break; 
    case THRD_SEM_LN_2:	
         if(op == THRD_RMV_OP){
            temp = _remove_link(&(thrd_link.sm_ln_2),NULL,&(thrd_link.sm_num_2));    	  
            if(!temp){
              printf("no available thread in THRD_SEM_LN_2 thrd_link_op()!\n");
              exit(-1);            	
            }            
         }
         else{
         	    _add_link(&(thrd_link.sm_ln_2),thrd,&(thrd_link.sm_num_2));
         } 
         break;  
    case THRD_CTH_LN:	
         if(op == THRD_RMV_OP){
            temp = (cthrd_t *)_remove_link_(&(thrd_link.curr_thrd->cthrd_head),NULL,&(thrd_link.curr_thrd->cthrd_num));    	  
            if(!temp){
              printf("no available thread in THRD_CTH_LN thrd_link_op()!\n");
              exit(-1);            	
            }            
         }
         else{
         	    _add_link_(&(thrd_link.curr_thrd->cthrd_head),(cthrd_t *)thrd,&(thrd_link.curr_thrd->cthrd_num));
         } 
         break;            	
  };
	  	
  return(temp); 	
}
/***********************************************************************
  remove an entry from link table for (thrd_t type)
  ***********************************************************************/
thrd_t* _remove_link(thrd_t **head,thrd_t *entry, unsigned int* num){
         
    thrd_t* temp = NULL;
    
    if(*head == NULL && *num != 0 ||
       *head != NULL && *num == 0)  {
       printf("head and num are inconsistent in _remove_link()!\n");
       exit(-1);
    } 
             
    temp = *head;  
    if(temp == NULL){ 
    	 return(temp);
    }
    
    *head = temp->next;
     if((*head)!= NULL){
       (*head)->prev = NULL;
     }
    temp->prev = NULL;
    temp->next = NULL;    
    (*num)--; 
    return(temp);
}

/***********************************************************************
  add an entry to link table for (thrd_t type)
  ***********************************************************************/
thrd_t* _add_link(thrd_t **head,thrd_t *entry, unsigned int* num){
    
    thrd_t* temp = NULL;
    
    if(*head == NULL && *num != 0 ||
       *head != NULL && *num == 0)  {
       printf("head and num are inconsistent in _add_link()!\n");
       exit(-1);
    } 
    
    if(*head == NULL) {
       *head = entry;
       temp = entry;
    } 
    else {                                                       //search for the link tail, always add the last
       temp = *head;
       while(temp->next){
    	    temp = temp->next;                                     //in case of child thread link table
       }       
    	 temp->next = entry;
    	 entry->prev = temp;
    }   
    
    (*num)++;     
    return(entry);
}
/***********************************************************************
  remove an entry from link table for (cthrd_t type)
  ***********************************************************************/
cthrd_t* _remove_link_(cthrd_t** head,cthrd_t* entry, unsigned int* num){
         
    cthrd_t* temp = NULL;
    
    if(*head == NULL && *num != 0 ||
       *head != NULL && *num == 0)  {
       printf("head and num are inconsistent in _remove_link()!\n");
       exit(-1);
    } 
             
    temp = *head;  
    if(temp == NULL){ 
    	 return(temp);
    }
    
    *head = temp->next;
    (*head)->prev = NULL;
    temp->prev = NULL;
    temp->next = NULL;    
    (*num)--; 
    
    return(temp);
}

/***********************************************************************
  add an entry to link table for (cthrd_t type)
  ***********************************************************************/
cthrd_t* _add_link_(cthrd_t** head, cthrd_t* entry, unsigned int* num){
    
    cthrd_t* temp = NULL;
    
    if(*head == NULL && *num != 0 ||
       *head != NULL && *num == 0)  {
       printf("head and num are inconsistent in _add_link_()!\n");
       exit(-1);
    } 
    
    if(*head == NULL) {
       *head = entry;
       temp = entry;
    } 
    else {                                                       //search for the link tail, always add the last
       temp = *head;
       while(temp->next){
    	    temp = temp->next;                                     //in case of child thread link table
       }       
    	 temp->next = entry;
    	 entry->prev = temp;
    }   
    
    (*num)++; 
    
    return(temp);
}
/***********************************************************************
  set & save registers 
  ***********************************************************************/
int thrd_setr (thrd_t *thrd, int type){ 												//set curr_thrd, set stack pt or all CPU registers
	  
	  unsigned char *sp_temp = thrd->sp_pt;
	  sp_temp = sp_temp + STACK_FD_SIZE - 1; 											//linux stack grow downwards for setr
	  switch(type){       
	  	 case THRD_SET_ALL:
	  	 	    thrd_link.curr_thrd = thrd;	  	 		   
	  	 break;
	  	 case THRD_SAVE_ALL:
	  	 	    if(thrd->curr_state == LWT_READY){
	  	 	    	thrd_link_op(thrd,THRD_RDY_LN, THRD_ADD_OP);
	  	 	    }
	  	 	    else if(thrd->curr_state == LWT_WAIT){         
	  	 	    	thrd_link_op(thrd,THRD_WAIT_LN, THRD_ADD_OP);
	  	 	    }
	  	 break;
	  }
	  return(1);
}
/***********************************************************************
  switch to thread wait 
  ***********************************************************************/
int  thrd_wait (thrd_t* parent){
	   
	   parent->prev_state = parent->curr_state;	   
	   parent->curr_state = LWT_WAIT;
	   
	   return(1);
}

/***********************************************************************
  check parent thread, if there is any thread waiting for current thread, move it to the ready line.
  if not in thread wait link table, remove the children
  ***********************************************************************/
thrd_t * thrd_rmvp(thrd_t * curr_thrd){
	                                      
	               
	  thrd_t* temp = curr_thrd->pthrd;
	  cthrd_t* temp1 = NULL;
	  thrd_t* temp2 = NULL;
	  int temp_state ; 
	  
	  if(temp->cthrd_head == NULL && temp->cthrd_num != 0 ||
	  	 temp->cthrd_head != NULL && temp->cthrd_num == 0){
       printf("cthrd is not the same as cthrd_num in thrd_rmvp()!\n");
       exit(-1); 
	  }
	  
	  if(temp->cthrd_num != 0){
	  	temp1 = _sch_link_(&(temp->cthrd_head), THRD_CTHS_OP, (unsigned int*)thrd_link.curr_thrd, &(temp->cthrd_num));

      if(temp1 != NULL){     
	    		 free(temp1);
	    		 if(temp->cthrd_num == 0 && temp->curr_state == LWT_WAIT){
              thrd_t* temp3 = _sch_link(&(thrd_link.wt_ln), 0, temp->fd, &(thrd_link.wt_num));	
              if(temp3 != temp){
              	printf("the fetched thread %d is not the same with the original thread %d in thrd_rmvp()!\n", temp3->fd, temp->fd);
              	exit(-1);
              }    		 	  
	    		 	  if(temp->prev_state != LWT_READY){
                printf("previous state err occurs in thrd_rmvp()!\n");
                exit(-1); 
              }	    		 	  	
	    		 	  temp->prev_state = temp->curr_state;
	    		 	  temp->curr_state = LWT_READY;
	    		 	  thrd_link_op(temp,THRD_RDY_LN, THRD_ADD_OP);
	    		 }
	    		 temp2 = thrd_link.curr_thrd;
	    }
	  }
	  return(temp2);
}

/***********************************************************************
  switch to thread wait
  awake sleep thread,check the sleep link table,check current time, check the time difference 
  ***********************************************************************/
int thrd_awk(){  
    
	  thrd_t* temp = NULL;
    struct timeval tv;
    struct timezone tz;
    double curr_time;
    int    i, sl_temp = thrd_link.sl_num;
    
    if(thrd_link.sl_num == 0){
      return(0);
    }
    
    for (i=0; i<sl_temp;i++){
       temp = thrd_link_op(NULL,THRD_SLP_LN, THRD_RMV_OP);
       if(temp == NULL){
          printf("I cannot find out the fd in any link table in thrd_awk()!\n");
          exit(-1);    	                 
       } 

       gettimeofday (&tv ,   &tz);
       curr_time = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;
       
       if(curr_time - temp->atime < temp->sltime/1000){
       		thrd_link_op(temp,THRD_SLP_LN, THRD_ADD_OP);
         	return(0);
       }
       
       if(temp->prev_state == LWT_READY) {
       		temp->prev_state = LWT_READY;
       		temp->curr_state = LWT_READY;    	
         	thrd_link_op(temp,THRD_RDY_LN, THRD_ADD_OP);    	   	 
       }
       else if(temp->prev_state == LWT_SEM) {
       		temp->curr_state = LWT_READY;    	
         	thrd_link_op(temp,THRD_RDY_LN, THRD_ADD_OP);   	
       }
    }
    return(1);
}

/***********************************************************************
  switch to sleep 
  ***********************************************************************/
int thrd_sleep(int fd, double sltime){
	  
	  int err = 1;
	  thrd_t * temp;
    struct timeval tv;
    struct timezone tz;

    temp = thrd_gets(fd); 
    if(!temp){   
        printf("cannot find thread %d in lwt_sleep()!\n",fd);
        return(-1);    	
    }     	  
	  temp->prev_state = LWT_READY; 
	  temp->curr_state = LWT_SLEEP;
    gettimeofday (&tv ,   &tz);
    temp->atime = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0; 
    temp->sltime = sltime;	

    thrd_link_op(temp,THRD_SLP_LN, THRD_ADD_OP);
    return(err);    
}
/***********************************************************************
  search for  an entry to link table for (cthrd_t type)
  ***********************************************************************/
cthrd_t* _sch_link_(cthrd_t** head, int type, unsigned int *value, unsigned int* num){
    
    cthrd_t* temp = NULL;
    cthrd_t* temp1 = NULL;
    int found = 0;
        
	  temp1 = *head;
	    while(temp1){
	    	if(temp1->cthrd_add == value){     //child is the same as the current thread
	    		if(found == 0) {
	    			 found = 1;
	    			 temp = temp1;
	    		}
	    		else {
             printf("two items are found in _sch_link_()!\n");
             exit(-1); 	    			
	    	  }
	    	}
	    	temp1 = temp1->next;
	    }
      
      if(temp != NULL){
	    	 if(temp->next != NULL){
	    	 	 temp->next->prev = temp->prev;
	    	 }
	    	 if(temp->prev != NULL){
	    	 	  temp->prev->next = temp->next;	    		 	  
	    	 }
	    	 if(*head == temp)
	    	    *head = temp->next; 
	    	 (*num)--; 
         temp->prev = NULL;
         temp->next = NULL;	    	    	
      }
      
    return(temp);
}

/***********************************************************************
  search for  an entry to link table for (cthrd_t type)
  ***********************************************************************/
thrd_t* _sch_link(thrd_t** head, int type, int value, unsigned int* num){
    
    thrd_t* temp = NULL;
    thrd_t* temp1 = NULL;
    int found = 0;     
   
	  temp1 = *head;
	    while(temp1){
	    	if(temp1->fd == value){     //child is the same as the current thread
	    		if(found == 0) {
	    			 found = 1;
	    			 temp = temp1;
	    			 }
	    		else {
             printf("two items are found in _sch_link_()!\n");
             exit(-1); 	    			
	    	  }
	    	}
	    	temp1 = temp1->next;
	    }
	  if(temp != NULL){
	  	
	  	  if(temp->prev) 
   	 	  	 temp->prev->next = temp->next;
        	
   	 	  if(temp->next) 
   	 	  	 temp->next->prev = temp->prev;
   	 	  	 
        if(*head == temp) *head = temp->next;
         temp->prev = NULL;
         temp->next = NULL;	
                 	
        (*num)--;
	  } 
    return(temp);
}
