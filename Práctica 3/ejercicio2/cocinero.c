#include "includes.h"


#define M 10

int finish = 0;
int* comida;

sem_t* servir;
sem_t* servido;

void putServingsInPot(int servings)
{
	printf("Rellenando el caldero con %d raciones...\n", servings);
	*comida += servings;
}

void cook(void)
{
	while(!finish) {
		sem_wait(servir);
		putServingsInPot(M);
		sem_post(servido);
	}
}

void handler(int signo)
{
	finish = 1;
	/*Unmapping memoria compartida*/
	munmap(comida, sizeof(int));
	shm_unlink(SMOBJ_NAME);
	sem_unlink(SEMSERVIR);
	sem_unlink(SEMSERVIDO);
	exit(0);
}

int main(int argc, char *argv[])
{
	struct sigaction sa;
	sa.sa_handler = handler;
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);

	servir = sem_open(SEMSERVIR, O_CREAT, 0700, 0);
	servido = sem_open(SEMSERVIDO, O_CREAT, 0700, 0);

	int fd = 0;//File descriptor para el objeto compartido en memoria
	if((fd = shm_open(SMOBJ_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR)) == -1)
	{
		perror("Error shm_open");
		exit(EXIT_FAILURE);
	}

	/*Resizing la memoria compartida del objeto*/
	ftruncate(fd, sizeof(int));

	/*Remapeo de memoria compartida para procesar la mem virtual*/
	comida = (int*)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if (comida == MAP_FAILED)
	{
		perror("Error mmap");
		exit(EXIT_FAILURE);
	}
	
	//cooking...
	cook();

	close(fd);
	
	return 0;
}
