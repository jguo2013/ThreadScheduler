#ifndef TEST_H
#define TEST_H

#define PROD_BUF_SIZE   2                                           
#define THRESHOLD       1000                                        

#define TEST_EMPTY   0                                              
#define TEST_FULL    1                                              
#define DEBUG_TEST   1

int test_buf[PROD_BUF_SIZE];

void test_pro_plain();  
void test_con_plain(); 
                                          
#endif
