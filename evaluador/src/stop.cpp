#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <semaphore.h>
#include <string>
#include "stop.h"
#include "elements.h"
using namespace std;

void Stop::borrar(string memseg){
    string open = "/" + memseg;
    string nombres[3] = {"Sangre","Piel","Ditritos"};
    int i;
    int mem = shm_open(open.c_str(), O_RDWR, 0660);
    struct Header *header =(struct Header *) mmap(NULL, sizeof(struct Header), PROT_READ | PROT_WRITE, MAP_SHARED, mem, 0);
    i = header->i;

    munmap((void *) header, sizeof(struct Header));

    string nombreMemoriaSangre = "/" + memseg + "Sangre";
    string nombreMemoriaPiel = "/" + memseg + "Piel";
    string nombreMemoriaDitritos = "/" + memseg + "Ditritos";
    string nombreSemaforoLlenos = memseg + "Llenos";
    string nombreSemaforoVacios = memseg + "Vacios";
    string nombreSemaforoMutex = memseg + "Mutex";
    string nombreSemaforoSangre = memseg + "Sangre";
    string nombreSemaforoPiel = memseg + "Piel";
    string nombreSemaforoDitritos = memseg + "Ditritos";
    string nombreSemaforoSalidaVacio = memseg +"LlenosSalida";
    string nombreSemaforoSalidaLLeno = memseg +"MutexSalida";
    string nombreSemaforoSalidaMutex = memseg +"VaciosSalida";

    for (int j=0; j<i; j++)
    {
        string nombreLlenos = nombreSemaforoLlenos + to_string(j);
        string nombreVacios = nombreSemaforoVacios + to_string(j);
        string nombreMutex = nombreSemaforoMutex + to_string(j);
        sem_unlink(nombreLlenos.c_str());
        sem_unlink(nombreVacios.c_str());
        sem_unlink(nombreMutex.c_str());
    }

    for (int j=0; j<3; j++)
    {
        string nombreLlenos = memseg + nombres[j] + "Llenos";
        string nombreVacios = memseg + nombres[j] + "Vacios";
        string nombreMutex = memseg + nombres[j] + "Mutex";
        sem_unlink(nombreLlenos.c_str());
        sem_unlink(nombreVacios.c_str());
        sem_unlink(nombreMutex.c_str());
    }
    string tmp = "/evaluato",tmp2="pruebaLl";
    shm_unlink(tmp.c_str());
    sem_unlink(tmp2.c_str());
    sem_unlink(nombreSemaforoSangre.c_str());
    sem_unlink(nombreSemaforoPiel.c_str());
    sem_unlink(nombreSemaforoDitritos.c_str());
    sem_unlink(nombreSemaforoSalidaLLeno.c_str());
    sem_unlink(nombreSemaforoSalidaVacio.c_str());
    sem_unlink(nombreSemaforoSalidaMutex.c_str());
    shm_unlink(open.c_str());
    shm_unlink(nombreMemoriaSangre.c_str());
    shm_unlink(nombreMemoriaPiel.c_str());
    shm_unlink(nombreMemoriaDitritos.c_str());
}
