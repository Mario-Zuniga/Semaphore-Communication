#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/wait.h>


#define CICLOS 10

void semwait(int semid);
void semsignal(int semid);
int createsem(int key, int value);
void erasesem(int semid);

char *pais[3]={"Peru","Bolvia","Colombia"};
int *g;
int sem;

void proceso(int i)
{
	int k;
	int l;
	for(k=0;k<CICLOS;k++)
	{	
		semwait(sem);
		// Entrada a la sección crítica
		printf("Entra %s",pais[i]);
		fflush(stdout);
		sleep(rand()%3);
		printf("- %s Sale\n",pais[i]);
		// Salida de la sección crítica
		semsignal(sem);
		// Espera aleatoria fuera de la sección crítica
		sleep(rand()%3);
	}
	exit(0); // Termina el proceso
}


int main()
{
int pid;
int status;
int args[3];
int i;

sem=createsem((key_t) 9236,1);




srand(getpid());
for(i=0;i<3;i++)
{
	// Crea un nuevo proceso hijo que ejecuta la función proceso()
	pid=fork();
	if(pid==0)
	proceso(i);
}
for(i=0;i<3;i++)
	pid = wait(&status);

  erasesem(sem);
}


void semwait(int semid)
{
	struct sembuf s;
	s.sem_num=0; // Indice del semaforo a actuar
	s.sem_op=-1; //Valor a incrementar o decrementar
	s.sem_flg=SEM_UNDO;	//flag
	
	semop(semid,&s,1); //Identificador, estructura de operaciones, elementos del array 
	return;
}
	
void semsignal(int semid)
{
	struct sembuf s;
	s.sem_num=0; // Indice del semaforo a actuar
	s.sem_op=1; // Valor del semaforo
	s.sem_flg=SEM_UNDO; //Identificador, estructura de operaciones, elementos del array 
	
	semop(semid,&s,1);
	return;
}

int createsem(int key, int value)
{
	int semid;
	semid=semget(key,1,0666|IPC_CREAT); //Identificador, número de semáforos (arreglo), macro predet
	semctl(semid,0,SETVAL,value);	//Identificador, Indice del semaforo, configurar valor , valor inicial
	return(semid);
}

void erasesem(int semid)
{
	semctl(semid,0,IPC_RMID,0);	// Identificador, Indice del semáforo,  eliminar el conjunto de semaforos, 
	return;
}