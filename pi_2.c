#include <pthread.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
// #include <time.h>

size_t* numprocs;
int* vetor;
const int tamanho=1024*1024*1024/4;


void* preenche(void * id){
	long ID=(long)id;
	printf("ID Thread %ld\n", ID);
	long i;
	pid_t tid;
	tid = syscall(SYS_gettid);
	printf("Thread: %ld(%d) - Process ID: %5d Pai:%5d\n",ID,tid,getpid(),getppid());

	int deslocamento = tamanho/(*numprocs);
	int inicio = ID * deslocamento;
	int fim = inicio + deslocamento;
	// srand(time(0));
	for (i = inicio; (i <= fim) && (i < tamanho) ; i++) { //1bi
		// vetor[i] = rand()%100;
		vetor[i] = i; //Para testar a consistencia dos dados
	}
	pthread_exit(NULL);
}


void* calcula(void * id){
	long ID=(long)id;
	// printf("ID Thread %ld\n", ID);
	long i;
	long valor = 0;
	pid_t tid;
	tid = syscall(SYS_gettid);

	int deslocamento = tamanho/(*numprocs);
	int inicio = ID * deslocamento;
	int fim = inicio + deslocamento;

	for (i = inicio; (i < fim) && (i < tamanho) ; i++) { //1bi
		valor += vetor[i];
	}
	printf("Valor calculado pela Thread %ld(%d)= %ld\n",ID, tid, valor);
	pthread_exit(valor);
}



int main(int argc, char *argv[]){
	printf("Numero de processos=%ld\n", atol(argv[1]));
	numprocs = malloc(sizeof(unsigned int)*1);
	*numprocs = atol(argv[1]);
	vetor = (int*)malloc(sizeof(int)*tamanho);
	pid_t tid;
	tid = syscall(SYS_gettid);
	long *soma = (long*)malloc(sizeof(long)*(int)(*numprocs));
	long long int final = 0;

	
	printf("Thread: (%d) - Process ID: %5d Pai:%5d\n",tid,getpid(),getppid());
	//Idenficador de cada thread
	pthread_t handles[(*numprocs)]; 
	/*Carrega os atributos padrões para criação
	 das threads. Dentre os atributos, estão:
	prioridade no escalonamento e tamanho da pilha.*/
	pthread_attr_t attr; 
	pthread_attr_init(&attr);
	//Cria as threads usando os atributos carregados.
	long i;
	for (i = 0; i < (*numprocs); i++){
		pthread_create(&handles[i], &attr, preenche, (void*)i);
	}
	// Espera todas as threads terminarem.
	for (i = 0; i != (*numprocs); ++i)
		pthread_join(handles[i], NULL); //NULL -> parâmetro de retorno
	
	for (i = 0; i < (*numprocs); i++){
		pthread_create(&handles[i], &attr, calcula, (void*)i);
	}
	
	for (i = 0; i != (*numprocs); ++i)
		pthread_join(handles[i], &soma[i]); 
	
	for (i = 0; i != (*numprocs); ++i){
		final += soma[i];
		printf("\nResultado da soma retornado pela thread %ld = %ld\n", i, soma[i]);
	}

	printf("Resultado da soma do vetor = %lld\n", final);

	return 0;
}
