/* 
 * Contributors: Jie Guo
 *
 * Description: semaphore
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern S;
extern S_avail;
extern S_fill;

int V( int type,int s);
int P( int type,int s);

#define SEM_MUTEX    0
#define SEM_FULL     1
#define SEM_EMPTY    2