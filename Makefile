CC = gcc
DDBUG = -g 
INC = ./include
INCDIRS = -I${INC}
CFLAGS = ${INCDIRS} ${DDBUG}

all: scheduler

clean:
	rm -f *.o scheduler

SCHEDULER_SRC = main.c lwt.c test.c thrd.c sem.c

SCHEDULER_OBJ = $(SCHEDULER_SRC:.c=.o)  

$(SCHEDULER_OBJ): %.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

scheduler : main.o $(SCHEDULER_OBJ)
	$(CC) $(CFLAGS) -o $@ $(SCHEDULER_OBJ)