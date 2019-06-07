#include <pthread.h>
#include <cstring>
#include "procesando.h"

using namespace std;

class Procs
{

    Procs *Procs::instance = 0;
    pthread_t hiloprocesar[];

    Procs::Procs(int i, string nomSeg)
    {
        pthread_t temp[i + 3];
        for (int j = 0; j < i; j++)
        {
            pthread_create(&hiloprocesar[0], NULL, wrapperProcesar, &estructura);
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
        procesaraux.procesado(est->name);
        return NULL;
    }
}