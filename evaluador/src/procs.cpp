#include <pthread.h>
#include <cstring>
#include <iostream>
#include <cstdlib>
#include "procs.h"
#include "procesando.h"
#include <ctime>
#include <unistd.h>

using namespace std;

void *wrapperProcesar(void *arg)
{
    Procesando procesaraux;
    struct EstructuraHilo *est = (struct EstructuraHilo *)arg;
    procesaraux.procesar(est->name, est->i);
    
    return NULL;
}

void *wrapperProcesando(void *arg){

    Procesando procesaraux;
    struct EstructuraHilo *est2 = (struct EstructuraHilo *)arg;
    procesaraux.procesado(est2->name, est2->i);
    return NULL;
    
}

void Procs::createThreads(int i, string nomSeg)
{
    pthread_t hiloEntrada[i];
    pthread_t hiloIntermedio[3];
    struct EstructuraHilo estructura;
    struct EstructuraHilo estructura2;
    strcpy(estructura.name,nomSeg.c_str());
    strcpy(estructura2.name,nomSeg.c_str());

    for (int j = 0; j < i; j++)
    {
        estructura.i = j;
        pthread_create(&hiloEntrada[j], NULL, wrapperProcesar, (void *) &estructura);
        sleep(1);
    }

    for (int k = 0; k < 3; k++){
        estructura2.i = k;
        pthread_create(&hiloIntermedio[k], NULL, wrapperProcesando, (void *) &estructura2);  
        sleep(1);  
    }
}