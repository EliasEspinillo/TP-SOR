#include <stdio.h>      // libreria estandar
#include <stdlib.h>     // para usar exit y funciones de la libreria standard
#include <string.h>
#include <pthread.h>    // para usar threads
#include <semaphore.h>  // para usar semaforos
#include <unistd.h>


#define LIMITE 50
pthread_mutex_t mutex_salar_auxiliar;
pthread_mutex_t mutex_cocinar;
pthread_mutex_t mutex_escribir=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_ganador=PTHREAD_MUTEX_INITIALIZER;
sem_t sem_cocinar_pan;

FILE *salida;
int ganador;

//creo estructura de semaforos 
struct semaforos {
    sem_t sem_mezclar; //Mezclar huevos con lo cortado
    sem_t sem_salar; //Salar mezcla
    sem_t sem_mezclar_milanesa; //Mezcla la milanesa con el menjunje
    sem_t sem_empanar; //mezclar la milanesa con pan rallado
    sem_t sem_cocinar_milanesa; //Se usa la sarten
    sem_t sem_armar_milanesa;
    sem_t sem_armar_milanesa2;
    sem_t sem_armar_milanesa3;	//Arma con Pan, milanesa, lechuga, tomate y pepino
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
 struct paso pasos_param[9];
};

void* cargarReceta(void *data){
	struct parametro *mydata=data;
	int i=0; //ind
       	int paso=0;
       	int ingrediente=0;
	int p=mydata->equipo_param;
       	char l; //letra
       	char renglon[90];
       	char *token; 	
       	FILE * res;
		
       	if(p==1){
             res=fopen("recetaEq1.txt","r");
       	}else if(p==2){
             res=fopen("recetaEq2.txt","r");
       	}else if(p==3){
             res=fopen("recetaEq3.txt","r");
	}else if(p==4){
             res=fopen("recetaEq4.txt","r");
       }
       	while((l= fgetc(res)) !=EOF){
                    renglon[i]=l;
                    i++;
                    if(l=='\n'){
                      token=strtok(renglon,"|");
                      strcpy(mydata->pasos_param[paso].accion,token);
                      token=strtok(NULL,"|"); 
                      while(token){
                          strcpy(mydata->pasos_param[paso].ingredientes[ingrediente],token);
                          token=strtok(NULL,"|");
                          ingrediente++;}   
                      memset(renglon,'\0',i); 
                      i=0;	
                      ingrediente=0;
                      paso++;
                    }
       }

       	fclose(res);
}


//funcion para imprimir las acciones y los ingredientes de la accion
void* imprimirAccion(void *data, char *accionIn) {
	struct parametro *mydata = data;
	//calculo la longitud del array de pasos 
	int sizeArray = (int)( sizeof(mydata->pasos_param) / sizeof(mydata->pasos_param[0]));
	//indice para recorrer array de pasos 
	int i;
	for(i = 0; i < sizeArray; i ++){
		//pregunto si la accion del array es igual a la pasada por parametro (si es igual la funcion strcmp devuelve cero)
		if(strcmp(mydata->pasos_param[i].accion, accionIn) == 0){
		
		printf("\tEquipo %d - accion %s \n " , mydata->equipo_param, mydata->pasos_param[i].accion);
		fprintf(salida,"\tEquipo %d - accion %s \n " , mydata->equipo_param, mydata->pasos_param[i].accion);
		//calculo la longitud del array de ingredientes
		int sizeArrayIngredientes = (int)( sizeof(mydata->pasos_param[i].ingredientes) / sizeof(mydata->pasos_param[i].ingredientes[0]) );
		//indice para recorrer array de ingredientes
		int h;
		printf("\tEquipo %d -----------ingredientes : ----------\n",mydata->equipo_param);
		fprintf(salida,"\tEquipo %d -----------ingredientes : ----------\n",mydata->equipo_param); 
			for(h = 0; h < sizeArrayIngredientes; h++) {
				//consulto si la posicion tiene valor porque no se cuantos ingredientes tengo por accion 
				if(strlen(mydata->pasos_param[i].ingredientes[h]) != 0) {
							
							printf("\tEquipo %d ingrediente  %d : %s \n",mydata->equipo_param,h,mydata->pasos_param[i].ingredientes[h]);
							fprintf(salida,"\tEquipo %d ingrediente  %d : %s \n",mydata->equipo_param,h,mydata->pasos_param[i].ingredientes[h]);
				}
			}
		}
	}
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
	usleep( 20000000 );
	//doy la señal a la siguiente accion (cortar me habilita mezclar)
    sem_post(&mydata->semaforos_param.sem_mezclar);

	
    pthread_exit(NULL);
}

void* mezclar(void *data) {
	char *accion = "mezclar";	
	struct parametro *mydata = data;
   sem_wait(&mydata->semaforos_param.sem_mezclar);	
	imprimirAccion(mydata,accion);	
	usleep( 2000000 );	
    sem_post(&mydata->semaforos_param.sem_salar);	
    pthread_exit(NULL);
}

void* salar(void *data) {	
	struct parametro *mydata = data;
	sem_wait(&mydata->semaforos_param.sem_salar);
	pthread_mutex_lock(&mutex_salar_auxiliar);	
	char *accion = "salar";	
	imprimirAccion(mydata,accion);	
	usleep( 5000000 );	
    	pthread_mutex_unlock(&mutex_salar_auxiliar);
 	sem_post(&mydata->semaforos_param.sem_mezclar_milanesa);	
    pthread_exit(NULL);
}

void* mezclar_milanesa(void *data) {	
	struct parametro *mydata = data;	
	sem_wait(&mydata->semaforos_param.sem_mezclar_milanesa);  	
	char *accion = "mezclarmilanesa";	
	imprimirAccion(mydata,accion);	
	usleep( 2000000 );	
 	sem_post(&mydata->semaforos_param.sem_empanar);	
    pthread_exit(NULL);
}

void* empanar(void *data) {	
	struct parametro *mydata = data;
	sem_wait(&mydata->semaforos_param.sem_empanar);
	char *accion = "empanar";
	imprimirAccion(mydata,accion);
	usleep( 2000000 );
 	sem_post(&mydata->semaforos_param.sem_cocinar_milanesa);
    pthread_exit(NULL);
}

void* cocinar_milanesa(void *data) {	
	struct parametro *mydata = data;

	sem_wait(&mydata->semaforos_param.sem_cocinar_milanesa);
	pthread_mutex_lock(&mutex_cocinar);

	char *accion = "cocinando";
	imprimirAccion(mydata,accion);
	usleep( 5000000 );

	pthread_mutex_unlock(&mutex_cocinar);
 	sem_post(&mydata->semaforos_param.sem_cocinar_milanesa);
	sem_post(&mydata->semaforos_param.sem_armar_milanesa3);
    pthread_exit(NULL);
}

void* cortar_aditivos(void *data) {

	char *accion = "cortaraditivos";

	struct parametro *mydata = data;

	imprimirAccion(mydata,accion);

	usleep( 2000000 );
	sem_post(&mydata->semaforos_param.sem_armar_milanesa2);
    pthread_exit(NULL);
}

void* cocinar_pan(void *data) {
	
	struct parametro *mydata = data;
	char *accion = "cocinarpan";
	sem_wait(&sem_cocinar_pan);
	imprimirAccion(mydata,accion);
	usleep( 10000000 );	
	sem_post(&sem_cocinar_pan);
	sem_post(&mydata->semaforos_param.sem_armar_milanesa);
    pthread_exit(NULL);
}



void* armar_milanesa(void *data) {
	struct parametro *mydata = data;
	sem_wait(&mydata->semaforos_param.sem_armar_milanesa);
	sem_wait(&mydata->semaforos_param.sem_armar_milanesa2);
	sem_wait(&mydata->semaforos_param.sem_armar_milanesa3);
	pthread_mutex_lock(&mutex_ganador);
	
	if(ganador==0){

	char *accion = "armarmilanesa";
	imprimirAccion(mydata,accion);
	usleep( 10000000 );
	ganador=1;
	int p=mydata->equipo_param;
	pthread_mutex_lock(&mutex_escribir);
	printf("Equipo %d ganador!\n",p);
	fprintf(salida,"Equipo %d ganador!\n",p);
	pthread_mutex_unlock(&mutex_escribir);	}
    pthread_mutex_unlock(&mutex_ganador);
    pthread_exit(NULL);
}


void* ejecutarReceta(void *i) {
	
	//variables semaforos
	sem_t sem_mezclar;
	sem_t sem_salar;
	pthread_mutex_t mutex_salar_auxiliar;
	sem_t sem_mezclar_milanesa;
	sem_t sem_empanar;
	sem_t sem_cocinar_milanesa;
	pthread_mutex_t mutex_cocinar;
	sem_t sem_cocinar_pan;
	sem_t sem_cocinar_pan2;
	sem_t sem_armar_milanesa;
	sem_t sem_armar_milanesa2;
	sem_t sem_armar_milanesa3;
	pthread_mutex_t mutex_pan;

	//crear variables semaforos aqui
	
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
	usleep(2000);
	printf("Ejecutando equipo %d \n", p);

	//reservo memoria para el struct
	struct parametro *pthread_data = malloc(sizeof(struct parametro));

	//seteo los valores al struct
	
	//seteo numero de grupo
	pthread_data->equipo_param = p;

	//seteo semaforos
	pthread_data->semaforos_param.sem_mezclar = sem_mezclar;
	pthread_data->semaforos_param.sem_salar = sem_salar;
	pthread_data->semaforos_param.sem_mezclar_milanesa = sem_mezclar_milanesa;
	pthread_data->semaforos_param.sem_empanar = sem_empanar;
	pthread_data->semaforos_param.sem_cocinar_milanesa = sem_cocinar_milanesa;
	pthread_data->semaforos_param.sem_armar_milanesa= sem_armar_milanesa;
	pthread_data->semaforos_param.sem_armar_milanesa= sem_armar_milanesa2;
	pthread_data->semaforos_param.sem_armar_milanesa= sem_armar_milanesa3;
	//setear demas semaforos al struct aqui
	

	//seteo las acciones y los ingredientes (Faltan acciones e ingredientes) ¿Se ve hardcodeado no? ¿Les parece bien?
     	    //strcpy(pthread_data->pasos_param[0].accion, "cortar");
	    //strcpy(pthread_data->pasos_param[0].ingredientes[0], "ajo");
            //strcpy(pthread_data->pasos_param[0].ingredientes[1], "perejil");

	    //strcpy(pthread_data->pasos_param[1].accion, "mezclar");
	    //strcpy(pthread_data->pasos_param[1].ingredientes[0], "ajo");
            //strcpy(pthread_data->pasos_param[1].ingredientes[1], "perejil");
 	    //strcpy(pthread_data->pasos_param[1].ingredientes[2], "huevo");
	    //strcpy(pthread_data->pasos_param[1].ingredientes[3], "carne");

	cargarReceta(pthread_data);
	
	//inicializo los semaforos

	sem_init(&(pthread_data->semaforos_param.sem_mezclar),0,0);
	sem_init(&(pthread_data->semaforos_param.sem_salar),0,0);
	pthread_mutex_init(&mutex_salar_auxiliar,NULL);
	sem_init(&(pthread_data->semaforos_param.sem_mezclar_milanesa),0,0);
	sem_init(&(pthread_data->semaforos_param.sem_empanar),0,0);
	sem_init(&(pthread_data->semaforos_param.sem_cocinar_milanesa),0,0);
	pthread_mutex_init(&mutex_cocinar,NULL);
	sem_init(&(pthread_data->semaforos_param.sem_armar_milanesa),0,0);
	sem_init(&(pthread_data->semaforos_param.sem_armar_milanesa2),0,0);
	sem_init(&(pthread_data->semaforos_param.sem_armar_milanesa3),0,0);
	pthread_mutex_init(&mutex_pan,NULL);
	//inicializar demas semaforos aqui


	//creo los hilos a todos les paso el struct creado (el mismo a todos los hilos) ya que todos comparten los semaforos 
    int rc;
    rc = pthread_create(&p1,                           //identificador unico
                            NULL,                          //atributos del thread
                                cortar,             //funcion a ejecutar
                                pthread_data);                     //parametros de la funcion a ejecutar, pasado por referencia
	//crear demas hilos aqui


	rc = pthread_create(&p2,NULL,mezclar,pthread_data); 
	rc = pthread_create(&p3,NULL,salar,pthread_data); 
	rc = pthread_create(&p4,NULL,mezclar_milanesa,pthread_data); 
	rc = pthread_create(&p5,NULL,empanar,pthread_data); 
	rc = pthread_create(&p6,NULL,cocinar_milanesa,pthread_data); 	
	rc = pthread_create(&p7,NULL,cortar_aditivos,pthread_data); 	
	rc = pthread_create(&p8,NULL,cocinar_pan,pthread_data); 
	rc = pthread_create(&p9,NULL,armar_milanesa,pthread_data); 
	//rc = pthread_create(&p10,NULL,cocinar_pan2,pthread_data); 	


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
	pthread_join (p9,NULL);
	//pthread_join (p10,NULL);
	//crear join de demas hilos


	//valido que el hilo se alla creado bien 
    if (rc){
       printf("Error:unable to create thread, %d \n", rc);
       exit(-1);
     }

	 
	//destruccion de los semaforos 
	sem_destroy(&sem_mezclar);
	sem_destroy(&sem_salar);
	pthread_mutex_destroy(&mutex_salar_auxiliar);
	sem_destroy(&sem_mezclar_milanesa);
	sem_destroy(&sem_empanar);
	sem_destroy(&sem_cocinar_milanesa);
	pthread_mutex_destroy(&mutex_cocinar);
	sem_destroy(&sem_armar_milanesa);
	sem_destroy(&sem_armar_milanesa2);
	sem_destroy(&sem_armar_milanesa3);
	pthread_mutex_destroy(&mutex_pan);
	//destruir demas semaforos 
	
	//salida del hilo
	 pthread_exit(NULL);
}


int main ()
{
	salida = fopen("salida.txt","w");
	sem_init(&sem_cocinar_pan,0,2);
	//creo los nombres de los equipos 
	int rc;
	int *equipoNombre1 =malloc(sizeof(*equipoNombre1));
	int *equipoNombre2 =malloc(sizeof(*equipoNombre2));
	int *equipoNombre3 =malloc(sizeof(*equipoNombre3));
	int *equipoNombre4 =malloc(sizeof(*equipoNombre4));
//faltan equipos
  
	*equipoNombre1 = 1;
	*equipoNombre2 = 2;
	*equipoNombre3 = 3;
	*equipoNombre4 = 4;

	ganador=0;

	//creo las variables los hilos de los equipos
	pthread_t equipo1; 
	pthread_t equipo2;
	pthread_t equipo3; 
	pthread_t equipo4;
//faltan hilos
  
	//inicializo los hilos de los equipos
    rc = pthread_create(&equipo1,                           //identificador unico
                            NULL,                          //atributos del thread
                                ejecutarReceta,             //funcion a ejecutar
                                equipoNombre1); 

    rc = pthread_create(&equipo2,NULL,ejecutarReceta, equipoNombre2);
    rc = pthread_create(&equipo3,NULL,ejecutarReceta, equipoNombre3);
    rc = pthread_create(&equipo4,NULL,ejecutarReceta, equipoNombre4);

  //faltn inicializaciones


   if (rc){
       printf("Error:unable to create thread, %d \n", rc);
       exit(-1);
     } 

	//join de todos los hilos
	pthread_join (equipo1,NULL);
	pthread_join (equipo2,NULL);
	pthread_join (equipo3,NULL);
	pthread_join (equipo4,NULL);
//.. faltan joins

	sem_destroy(&sem_cocinar_pan);	
	pthread_mutex_destroy(&mutex_ganador);
	pthread_mutex_destroy(&mutex_escribir);
    pthread_exit(NULL);
    fclose(salida);
}


//Para compilar:   gcc subwayArgento.c -o ejecutable -lpthread
//Para ejecutar:   ./ejecutable
