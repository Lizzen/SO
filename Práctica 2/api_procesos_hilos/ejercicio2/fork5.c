#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//Número de procesos 
#define N 8

/* programa que crea N hijos siguiendo un grado de dependencias. Usa las
 * versiones de execl y execlp */

/*Las llamadas execl y execlp reemplaza la imagen del proceso actual con un nuevo programa, por lo que no se ejecutará ningún código después de la llamada en los procesos secundarios.*/

int main(int argc, char **argv)
{
	pid_t proceso[N];
	int status;

	printf("P0 - Proceso Padre con id: %d\n", getpid());

	// Procesos Hijos
	// P1
	proceso[0] = fork();
	
	if (proceso[0] < 0){
		perror("fork en P1");
		exit(EXIT_FAILURE);
	}
	else if(proceso[0] == 0){
		//execlp("echo", "echo", "P1", NULL);
		printf("P1 - Proceso Hijo con id: %d\n", getpid());
		sleep(1);
		printf("Fin P1\n");
		exit(EXIT_SUCCESS);
	}
	// Espera P2 a P1
	waitpid(proceso[0], &status, WUNTRACED);

	// P2
	proceso[1] = fork();
	
	if (proceso[1] < 0){
		perror("fork en P2");
		exit(EXIT_FAILURE);
	}
	else if(proceso[1] == 0){
		//execl("/bin/echo", "echo", "P2", NULL);
		printf("P2 - Proceso Hijo con id: %d\n", getpid());
		sleep(1);
		printf("Fin P2\n");
		exit(EXIT_SUCCESS);
	}

	// P5
	proceso[4] = fork();
	
	if (proceso[4] < 0){
		perror("fork en P5");
		exit(EXIT_FAILURE);
	}
	else if(proceso[4] == 0){
		//execl("echo", "echo", "P5", NULL);
		printf("P5 - Proceso Hijo con id: %d\n", getpid());
		sleep(1);
		printf("Fin P5\n");
		exit(EXIT_SUCCESS);
	}

	// P3 y P4 esperan a P2
	waitpid(proceso[1], &status, WUNTRACED);


	// P3
	proceso[2] = fork();
	
	if (proceso[2] < 0){
		perror("fork en P3");
		exit(EXIT_FAILURE);
	}
	else if(proceso[2] == 0){
		//execl("echo", "echo", "P3", NULL);
		printf("P3 - Proceso Hijo con id: %d\n", getpid());
		sleep(1);
		printf("Fin P3\n");
		exit(EXIT_SUCCESS);
	}
	waitpid(proceso[2], &status, WUNTRACED);

	// P4
	proceso[3] = fork();
	
	if (proceso[3] < 0){
		perror("fork en P4");
		exit(EXIT_FAILURE);
	}
	else if(proceso[3] == 0){
		//execl("/bin/echo", "echo", "P4", NULL);
		printf("P4 - Proceso Hijo con id: %d\n", getpid());
		sleep(1);
		printf("Fin P4\n");
		exit(EXIT_SUCCESS);
	}
	waitpid(proceso[3], &status, WUNTRACED);



	// P6
	proceso[5] = fork();
	
	if (proceso[5] < 0){
		perror("fork en P6");
		exit(EXIT_FAILURE);
	}
	else if(proceso[5] == 0){
		//execl("/bin/echo", "echo", "P6", NULL);
		printf("P6 - Proceso Hijo con id: %d\n", getpid());
		sleep(1);
		printf("Fin P6\n");
		exit(EXIT_SUCCESS);
	}

	//P7 espera a P6
	waitpid(proceso[5], &status, WUNTRACED);

	// P7
	proceso[6] = fork();
	
	if (proceso[6] < 0){
		perror("fork en P7");
		exit(EXIT_FAILURE);
	}
	else if(proceso[6] == 0){
		//execl("echo", "echo", "P7", NULL);
		printf("P7 - Proceso Hijo con id: %d\n", getpid());
		sleep(1);
		printf("Fin P7\n");
		exit(EXIT_SUCCESS);
	}

	// P8 espera a P2, P3 y P7
	waitpid(proceso[1], &status, WUNTRACED);
	waitpid(proceso[2], &status, WUNTRACED);
	waitpid(proceso[6], &status, WUNTRACED);

	// P8
	proceso[7] = fork();
	
	if (proceso[7] < 0){
		perror("fork en P8");
		exit(EXIT_FAILURE);
	}
	else if(proceso[7] == 0){
		//execl("/bin/echo", "echo", "P8", NULL);
		printf("P8 - Proceso Hijo con id: %d\n", getpid());
		sleep(1);
		printf("Fin P8\n");
		exit(EXIT_SUCCESS);
	}
	waitpid(proceso[7], &status, WUNTRACED);

	printf("Fin Padre\n");


	return 0;
}
