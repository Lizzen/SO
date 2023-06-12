#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

/*programa que temporiza la ejecución de un proceso hijo */

pid_t pid;

// Enviar la señal SIGKILL al proceso actual
void sigkillHandler(int sig) {
    kill(getpid(), SIGKILL);
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: %s <nombre_ejecutable>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Crear el proceso hijo
    pid = fork();

    if (pid == -1) {
        perror("Error en fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Proceso hijo
		printf("Proceso Hijo\n");

        // Cambiar la ejecución al programa especificado
        if (execvp(argv[1], &argv[1]) == -1) {
            perror("Error en execvp");
            exit(EXIT_FAILURE);
        }
    } else {
        // Proceso padre
		printf("Proceso Padre\n");

        // Establecer el manejador de señal SIGALRM
        struct sigaction sa;
        sa.sa_handler = sigkillHandler;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;

        if (sigaction(SIGALRM, &sa, NULL) == SIG_ERR) {
            perror("Error en sigaction");
            exit(EXIT_FAILURE);
        }

		// Ignorar la señal SIGINT
    	if (signal(SIGINT, SIG_IGN) == SIG_ERR){
        	printf("Error al capturar la señal SIGINT\n");
        	exit(EXIT_FAILURE);
    	}

        // Programar una alarma para enviar una señal SIGALRM después de 5 segundos
        alarm(5);
		int status;

        // Esperar a que el hijo termine
        if (waitpid(pid, &status, WUNTRACED) == -1) {
            perror("Error en waitpid");
            exit(EXIT_FAILURE);
        }

        if (status == 0) {
            printf("El hijo terminó de forma normal con código de salida %d\n", WEXITSTATUS(status));
        } else {
            printf("El hijo fue terminado por una señal: %d\n", WTERMSIG(status));
        }
    }
	
	return 0;
}
