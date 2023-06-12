#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#define N_HILOS 10

struct th_user{
	int userId;
	char priority;
};

void *thread_usuario(void *arg)
{
	// Inicializar
	struct th_user *args = (struct th_user *)arg;
	int userId = args->userId;
	char priority = args->priority;
	// Liberar memoria dinámica
	free(args);
	// Devolver ID del thread
	pthread_t threadId = pthread_self();

	printf("Hilo %lu - Usuario %d - Prioridad %c\n", threadId, userId, priority);

	pthread_exit(0);
}

int main(int argc, char* argv[])
{
	pthread_t threads[N_HILOS];

	for (int i = 0; i < N_HILOS; ++i){
		struct th_user *args = (struct th_user *)malloc(sizeof(struct th_user));
		args->userId = i;
		args->priority = (i % 2 == 0) ?  'P' : 'N';

		// int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg);
		// (hilo, atributo, función, argumento)
		pthread_create(&threads[i], NULL, thread_usuario, args);
	}

	for (int i = 0; i < N_HILOS; ++i){
		// (Hilo a terminar, valor devuelto por la función)
		pthread_join(threads[i], NULL);
	}

	return 0;
}
