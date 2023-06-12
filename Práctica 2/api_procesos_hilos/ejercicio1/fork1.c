#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/*programa que crea dos hijos: uno no cambia de ejecutable y otro si */


int main(int argc, char **argv)
{
	int pid1, pid2;
	int status;

	//Primera llamada a fork
	pid1 =  fork();

	if (pid1 < 0){
		perror("Error fork pid1");
		exit(EXIT_FAILURE);
	}
	else if (pid1 == 0){
		// Proceso hijo 1
		printf ("Soy el hijo 1. Mi PID es %d. Mi padre es %d.\n", getpid(), getppid());

		// Segunda llamada a fork en hijo 1
		pid2 = fork();

		if (pid1 < 0){
			perror("Error fork pid2");
			exit(EXIT_FAILURE);
		}
		else if (pid2 == 0){
			// Proceso hijo 2
			printf ("Soy el hijo 2. Mi PID es %d. Mi padre es %d.\n", getpid(), getppid());

			// Cambiar ejecutable del hijo 2
			if (execvp(argv[1], &argv[1]) < 0){
				perror("Error execvp");
				exit(EXIT_FAILURE);
			}
		}
	}
	else{
		// Proceso padre
		waitpid(pid1, &status, 0);
		printf("Fin hijo 1.\n");
		exit(EXIT_FAILURE);
	}

	return 0;
}
