#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/msg.h>
#include <wait.h>

#define CICLOS 10

/* Definir una estructura que se va a usar para mandar los mensajes */
typedef struct {
	long msg_type;		/* Tipo de mensaje */
	char mensaje[100];	/* Contenido */
} MSGTYPE;

int msgqid;

void emisor();
void receptor();


char *pais[3] = {"Peru","Bolivia","Colombia"};

int *g;

void proceso(int i) {
	int k;

	for(k = 0; k < CICLOS; k++) {
		receptor();
		/* Entrada a la seccion critica */
		printf("Entra %s ", pais[i]);
		fflush(stdout);
		sleep(rand()%3);
		printf("- %s Sale\n", pais[i]);
		emisor();
		/* Salida de la seccion critica */
		/* Espera aleatoria fuera de la seccion critica */
		sleep(rand()%3);
		
	}
	exit(0);	/* Termina el proceso */
}


int main() {
	int pid;
	int status;
	int i;

	srand(getpid());

	/* Creamos buzon */
	msgqid = msgget(0x1234,0666|IPC_CREAT);
	
	/* Mandamos el primer mensaje al emisor, esto nos permite darle acceso al primer proceso*/
	emisor();

	for(i = 0; i < 3; i++) {
		/* Crea un nuevo proceso hijo que ejecuta la funcion proceso() */
		pid = fork();


		if(pid == 0){
			proceso(i);
		}
	}
	
	for(i = 0; i < 3; i++)
		pid = wait(&status);

	/* Eliminamos el buzon */
	msgctl(msgqid,IPC_RMID,NULL);
}


void emisor() {
	MSGTYPE m;	/* Valor m de la estructura */
	m.msg_type = 1;	/* Tipo de mensaje 1 */
	strcpy(m.mensaje,"Mensaje");	/* Contenido del mensaje */
	msgsnd(msgqid,&m,sizeof(MSGTYPE)-sizeof(long),0);	/* El mensaje es mandado */
	return;
}

void receptor() {
	MSGTYPE m;	/* Estructura tipo n */

	/* En este ciclo se mantendra el receptor hasta recivir un mensaje con contenido "Mensaje" */
	do{
		msgrcv(msgqid,&m,sizeof(MSGTYPE)-sizeof(long),1,0);	/* Recibimos el mensaje */
	}while(strcmp(m.mensaje,"Mensaje")!=0);	/* Condicion del ciclo */
	
	return;
}