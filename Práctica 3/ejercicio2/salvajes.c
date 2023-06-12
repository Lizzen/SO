#include "includes.h"

#define SEMSALVAJE "SEMSALVAJE"
#define NUMITER 3

int* comida;
sem_t* servido;
sem_t* servir;
sem_t* salvaje;

int getServingsFromPot(void)
{
	printf("Sirviendo comida del caldero\n");
	(*comida)--;
}

void eat(void)
{
	unsigned long id = (unsigned long) getpid();
	printf("Savage %lu eating\n", id);
	sleep(rand() % 5);
}

void savages(void)
{
	sem_wait(salvaje);
	int i;
	for (i = 0; i < NUMITER; i++){
		if (comida == 0){
			sem_post(servir);
			sem_wait(servido);
		}
		getServingsFromPot();
  		eat();
	}
	sem_post(salvaje);
}

int main(int argc, char *argv[])
{
	salvaje = sem_open(SEMSALVAJE, O_CREAT, 0700, 1);
	servir = sem_open(SEMSERVIR, O_RDWR, 0700, 0);
	servido = sem_open(SEMSERVIDO, O_RDWR, 0700, 0);


	int fd = 0;//File descriptor para el objeto compartido en memoria
	if((fd = shm_open(SMOBJ_NAME, O_RDWR, 0666)) == -1)
	{
		perror("Error shm_open");
		exit(EXIT_FAILURE);
	}

	/*Remapeo de memoria compartida para procesar la mem virtual*/
	comida = (int*)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	
	if (comida == MAP_FAILED)
	{
		perror("Error mmap");
		exit(EXIT_FAILURE);
	}
	
	savages();

	/*Unmapping memoria compartida*/
	munmap(comida, sizeof(int));
	close(fd);
	sem_unlink(SEMSALVAJE);
	return 0;
}
