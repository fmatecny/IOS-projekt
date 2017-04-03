/**
 * Subor: rivercrossing.c
 * Datum: 2014/05/04
 * Autor: František Matečný, FIT
 * Projekt: 2. uloha z predmetu IOS - semafory
 **/

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

#include "rivercrossing.h"

long int pocet;
long int t_hackers;
long int t_serf;
long int t_plavba;

sem_t *vypis;
sem_t *sem1a;
sem_t *sem1b;
sem_t *sem2;
sem_t *sem3;
sem_t *sem4;
sem_t *sem5;
sem_t *sem6;
sem_t *cap;
sem_t *member;
sem_t *land;
sem_t *end;

int *sharedVar;
int *moloHack;
int *moloSerf;
int *boardCrewStart;
int *boardCrewEnd;

int shm_fd;
int shm_fd1;
int shm_fd2;
int shm_fd3;
int shm_fd4;


int captain = 0;

FILE *subor;


/**
 * uspanie procesu v rozsahu 0 az cas
 **/
unsigned int sleep_time(long int cas){
	
	srand(time(NULL) * getpid());
	
	if (cas == 0)
		{
			return 0;
		}	
		else {
				return (rand () % cas) * 1000;
			 }
}


/**
 * vypis chyb
 **/
void print_err (errPrint i){
	
switch (i)
	{
		//ERR_ARG
		case (ERR_ARG): 		fprintf(stderr, "zle argumenty\n");
								break;
								
		//ERR_FILE		 
		case (ERR_FILE_OPEN):	fprintf(stderr, "chyba pri otvoreni suboru\n");
								break;
				 
		case (ERR_FILE_CLOSE): 	fprintf(stderr, "chyba pri zatvoreni suboru\n");
								break;
								
		//ERR_FORK		 
		case (ERR_FORK): 		fprintf(stderr, "chyba pri fork\n");
								break;
								
		//ERR_SEM		 
		case (ERR_SEM_OPEN): 	fprintf(stderr, "chyba pri otvarani semaforu\n");
								break;
				 
		case (ERR_SEM_CLOSE): 	fprintf(stderr, "chyba pri zatvarani semaforu\n");
								break;
				 
		case (ERR_SEM_UNLINK): 	fprintf(stderr, "chyba pri cisteni pamati po semaforoch\n");
								break;
								
		//ERR_SHM		 
		case (ERR_SHM_OPEN): 	fprintf(stderr, "chyba pri otvarani zdielanej pamate\n");
								break;
				 
		case (ERR_SHM_CUT): 	fprintf(stderr, "chyba pri skrateni zdielanej pamate\n");
								break;
				 
		case (ERR_SHM_MAP): 	fprintf(stderr, "chyba pri mapovani zdielanej pamate\n");
								break;
				 
		case (ERR_SHM_UNMAP): 	fprintf(stderr, "chyba pri odmapovani zdielanej pamate\n");
								break;
				 
		case (ERR_SHM_UNLINK): 	fprintf(stderr, "chyba pri mazani zdielanej pamate\n");
								break;
				 
		case (ERR_SHM_CLOSE): 	fprintf(stderr, "chyba pri zatvoreni zdielanej pamate\n");
								break;
				 
		default: fprintf(stderr, "neznama chyba\n");
	}

fflush(stderr);

if (i < ERR_FORK)
		exit(1);
		else exit(2);
}


/**
 * kontrola argumentov
 **/
void load_arg (int argc, char *argv[]){

	if (argc != 5)
		{
			print_err(ERR_ARG);
		}
		
	char *pEnd;
		
	if ( ((pocet = strtol (argv[1],&pEnd,10)) == 0) || (strlen(argv[1]) > 5) )
		{
			print_err(ERR_ARG);
		}	
		else if ( (*pEnd != '\0') || (pocet % 2 != 0) )
			{
				print_err(ERR_ARG);
			}
	
	
	t_hackers = strtol (argv[2], &pEnd, 10);
	if ( (strlen(argv[2]) > 4) || (*pEnd != '\0') || (t_hackers > 5000) )
		{
			print_err(ERR_ARG);
		}
		else if ( (t_hackers == 0) && (strlen(argv[2]) != 1) )
			{
				print_err(ERR_ARG);
			}		
		
			
	t_serf = strtol (argv[3], &pEnd, 10);
	if ( (strlen(argv[3]) > 4) || (*pEnd != '\0') || (t_serf > 5000) )
		{
			print_err(ERR_ARG);
		}
		else if ( (t_serf == 0) && (strlen(argv[3]) != 1) )
			{
				print_err(ERR_ARG);
			}
			
	
	t_plavba = strtol (argv[4], &pEnd, 10);
	if ( (strlen(argv[4]) > 4) || (*pEnd != '\0') || (t_plavba > 5000) )
		{
			print_err(ERR_ARG);
		}
		else if ( (t_plavba == 0) && (strlen(argv[4]) != 1) )
			{
				print_err(ERR_ARG);
			}
}


/**
 * vycistenie pamati
 **/
void clean(){
	
	//zatvorenie vsetkych semaforov
	if ((sem_close(vypis)) == -1 ) {print_err(ERR_SEM_CLOSE);}
	if ((sem_close(sem1a)) == -1 ) {print_err(ERR_SEM_CLOSE);}
	if ((sem_close(sem1b)) == -1 ) {print_err(ERR_SEM_CLOSE);}
    if ((sem_close(sem2)) == -1 ) {print_err(ERR_SEM_CLOSE);}
    if ((sem_close(sem3)) == -1 ) {print_err(ERR_SEM_CLOSE);}
    if ((sem_close(sem4)) == -1 ) {print_err(ERR_SEM_CLOSE);}
    if ((sem_close(sem5)) == -1 ) {print_err(ERR_SEM_CLOSE);}
    if ((sem_close(sem6)) == -1 ) {print_err(ERR_SEM_CLOSE);}
    if ((sem_close(cap)) == -1 ) {print_err(ERR_SEM_CLOSE);}
    if ((sem_close(member)) == -1 ) {print_err(ERR_SEM_CLOSE);}
    if ((sem_close(land)) == -1 ) {print_err(ERR_SEM_CLOSE);}
    if ((sem_close(end)) == -1 ) {print_err(ERR_SEM_CLOSE);}

    // Kompletne vycistenie pamati po semaforoch
    if ((sem_unlink("/vypis")) == -1 ) {print_err(ERR_SEM_UNLINK);}
    if ((sem_unlink("/semafor1a")) == -1 ) {print_err(ERR_SEM_UNLINK);}
    if ((sem_unlink("/semafor1b")) == -1 ) {print_err(ERR_SEM_UNLINK);}
    if ((sem_unlink("/semafor2")) == -1 ) {print_err(ERR_SEM_UNLINK);}
    if ((sem_unlink("/semafor3")) == -1 ) {print_err(ERR_SEM_UNLINK);}
    if ((sem_unlink("/semafor4")) == -1 ) {print_err(ERR_SEM_UNLINK);}
    if ((sem_unlink("/semafor5")) == -1 ) {print_err(ERR_SEM_UNLINK);}
    if ((sem_unlink("/semafor6")) == -1 ) {print_err(ERR_SEM_UNLINK);}
    if ((sem_unlink("/cap")) == -1 ) {print_err(ERR_SEM_UNLINK);}
    if ((sem_unlink("/member")) == -1 ) {print_err(ERR_SEM_UNLINK);}
    if ((sem_unlink("/land")) == -1 ) {print_err(ERR_SEM_UNLINK);}
    if ((sem_unlink("/end")) == -1 ) {print_err(ERR_SEM_UNLINK);}

    // Odmapovanie pamate
    if ((munmap(sharedVar, sizeof(int))) == -1 ) {print_err(ERR_SHM_UNMAP);}
    if ((munmap(moloHack, sizeof(int))) == -1 ) {print_err(ERR_SHM_UNMAP);}
    if ((munmap(moloSerf, sizeof(int))) == -1 ) {print_err(ERR_SHM_UNMAP);}
    if ((munmap(boardCrewStart, sizeof(int))) == -1 ) {print_err(ERR_SHM_UNMAP);}
    if ((munmap(boardCrewEnd, sizeof(int))) == -1 ) {print_err(ERR_SHM_UNMAP);}

    // Zmazanie pamati
    if ((shm_unlink("/sharedMemory")) == -1 ) {print_err(ERR_SHM_UNLINK);}
    if ((shm_unlink("/sharedMemory1")) == -1 ) {print_err(ERR_SHM_UNLINK);}
    if ((shm_unlink("/sharedMemory2")) == -1 ) {print_err(ERR_SHM_UNLINK);}
    if ((shm_unlink("/sharedMemory3")) == -1 ) {print_err(ERR_SHM_UNLINK);}
    if ((shm_unlink("/sharedMemory4")) == -1 ) {print_err(ERR_SHM_UNLINK);}
    
    // Zatvorenie pamati
    if ((close(shm_fd)) == -1 ) {print_err(ERR_SHM_CLOSE);}
    if ((close(shm_fd1)) == -1 ) {print_err(ERR_SHM_CLOSE);}
    if ((close(shm_fd2)) == -1 ) {print_err(ERR_SHM_CLOSE);}
    if ((close(shm_fd3)) == -1 ) {print_err(ERR_SHM_CLOSE);}
    if ((close(shm_fd4)) == -1 ) {print_err(ERR_SHM_CLOSE);}

    
	//zatvorenie suboru
	if((fclose(subor)) == EOF)
		{
		print_err(ERR_FILE_CLOSE);
		}

}


/**
 * vypis do suboru rivercrossing.out
 **/
void print_out (print a, int b, char *crew){
	
sem_wait(vypis);
	
	fflush (subor);	
	
switch (a){
	
	case (START_H): 	fprintf(subor,"%d: hacker: %d: started\n", (*sharedVar)++, b);
						break;
					
	case (START_S): 	fprintf(subor,"%d: serf: %d: started\n", (*sharedVar)++, b);
						break;
					
	case (WAIT_H): 		fprintf(subor,"%d: hacker: %d: waiting for boarding: %d: %d\n", (*sharedVar)++, b, ++(*moloHack), *moloSerf );
						break;
				   
	case (WAIT_S): 		fprintf(subor,"%d: serf: %d: waiting for boarding: %d: %d\n", (*sharedVar)++, b, *moloHack, ++(*moloSerf) );
						break;
				   
	case (BOARD_H): 	fprintf(subor,"%d: hacker: %d: boarding: %d: %d\n", (*sharedVar)++, b, *moloHack, *moloSerf );
						(*boardCrewStart)++;
						break;
				   
	case (BOARD_S): 	fprintf(subor,"%d: serf: %d: boarding: %d: %d\n", (*sharedVar)++, b, *moloHack, *moloSerf );
						(*boardCrewStart)++;
						break;
				    
	case (MEM_H): 		fprintf(subor,"%d: hacker: %d: %s\n", (*sharedVar)++, b, crew);
						(*boardCrewStart)++;
						break;
					
	case (MEM_S): 		fprintf(subor,"%d: serf: %d: %s\n", (*sharedVar)++, b, crew);
						(*boardCrewStart)++;
						break;
					   
	case (LANDING_H): 	fprintf(subor,"%d: hacker: %d: landing: %d: %d\n", (*sharedVar)++, b, *moloHack, *moloSerf );
						(*boardCrewEnd)--;
						break;
				   
	case (LANDING_S): 	fprintf(subor,"%d: serf: %d: landing: %d: %d\n", (*sharedVar)++, b, *moloHack, *moloSerf );
						(*boardCrewEnd)--;
						break;
					  				
	case (END_H): 		fprintf(subor,"%d: hacker: %d: finished\n", (*sharedVar)++, b);
						break;
				  
	case (END_S): 		fprintf(subor,"%d: serf: %d: finished\n", (*sharedVar)++, b);
						break;
	
	
	default: fprintf(stderr,"chyba pri zapise\n");
			 fflush (subor);
			 exit(0);
	}
	
	fflush (subor);	
	
		
	if (crew != NULL)
		{
		if (strcmp(crew,"captain") == 0)
			{
				(*boardCrewStart) = 0;
				(*boardCrewEnd) = 4;
			}	
		}
		
		 
sem_post(vypis);
}


/**
 * pracovanie s child procesmi - hacker
 **/
void hackerChild (int i){
	
	//prvotny vypis - hacker
	print_out(START_H, i, NULL);
	sem_post(sem1a);
	
	//cakanie pred molom
	sem_wait(sem2);
	print_out(WAIT_H, i, NULL);
	//posledny proces ktory vytvori skupinu je captain
	if ( (*moloSerf == 4) || (*moloHack == 4) || ((*moloSerf >= 2) && (*moloHack == 2)) )
		{
			captain = 1;
			sem_post(sem3);
		}
		else {
			sem_post(sem2);
			}
	
	//molo
	sem_wait(sem3);
		if (*moloHack == 4)
			{
				*moloHack=0;	
				sem_post(sem4);
				sem_post(sem4);
				sem_post(sem4);
				sem_post(sem4);
			}
		else if ( (*moloHack == 2) && (*moloSerf >=2) )
			{
				*moloHack = 0;
				*moloSerf -= 2;
				sem_post(sem4);			
				sem_post(sem4);			
				sem_post(sem5);			
				sem_post(sem5);		
			}	
	sem_post(sem3);
	
	//nalodenie
	sem_wait(sem4);
		print_out(BOARD_H, i, NULL);
	
	//polsedny nalodeni proces otvori semafor na vypis clenov posadky
	if (*boardCrewStart >= 4)
		{
		sem_post(member);
		}
	
	
	if (captain == 1)
		{
		sem_post(sem6);
		sem_wait(cap);
		}
	
	
	sem_wait(member);
	//vypis clenov posadky a kapitana
			sem_wait(sem6);
			if (captain == 0) 
				{
					print_out(MEM_H, i,"member");		//member
					sem_post(member);
					sem_post(sem6);
					if (*boardCrewStart == 7)
						{
							sem_post(cap);
						}
				}
				else if (captain == 1)		//captain
					{
						print_out(MEM_H, i,"captain");
						usleep(sleep_time(t_plavba));
						sem_post(land);
					}
			
	//vylodenie
	sem_wait(land);		
		print_out(LANDING_H, i, NULL);
	
	
	if (*boardCrewEnd == 0)
		{
			sem_post(sem2);
		}	
	else {
		sem_post(land);
		}
	
	
	//posledny proces povoli vypisanie finished
	if (*sharedVar == ((pocet*10)+1) )
		{
			sem_post(end);
		}
	//vypis finished		
	sem_wait(end);
		print_out(END_H, i, NULL);
	sem_post(end);
	
	//kazdy proces zatvori semafori s ktorymi pracoval
	sem_close(vypis);
	sem_close(sem1a);
	sem_close(sem2);
	sem_close(sem3);
	sem_close(sem4);
	sem_close(sem5);
	sem_close(sem6);
	sem_close(cap);
	sem_close(member);
	sem_close(land);
	sem_close(end);
	
	//ukoncenie child procesu - hacker
	exit(0);	

}


/**
 * pracovanie s child procesmi - serf
 **/
void serfChild (int j){
	
	//prvotny vypis - serf
	print_out(START_S, j, NULL);
	sem_post(sem1b);
	
	//cakanie pred molom
	sem_wait(sem2);
	print_out(WAIT_S, j, NULL);
	//posledny proces ktory vytvori skupinu je captain
	if ( (*moloSerf == 4) || (*moloHack == 4) || ((*moloSerf == 2) && (*moloHack >= 2)) )
		{
			captain = 1;
			sem_post(sem3);
		}
		else {
			sem_post(sem2);
			}

	//molo
	sem_wait(sem3);
		if (*moloSerf == 4)
			{
				*moloSerf=0;	
				sem_post(sem5);
				sem_post(sem5);
				sem_post(sem5);
				sem_post(sem5);
			}
			else if ( (*moloSerf == 2) && (*moloHack >=2) )
				{
					*moloSerf = 0;
					*moloHack -= 2;
					sem_post(sem4);
					sem_post(sem4);
					sem_post(sem5);
					sem_post(sem5);
				}	
	sem_post(sem3);

	//nalodenie
	sem_wait(sem5);
		print_out(BOARD_S, j, NULL);
	
	//posledny proces 
	if (*boardCrewStart >= 4)
		{
		sem_post(member);
		}
	
	if (captain == 1)
		{
		sem_post(sem6);
		sem_wait(cap);
		}
	
	
	sem_wait(member);
	//vypis clenov posadky a kapitana
	if (captain == 0) 
		{
			print_out(MEM_S, j,"member");
			sem_post(member);
			sem_post(sem6);
			if (*boardCrewStart == 7)
				{	
					sem_post(cap);
				}
		}
		else if (captain == 1)
			{
				print_out(MEM_S, j,"captain");
				usleep(sleep_time(t_plavba));
				sem_post(land);
			}
	
	//vylodenie
	sem_wait(land);		
		print_out(LANDING_S, j, NULL);

	
	if (*boardCrewEnd == 0)
		{
			sem_post(sem2);
		}	
		else {
				sem_post(land);
			 }
	
	//posledny proces povoli vypisanie finished
	if (*sharedVar == ((pocet*10)+1) ){sem_post(end);}
	
	//vypis finished	
	sem_wait(end);
		print_out(END_S, j, NULL);
	sem_post(end);

	//kazdy proces zatvori semafori s ktorymi pracoval
	sem_close(vypis);
	sem_close(sem1b);
	sem_close(sem2);
	sem_close(sem3);
	sem_close(sem4);
	sem_close(sem5);
	sem_close(sem6);
	sem_close(cap);
	sem_close(member);
	sem_close(land);
	sem_close(end);
	
	//ukoncenie child procesu - serf
	exit(0);	

}

/**
 * Hlany program
 **/
int main (int argc, char *argv[]){
	
	signal(SIGINT, clean);
	signal(SIGKILL, clean);
	
	
	load_arg(argc, argv);
	
	subor = fopen("rivercrossing.out","w");
	if (subor == NULL)
	{
		print_err(ERR_FILE_OPEN);
	}
	
	setbuf(subor, NULL);

	
	// Vytvorenie semaforov
    if ((vypis = sem_open("/vypis", O_CREAT | O_EXCL, 0644, 1)) == SEM_FAILED) {print_err(ERR_SEM_OPEN);}
    if ((sem1a = sem_open("/semafor1a", O_CREAT | O_EXCL, 0644, 0)) == SEM_FAILED) {print_err(ERR_SEM_OPEN);}
    if ((sem1b = sem_open("/semafor1b", O_CREAT | O_EXCL, 0644, 0)) == SEM_FAILED) {print_err(ERR_SEM_OPEN);}
    if ((sem2 = sem_open("/semafor2", O_CREAT | O_EXCL, 0644, 1)) == SEM_FAILED) {print_err(ERR_SEM_OPEN);}
    if ((sem3 = sem_open("/semafor3", O_CREAT | O_EXCL, 0644, 0)) == SEM_FAILED) {print_err(ERR_SEM_OPEN);}
    if ((sem4 = sem_open("/semafor4", O_CREAT | O_EXCL, 0644, 0)) == SEM_FAILED) {print_err(ERR_SEM_OPEN);}
    if ((sem5 = sem_open("/semafor5", O_CREAT | O_EXCL, 0644, 0)) == SEM_FAILED) {print_err(ERR_SEM_OPEN);}
    if ((sem6 = sem_open("/semafor6", O_CREAT | O_EXCL, 0644, 0)) == SEM_FAILED) {print_err(ERR_SEM_OPEN);}
    if ((cap = sem_open("/cap", O_CREAT | O_EXCL, 0644, 0)) == SEM_FAILED) {print_err(ERR_SEM_OPEN);}
    if ((member = sem_open("/member", O_CREAT | O_EXCL, 0644, 0)) == SEM_FAILED) {print_err(ERR_SEM_OPEN);}
    if ((land = sem_open("/land", O_CREAT | O_EXCL, 0644, 0)) == SEM_FAILED) {print_err(ERR_SEM_OPEN);}
    if ((end = sem_open("/end", O_CREAT | O_EXCL, 0644, 0)) == SEM_FAILED) {print_err(ERR_SEM_OPEN);}


    // Vytvorenie zdielanej pamate
    if ((shm_fd = shm_open("/sharedMemory", O_CREAT | O_EXCL | O_RDWR, 0644)) == -1 ) {print_err(ERR_SHM_OPEN);}
    if ((shm_fd1 = shm_open("/sharedMemory1", O_CREAT | O_EXCL | O_RDWR, 0644)) == -1 ) {print_err(ERR_SHM_OPEN);}
    if ((shm_fd2 = shm_open("/sharedMemory2", O_CREAT | O_EXCL | O_RDWR, 0644)) == -1 ) {print_err(ERR_SHM_OPEN);}
    if ((shm_fd3 = shm_open("/sharedMemory3", O_CREAT | O_EXCL | O_RDWR, 0644)) == -1 ) {print_err(ERR_SHM_OPEN);}
    if ((shm_fd4 = shm_open("/sharedMemory4", O_CREAT | O_EXCL | O_RDWR, 0644)) == -1 ) {print_err(ERR_SHM_OPEN);}
	
	// Skratenie zdielanej pamati na velkost jeden int
    if ((ftruncate(shm_fd, sizeof(int))) == -1 ) {print_err(ERR_SHM_CUT);}
    if ((ftruncate(shm_fd1, sizeof(int))) == -1 ) {print_err(ERR_SHM_CUT);}
    if ((ftruncate(shm_fd2, sizeof(int))) == -1 ) {print_err(ERR_SHM_CUT);}
    if ((ftruncate(shm_fd3, sizeof(int))) == -1 ) {print_err(ERR_SHM_CUT);}
    if ((ftruncate(shm_fd4, sizeof(int))) == -1 ) {print_err(ERR_SHM_CUT);}
	
	// Namapovanie pamate
    if ((sharedVar = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0)) == MAP_FAILED ) {print_err(ERR_SHM_MAP);}
    if ((moloHack = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd1, 0)) == MAP_FAILED ) {print_err(ERR_SHM_MAP);}
    if ((moloSerf = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd2, 0)) == MAP_FAILED ) {print_err(ERR_SHM_MAP);}
    if ((boardCrewStart = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd3, 0)) == MAP_FAILED ) {print_err(ERR_SHM_MAP);}
    if ((boardCrewEnd = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd4, 0)) == MAP_FAILED ) {print_err(ERR_SHM_MAP);}
    
    *sharedVar = 1;
    *moloHack = 0;
    *moloSerf = 0;
    *boardCrewStart = 0;
    *boardCrewEnd = 0;
    
    pid_t hacker;
    pid_t group_hacker;
    
    if ((group_hacker = fork()) == -1) {print_err(ERR_FORK);}
    
    if (group_hacker == 0) // Child - hacker
    {
        for (int i = 1; i <= pocet; i++)
        {
			
			usleep(sleep_time(t_hackers));
			
			if ((hacker = fork()) == -1) {print_err(ERR_FORK);}
			
			if (hacker == 0)	//child
				{
					hackerChild(i);
				}
				
			sem_wait(sem1a);
        }
        
        wait(NULL);
		if ((sem_close(vypis)) == -1 ) {print_err(ERR_SEM_CLOSE);}
		if ((sem_close(sem1a)) == -1 ) {print_err(ERR_SEM_CLOSE);}
        if ((sem_close(sem2)) == -1 ) {print_err(ERR_SEM_CLOSE);}
		if ((sem_close(sem3)) == -1 ) {print_err(ERR_SEM_CLOSE);}
        if ((sem_close(sem4)) == -1 ) {print_err(ERR_SEM_CLOSE);}

        exit(0);
        
    }
    else if (group_hacker > 0)
		{
			
			pid_t serf;
			pid_t group_serf;
			
			if ((group_serf = fork()) == -1 ) {print_err(ERR_FORK);}
			
			if (group_serf == 0) //child - group_serf
			{
				for (int j = 1; j <= pocet; j++)
				{
					
					usleep(sleep_time(t_serf));
					if ((serf = fork()) == -1 ) {print_err(ERR_FORK);}
					
					if (serf == 0)	//child - serf
						{
							serfChild (j);
						}
						
					sem_wait(sem1b);
				}
				
				wait(NULL);
				if ((sem_close(vypis)) == -1 ) {print_err(ERR_SEM_CLOSE);}
				if ((sem_close(sem1b)) == -1 ) {print_err(ERR_SEM_CLOSE);}
				if ((sem_close(sem2)) == -1 ) {print_err(ERR_SEM_CLOSE);}
				if ((sem_close(sem3)) == -1 ) {print_err(ERR_SEM_CLOSE);}
				if ((sem_close(sem4)) == -1 ) {print_err(ERR_SEM_CLOSE);}
				
				exit(0);
			}
			
		}

	wait(NULL);
	
	clean();
	
	exit(0);
}
