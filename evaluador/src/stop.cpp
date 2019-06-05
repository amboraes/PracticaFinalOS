#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <semaphore.h>
#include <string>
#include "stop.h"
using namespace std;

void Stop::borrar(string memseg){
    string open = "/" + memseg;
    string nombreSemaforoLlenos = memseg + "Llenos";
    string nombreSemaforoVacios = memseg + "Vacios";
    string nombreSemaforoMutex = memseg + "Mutex";
    string nombreSemaforoSangre = memseg + "Sangre";
    string nombreSemaforoPiel = memseg + "Piel";
    string nombreSemaforoDitritos = memseg + "Ditritos";

    sem_unlink(nombreSemaforoLlenos.c_str());
    sem_unlink(nombreSemaforoVacios.c_str());
    sem_unlink(nombreSemaforoMutex.c_str());
    sem_unlink(nombreSemaforoSangre.c_str());
    sem_unlink(nombreSemaforoPiel.c_str());
    sem_unlink(nombreSemaforoDitritos.c_str());
    shm_unlink(open.c_str());
}