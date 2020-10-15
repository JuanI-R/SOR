#include <stdio.h>      // libreria estandar
#include <stdlib.h>     // para usar exit y funciones de la libreria standard
#include <string.h>
#include <pthread.h>    // para usar threads
#include <semaphore.h>  // para usar semaforos
#include <unistd.h>


#define LIMITE 50
pthread_mutex_t sem_salar;
pthread_mutex_t sem_cocinarCarne;
pthread_mutex_t sem_hornear;
//pthread_mutex_t sem_validar;

//creo estructura de semaforos 
struct semaforos {
    sem_t sem_mezclar;
    sem_t sem_salar;
    sem_t sem_armar;
    //sem_t sem_hornear;
    sem_t sem_armar_medallones;
    sem_t sem_cocinar;
    sem_t sem_validar;
	//poner demas semaforos aqui
};

//creo los pasos con los ingredientes
struct paso {
   char accion [LIMITE];
   char ingredientes[4][LIMITE];
   
};

//creo los parametros de los hilos 
struct parametro {
 int equipo_param;
 struct semaforos semaforos_param;
 struct paso pasos_param[8];
};

//funcion para imprimir las acciones y los ingredientes de la accion
void* imprimirAccion(void *data, char *accionIn) {
    FILE * LOG = fopen ("log.txt","a");
    fseek (LOG, 0, SEEK_END);

	struct parametro *mydata = data;
	//calculo la longitud del array de pasos 
	int sizeArray = (int)( sizeof(mydata->pasos_param) / sizeof(mydata->pasos_param[0]));
	//indice para recorrer array de pasos 
	int i;
	for(i = 0; i < sizeArray; i ++){
		//pregunto si la accion del array es igual a la pasada por parametro (si es igual la funcion strcmp devuelve cero)
		if(strcmp(mydata->pasos_param[i].accion, accionIn) == 0){
		fprintf(LOG,"\tEquipo %d - accion %s \n " , mydata->equipo_param, mydata->pasos_param[i].accion);
        printf("\tEquipo %d - accion %s \n " , mydata->equipo_param, mydata->pasos_param[i].accion);
		//calculo la longitud del array de ingredientes
		int sizeArrayIngredientes = (int)( sizeof(mydata->pasos_param[i].ingredientes) / sizeof(mydata->pasos_param[i].ingredientes[0]) );
		//indice para recorrer array de ingredientes
		int h;
        fprintf(LOG,"\tEquipo %d -----------ingredientes : ----------\n",mydata->equipo_param); 
		printf("\tEquipo %d -----------ingredientes : ----------\n",mydata->equipo_param); 
			for(h = 0; h < sizeArrayIngredientes; h++) {
				//consulto si la posicion tiene valor porque no se cuantos ingredientes tengo por accion 
				if(strlen(mydata->pasos_param[i].ingredientes[h]) != 0) {
                            fprintf(LOG,"\tEquipo %d ingrediente  %d : %s \n",mydata->equipo_param,h,mydata->pasos_param[i].ingredientes[h]);
							printf("\tEquipo %d ingrediente  %d : %s \n",mydata->equipo_param,h,mydata->pasos_param[i].ingredientes[h]);
				}
			}
		}
	}
    fclose(LOG);
}

//funcion para tomar de ejemplo
void* cortar(void *data) {
	//creo el nombre de la accion de la funcion 
	char *accion = "cortar";
	//creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero). 
	struct parametro *mydata = data;
	//llamo a la funcion imprimir le paso el struct y la accion de la funcion
	imprimirAccion(mydata,accion);
	//uso sleep para simular que que pasa tiempo
	usleep( 20000 );
	//doy la señal a la siguiente accion (cortar me habilita mezclar)
    sem_post(&mydata->semaforos_param.sem_mezclar);
	
    pthread_exit(NULL);
}
void* cortarLechuga(void *data) {
	//creo el nombre de la accion de la funcion 
	char *accion = "cortar Lechuga";
	//creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero). 
	struct parametro *mydata = data;
	//llamo a la funcion imprimir le paso el struct y la accion de la funcion
	imprimirAccion(mydata,accion);
	//uso sleep para simular que que pasa tiempo
	usleep( 20000 );
	//doy la señal a la siguiente accion (cortar me habilita mezclar)
	
    pthread_exit(NULL);
}

void* mezclar(void *data) {
    
	//creo el nombre de la accion de la funcion 
	char *accion = "mezclar";
	//creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero). 
	struct parametro *mydata = data;
    sem_wait(&mydata->semaforos_param.sem_mezclar);
	//llamo a la funcion imprimir le paso el struct y la accion de la funcion
	imprimirAccion(mydata,accion);
	//uso sleep para simular que que pasa tiempo
	usleep( 20000 );
	//doy la señal a la siguiente accion (cortar me habilita mezclar)
    sem_post(&mydata->semaforos_param.sem_salar);
	
    pthread_exit(NULL);
}
void* salar(void *data) {

    

	//creo el nombre de la accion de la funcion 
	char *accion = "salar";
	//creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero). 
	struct parametro *mydata = data;
    sem_wait(&mydata->semaforos_param.sem_salar);
    pthread_mutex_lock(&sem_salar);
	//llamo a la funcion imprimir le paso el struct y la accion de la funcion
	imprimirAccion(mydata,accion);
	//uso sleep para simular que que pasa tiempo
	usleep( 20000 );
	//doy la señal a la siguiente accion (cortar me habilita mezclar)
    sem_post(&mydata->semaforos_param.sem_armar_medallones);
    pthread_mutex_unlock(&sem_salar);
    
	
    pthread_exit(NULL);
}

void* hornear(void *data) {
    pthread_mutex_lock (&sem_hornear);
	//creo el nombre de la accion de la funcion 
	char *accion = "hornear";
	//creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero). 
	struct parametro *mydata = data;
    
	//llamo a la funcion imprimir le paso el struct y la accion de la funcion
	imprimirAccion(mydata,accion);
	//uso sleep para simular que que pasa tiempo
	usleep( 20000 );
	//doy la señal a la siguiente accion (cortar me habilita mezclar)
	//printf("\tEquipo %d - accion %s \n " , mydata->equipo_param, mydata->pasos_param[5].accion);
	//printf ("%s ", mydata->pasos_param[5].accion);
    pthread_mutex_unlock(&sem_hornear);
	
    pthread_exit(NULL);
}

void* armar_medallones(void *data) {
	//creo el nombre de la accion de la funcion 
	char *accion = "armar medallones";
	//creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero). 
	struct parametro *mydata = data;
    sem_wait(&mydata->semaforos_param.sem_armar_medallones);
	//llamo a la funcion imprimir le paso el struct y la accion de la funcion
	imprimirAccion(mydata,accion);
	//uso sleep para simular que que pasa tiempo
	usleep( 20000 );
	//doy la señal a la siguiente accion (cortar me habilita mezclar)
    sem_post(&mydata->semaforos_param.sem_cocinar);
	
    pthread_exit(NULL);
}


void* cocinar(void *data) {
    
 	pthread_mutex_lock (&sem_cocinarCarne);
	//creo el nombre de la accion de la funcion 
	char *accion = "cocinar";
	//creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero). 
	struct parametro *mydata = data;
    sem_wait(&mydata->semaforos_param.sem_cocinar);
   
	//llamo a la funcion imprimir le paso el struct y la accion de la funcion
	imprimirAccion(mydata,accion);
	//uso sleep para simular que que pasa tiempo
	usleep( 20000 );
	//doy la señal a la siguiente accion (cortar me habilita mezclar)
    pthread_mutex_unlock (&sem_cocinarCarne);
	sem_post(&mydata->semaforos_param.sem_armar);
    pthread_exit(NULL);
}
void* armar(void *data) {
	//creo el nombre de la accion de la funcion 
	char *accion = "armarBurger";
	//creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero). 
	struct parametro *mydata = data;
    sem_wait(&mydata->semaforos_param.sem_armar);
	//llamo a la funcion imprimir le paso el struct y la accion de la funcion
	imprimirAccion(mydata,accion);
	//uso sleep para simular que que pasa tiempo
	usleep( 20000 );
	//doy la señal a la siguiente accion (cortar me habilita mezclar)
    sem_post(&mydata->semaforos_param.sem_validar);
	
    pthread_exit(NULL);
}
void* validar(void *data) {
	//creo el nombre de la accion de la funcion 
	char *accion = "validar";
	//creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero). 
	struct parametro *mydata = data;
    sem_wait(&mydata->semaforos_param.sem_validar);
    FILE * LOG = fopen ("log.txt","a");
    fseek (LOG, 0, SEEK_END);
    
	//llamo a la funcion imprimir le paso el struct y la accion de la funcion
	imprimirAccion(mydata,accion);
	
	fprintf(LOG,"\t---------------EQUIPO %d ES EL GANADOR!!!-------------\n " , mydata->equipo_param);
	fclose(LOG);
	printf("\t---------------EQUIPO %d ES EL GANADOR!!!-------------\n " , mydata->equipo_param);
	exit(-1);
	
	//uso sleep para simular que que pasa tiempo
	//usleep( 20000 );
	//doy la señal a la siguiente accion (cortar me habilita mezclar)
    //pthread_mutex_unlock (&sem_cocinarCarne);
	//sem_post(&mydata->semaforos_param.sem_validar);
	
    pthread_exit(NULL);
}

void* ejecutarReceta(void *i) {
	
	//variables semaforos
	sem_t sem_mezclar;
	//crear variables semaforos aqui
	sem_t sem_salar;
	//sem_t sem_hornear;
    sem_t sem_armar;
    sem_t sem_armar_medallones;
    sem_t sem_cocinar;
    sem_t sem_validar;
	//variables hilos
	pthread_t p1; 
    pthread_t p2;
    pthread_t p3;
    pthread_t p4; 
    pthread_t p5;
    pthread_t p6;
    pthread_t p7; 
    pthread_t p8;
    pthread_t p9;
	//crear variables hilos aqui
	
	//numero del equipo (casteo el puntero a un int)
	int p = *((int *) i);
	FILE * LOG = fopen ("log.txt", "a");
    fseek (LOG, 0, SEEK_END);
    fprintf(LOG, "Ejecutando equipo %d \n", p);
    fclose (LOG);
	printf("Ejecutando equipo %d \n", p);

	//reservo memoria para el struct
	struct parametro *pthread_data = malloc(sizeof(struct parametro));

	//seteo los valores al struct
	
	//seteo numero de grupo
	pthread_data->equipo_param = p;

	//seteo semaforos
	pthread_data->semaforos_param.sem_mezclar = sem_mezclar;
    pthread_data->semaforos_param.sem_salar = sem_salar;
    pthread_data->semaforos_param.sem_armar = sem_armar;
    pthread_data->semaforos_param.sem_armar_medallones = sem_armar_medallones;
    pthread_data->semaforos_param.sem_cocinar = sem_cocinar;
    pthread_data->semaforos_param.sem_validar = sem_validar;

	//setear demas semaforos al struct aqui
	

	//seteo las acciones y los ingredientes (Faltan acciones e ingredientes) ¿Se ve hardcodeado no? ¿Les parece bien?
    strcpy(pthread_data->pasos_param[0].accion, "cortar");
	strcpy(pthread_data->pasos_param[0].ingredientes[0], "ajo");
    strcpy(pthread_data->pasos_param[0].ingredientes[1], "perejil");
 	strcpy(pthread_data->pasos_param[0].ingredientes[2], "cebolla");

	strcpy(pthread_data->pasos_param[1].accion, "mezclar");
	strcpy(pthread_data->pasos_param[1].ingredientes[0], "ajo");
    strcpy(pthread_data->pasos_param[1].ingredientes[1], "perejil");
 	strcpy(pthread_data->pasos_param[1].ingredientes[2], "cebolla");
	strcpy(pthread_data->pasos_param[1].ingredientes[3], "carne picada");

	strcpy(pthread_data->pasos_param[2].accion, "salar");
	strcpy(pthread_data->pasos_param[2].ingredientes[0], "sal");

	strcpy(pthread_data->pasos_param[7].accion, "cocinar");
	strcpy(pthread_data->pasos_param[7].ingredientes[0], "medallon");

    strcpy(pthread_data->pasos_param[3].accion, "Cortar_vegetales");
	strcpy(pthread_data->pasos_param[3].ingredientes[0], "lechuga");
    strcpy(pthread_data->pasos_param[3].ingredientes[1], "tomate");

	strcpy(pthread_data->pasos_param[4].accion, "armar medallones");
	strcpy(pthread_data->pasos_param[4].ingredientes[0], "ajo");
    strcpy(pthread_data->pasos_param[4].ingredientes[1], "perejil");
 	strcpy(pthread_data->pasos_param[4].ingredientes[2], "cebolla");
    strcpy(pthread_data->pasos_param[4].ingredientes[3], "carne");

    strcpy(pthread_data->pasos_param[5].accion, "hornear");
	strcpy(pthread_data->pasos_param[5].ingredientes[0], "pan");

	strcpy(pthread_data->pasos_param[6].accion, "armarBurger");
	//inicializo los semaforos

	sem_init(&(pthread_data->semaforos_param.sem_mezclar),0,0);
    sem_init(&(pthread_data->semaforos_param.sem_salar),0,0);
    sem_init(&(pthread_data->semaforos_param.sem_armar),0,0);
    sem_init(&(pthread_data->semaforos_param.sem_armar_medallones),0,0);
    sem_init(&(pthread_data->semaforos_param.sem_validar),0,0);
    sem_init(&(pthread_data->semaforos_param.sem_cocinar),0,0);
	//inicializar demas semaforos aqui


	//creo los hilos a todos les paso el struct creado (el mismo a todos los hilos) ya que todos comparten los semaforos 
    int rc;
    rc = pthread_create(&p1,                           //identificador unico
                            NULL,                          //atributos del thread
                                cortar,             //funcion a ejecutar
                                pthread_data);                     //parametros de la funcion a ejecutar, pasado por referencia
	//crear demas hilos aqui
	
    rc = pthread_create(&p2,                           //identificador unico
                            NULL,                          //atributos del thread
                                mezclar,             //funcion a ejecutar
                                pthread_data);                     //parametros de la funcion a ejecutar, pasado por referencia
	
    rc = pthread_create(&p3,                           //identificador unico
                            NULL,                          //atributos del thread
                                salar,             //funcion a ejecutar
                                pthread_data);                       //parametros de la funcion a ejecutar, pasado por referencia
	
    rc = pthread_create(&p4,                           //identificador unico
                            NULL,                          //atributos del thread
                                cortarLechuga,             //funcion a ejecutar
                                pthread_data);                     //parametros de la funcion a ejecutar, pasado por referencia
	

	rc = pthread_create(&p5,                           //identificador unico
                            NULL,                          //atributos del thread
                                armar_medallones,             //funcion a ejecutar
                                pthread_data);                       //parametros de la funcion a ejecutar, pasado por referencia
	
    rc = pthread_create(&p6,                           //identificador unico
                            NULL,                          //atributos del thread
                                hornear,             //funcion a ejecutar
                                pthread_data);                       //parametros de la funcion a ejecutar, pasado por referencia
	
    rc = pthread_create(&p7,                           //identificador unico
                            NULL,                          //atributos del thread
                                armar,             //funcion a ejecutar
                                pthread_data);                       //parametros de la funcion a ejecutar, pasado por referencia
	
    rc = pthread_create(&p8,                           //identificador unico
                            NULL,                          //atributos del thread
                                cocinar,             //funcion a ejecutar
                                pthread_data);                       //parametros de la funcion a ejecutar, pasado por referencia
	
    rc = pthread_create(&p9,                           //identificador unico
                            NULL,                          //atributos del thread
                                validar,             //funcion a ejecutar
                                pthread_data);                       //parametros de la funcion a ejecutar, pasado por referencia
	
    
	//join de todos los hilos
	pthread_join (p1,NULL);
    pthread_join (p2,NULL);
    pthread_join (p3,NULL);
    pthread_join (p4,NULL);
    pthread_join (p5,NULL);
    pthread_join (p6,NULL);
    pthread_join (p7,NULL);
    pthread_join (p8,NULL);
    pthread_join (p9,NULL);
	//crear join de demas hilos


	//valido que el hilo se alla creado bien 
    if (rc){
       printf("Error:unable to create thread, %d \n", rc);
       exit(-1);
     }

	 
	//destruccion de los semaforos 
	sem_destroy(&sem_mezclar);
	//destruir demas semaforos 
	
    sem_destroy(&sem_salar);
    //sem_destroy(&sem_hornear);
    sem_destroy(&sem_cocinar);
    sem_destroy(&sem_armar);
    sem_destroy(&sem_armar_medallones);
    sem_destroy(&sem_validar);

	//salida del hilo
	 pthread_exit(NULL);
}


int main ()
{
	//creo los nombres de los equipos 
    FILE * LOG;
	
	LOG = fopen("log.txt","a+");

	fseek (LOG, 0, SEEK_END);
	fclose(LOG);

    pthread_mutex_init (&sem_salar, NULL);
    pthread_mutex_init (&sem_hornear, NULL);
    pthread_mutex_init (&sem_cocinarCarne, NULL);
    //pthread_mutex_init (&sem_validar, NULL);
	int rc;
	int *equipoNombre1 =malloc(sizeof(*equipoNombre1));
	int *equipoNombre2 =malloc(sizeof(*equipoNombre2));
	int *equipoNombre3 =malloc(sizeof(*equipoNombre3));
	*equipoNombre1 = 1;
	*equipoNombre2 = 2;
	*equipoNombre3 = 3;
    
	//creo las variables los hilos de los equipos
	pthread_t equipo3; 
	pthread_t equipo2;
	pthread_t equipo1;
	//inicializo los hilos de los equipos
    rc = pthread_create(&equipo1,                           //identificador unico
                            NULL,                          //atributos del thread
                                ejecutarReceta,             //funcion a ejecutar
                                equipoNombre1); 

    rc = pthread_create(&equipo2,                           //identificador unico
                            NULL,                          //atributos del thread
                                ejecutarReceta,             //funcion a ejecutar
                                equipoNombre2);

    rc = pthread_create(&equipo3,                           //identificador unico
                            NULL,                          //atributos del thread
                                ejecutarReceta,             //funcion a ejecutar
                                equipoNombre3);

   if (rc){
       printf("Error:unable to create thread, %d \n", rc);
       exit(-1);
     } 

	//join de todos los hilos
	pthread_join (equipo3,NULL);
	pthread_join (equipo2,NULL);
	pthread_join (equipo1,NULL);

    pthread_exit(NULL);
}


//Para compilar:   gcc HellsBurgers.c -o ejecutable -lpthread
//Para ejecutar:   ./ejecutable
