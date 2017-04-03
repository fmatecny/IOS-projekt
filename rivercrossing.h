#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>  
#include <errno.h>

// fork() a wait()
#include <unistd.h>
#include <sys/wait.h>
 
// Semafory a zdielana pamat
#include <sys/types.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <fcntl.h>
#include <signal.h>

pid_t getpid(void);

typedef enum {
	          ERR_ARG,
	          
			  ERR_FILE_OPEN,
			  ERR_FILE_CLOSE,
			  
			  ERR_FORK,
			  
			  ERR_SEM_OPEN,
			  ERR_SEM_CLOSE,
			  ERR_SEM_UNLINK,
			  
			  ERR_SHM_OPEN,
			  ERR_SHM_CUT,
			  ERR_SHM_MAP,
			  ERR_SHM_UNMAP,
			  ERR_SHM_UNLINK,
			  ERR_SHM_CLOSE
			  } errPrint;

typedef enum {
			  START_H,
			  START_S,
			  
			  WAIT_H,
			  WAIT_S,
			  
			  BOARD_H,
			  BOARD_S,
			  
			  MEM_H,
			  MEM_S,
			  
			  LANDING_H,
			  LANDING_S,
			  
			  END_H,
			  END_S
			  } print;


unsigned int sleep_time(long int cas);

void print_err (errPrint i);

void load_arg (int argc, char *argv[]);

void clean();

void print_out (print a, int b, char *crew);

void hackerChild (int i);

void serfChild (int j);

