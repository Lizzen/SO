#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define CAPACITY 5
#define VIPSTR(vip) ((vip) ? "  vip  " : "not vip")

pthread_mutex_t m;
pthread_cond_t vaciar;
int capacidad;

struct cliente{
	int id;
	int isvip;
};

void enter_normal_client(int id)
{
	pthread_mutex_lock(&m);
	printf("cliente normal entra al club con id: %d \n", id);
	if (capacidad == CAPACITY){
		pthread_cond_wait(&vaciar, &m);
	}

	capacidad++;
	pthread_mutex_unlock(&m);
}

void enter_vip_client(int id)
{
	pthread_mutex_lock(&m);
	printf("cliente vip entra al club con id: %d \n", id);
	if (capacidad == CAPACITY){
		pthread_cond_wait(&vaciar, &m);
	}

	capacidad++;
	pthread_mutex_unlock(&m);
}

void dance(int id, int isvip)
{
	printf("Client %2d (%s) dancing in disco\n", id, VIPSTR(isvip));
	sleep((rand() % 3) + 1);
}

void disco_exit(int id, int isvip)
{
	printf("Cliente %2d (%s) saliendo de la disco\n", id, VIPSTR(isvip));
	pthread_cond_broadcast(&vaciar);
}

void *client(void *arg)
{
	struct cliente* clientes = (struct cliente*) arg;
	int id = clientes->id;
	int isvip = clientes->isvip;

    if ( isvip )
        enter_vip_client(id);
    else{
        enter_normal_client(id);
	}
    dance(id, isvip);
    disco_exit(id, isvip);

	return NULL;
}

void loadClientes(struct cliente** clientes, char* path, int* num_clientes)
{
	int ticket = 0;
	char buffer[3];

	FILE* fd = fopen(path, "r");
	if (fd == NULL){
		perror("Open");
		exit(EXIT_FAILURE);
	}
	
	fgets(buffer, 3, fd);
	fgetc(fd);

	ticket = atoi(buffer);
	*clientes = malloc(sizeof(struct cliente) * ticket);
	char buffCliente[3];
	int i = 0;
	while (fgets(buffCliente, 3, fd) != NULL)
	{
		if (buffCliente[0] != '\n')
		{
			(*clientes)[i].id = i;
			(*clientes)[i].isvip = atoi(buffCliente);
			//(*clientes)[i].isvip = ((buffCliente[0] == '1') ? 1 : 0);
			i++;
		}	
	}
	*num_clientes = ticket;
}

int main(int argc, char *argv[])
{
	int i;
	pthread_mutex_init(&m, NULL);

	if (argc != 2){
		printf("Usage: %s path", argv[0]);
		exit(EXIT_FAILURE);
	}

	struct cliente* clientes;
	int num_clientes;
	int hayVips = 1;
	loadClientes(&clientes, argv[1], &num_clientes);

	pthread_t* threads = malloc(sizeof(pthread_t) * num_clientes);

	// Primero pasan los vip por orden de llegada, después los no vip
	for (i = 0; i < num_clientes; ++i){
		if (clientes[i].isvip){
			// (hilo, atributo, función, argumento)
			pthread_create(&threads[i], NULL, client, &clientes[i]);
		}
		else if (!clientes[i].isvip && !hayVips){
			pthread_create(&threads[i], NULL, client, &clientes[i]);
		}
		else if (i == num_clientes - 1 && hayVips){
			i = -1;
			hayVips = 0;
		}
	}

	for (i = 0; i < num_clientes; ++i){
		pthread_join(threads[i], NULL);
	}

	return 0;
}
