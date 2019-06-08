#include <pthread.h>
#include <cstring>
#include "procs.h"
#include "procesando.h"

using namespace std;

Procs* instance = 0;
pthread_t hiloprocesar[];


Procs::Procs(int i, string nomSeg)
{
    pthread_t temp[i + 3];
    struct EstructuraHilo estructura;
    strcpy(estructura.name,nomSeg.c_str());

    for (int j = 0; j < i; j++)
    {
        pthread_create(&hiloprocesar[j], NULL, wrapperProcesar, &estructura);
    }

    for (int k = i; k < (i + 3); k++){
        pthread_create(&hiloprocesar[k], NULL, wrapperProcesar, &estructura);    
    }
}

Procs *Procs::getInstance(int i, string nomSeg)
{
    if (instance == 0)
    {
        instance = new Procs(i, nomSeg);
    }

    return instance;
}

void *wrapperProcesar(void *arg)
{
    Procesando procesaraux;
    struct EstructuraHilo *est = (struct EstructuraHilo *)arg;
    //cout << est->i <<endl;
    procesaraux.procesar(est->name, est->i);
    
    return NULL;
}

void *wrapperProcesando(void *arg){

    Procesando procesaraux;
    struct EstructuraHilo *est = (struct EstructuraHilo *)arg;
    procesaraux.procesado(est->name);
    return NULL;
    
}
