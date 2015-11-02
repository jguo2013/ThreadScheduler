#ifndef LWT_H
#define LWT_H

#include <setjmp.h>
#include <signal.h>
#include <unistd.h>

#define lwt_quantum   10000                                        //time slot for each thread
#define MAX_FD_SIZE   128                                          //the max file descriptor number
#define STACK_FD_SIZE 16384
#define MAIN_FD       0

#define LWT_NONE      0
#define LWT_READY     1
#define LWT_WAIT      2
#define LWT_SLEEP     3
#define LWT_EXIT      4
#define LWT_SEM       5
#define LWT_PRDY      6

#define LWT_PRT_MAIN  0                                            //main function
#define LWT_PRT_SIG   1                                            //alarm time up
#define LWT_PRT_TW    2                                            //alarm thread wait
#define LWT_PRT_SEM   3                                            //semaphore wait

#define LWT_START 0
#define LWT_RES   1 

typedef struct cthrd_entry {
	      unsigned int* cthrd_add;                                    //avoid deadlock
	      struct   cthrd_entry *prev;
	      struct   cthrd_entry *next;	      
}cthrd_t;
	
typedef struct thread_info {
   int             fd;                                              //file descriptor
   void            (*rt) ();                                        //function address
   unsigned char*  sp_pt;                                           //stack pointer
   jmp_buf         regs;                                            //register buffer for context switch
   int             prev_state;
   int             curr_state;
   struct thread_info *pthrd;                                      	//parent thread pointer
   int             cthrd_num;                                       //childern thread number
   cthrd_t         *cthrd_head;                                     //childern thread header  
   struct thread_info *next;
   struct thread_info *prev;  
   double          atime;                                           //last access time for sleep and thread wait
   double          sltime;                                          //required sleep time
   int             timeout_set;                                     //reserved: prevent no wait thread return
} thrd_t;

typedef struct thread_management {
   thrd_t        *afd_head;                                         //available fd link 
   unsigned int   usd_num;                                          //used fd number    
   unsigned int   av_num;                                           //available fd number
   thrd_t        *rdy_ln;                                           //ready link
   unsigned int   rdy_num;                                          //ready link number   
   thrd_t        *sl_ln;                                            //sleep link
   unsigned int   sl_num;                                           //sleep link number   
   thrd_t        *wt_ln;                                            //wait thread link
   unsigned int   wt_num;                                           //wait thread link number   
   thrd_t        *sm_ln;                                            //sem wait link
   unsigned int   sm_num;                                           //sem wait link number  
   thrd_t        *sm_ln_1;                                          //sem wait link
   unsigned int   sm_num_1;                                         //sem wait link number 
   thrd_t        *sm_ln_2;                                          //sem wait link
   unsigned int   sm_num_2;                                         //sem wait link number     
   thrd_t        *curr_thrd;                                        //current thread
   int           curr_thrd_timeup;                                  //current thread time up   
} thrd_mng_t; 

thrd_mng_t thrd_link;
static int lwt_create_jmp;
static int lwt_switch_jmp;

jmp_buf env; 
static sigset_t x;
static unsigned char * switch_sp;
struct sigaction act; 

int    lwt_init();                                                   //initialize the link table for thrd_mng
int    lwt_create(void (*f) ());                                       
void   lwt_switch();                                                 //switch thread and perform context switch
void   lwt_stime (int thrd_no, int flag);    
int    lwt_sleep(int fd,double sltime);  
int    lwt_exit();

#endif                                             

