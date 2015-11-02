#ifndef THRD_H
#define THRD_H

#include "lwt.h"

#define THRD_SET_SP   0
#define THRD_SET_ALL  1
#define THRD_SAVE_ALL 2

#define THRD_AVA_LN   0
#define THRD_RDY_LN   1
#define THRD_WAIT_LN  2
#define THRD_SLP_LN   3
#define THRD_SEM_LN   4
#define THRD_CTH_LN   5                          //for child thread tree
#define THRD_SEM_LN_1 6
#define THRD_SEM_LN_2 7

#define THRD_RMV_OP   0
#define THRD_ADD_OP   1
#define THRD_SEMS_OP  2                          
#define THRD_CTHS_OP  3                          //search for children thread
#define THRD_SCH_FD   4

thrd_t    *thrd_getnew(void (*f_pt) (), thrd_t * parent_fd);
thrd_t    *thrd_gets(int curr_fd);
thrd_t    *thrd_getrdy();
int        thrd_setr(thrd_t *thrd, int type);     
int        thrd_wait(thrd_t* parent);
thrd_t    *thrd_rmvp(thrd_t*child);            		
int        thrd_awk();                            
int        thrd_rmv();                            
int        thrd_sleep(int fd, double sltime);
thrd_t*    thrd_link_op(thrd_t *thrd,int type, int op);
thrd_t*    _add_link(thrd_t **head,thrd_t *entry, unsigned int* num);
thrd_t*    _sch_link(thrd_t **head,int type, int value, unsigned int* num);
thrd_t*    _remove_link(thrd_t **head,thrd_t *entry, unsigned int* num);
cthrd_t*   _add_link_(cthrd_t **head,cthrd_t *entry, unsigned int* num);
cthrd_t*   _remove_link_(cthrd_t **head,cthrd_t *entry, unsigned int* num);
cthrd_t* _sch_link_(cthrd_t** head, int type, unsigned int *value, unsigned int* num);
#endif
