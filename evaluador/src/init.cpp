#include <fcntl.h>
#include <cerrno>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <semaphore.h>
#include <iostream>
#include "elements.h"
#include "init.h"
#include <cstdlib>
#include <cstring>
#include <map>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "procs.h"

using namespace std;

    //Metodo que inicializa todos los semaforoes, memorias e hilos correspondientes
    void Init::inicializar (int i, int pos, int entradasCola, string nombreSeg,int reactSangre, int reactDetritos, int  reactPiel, int sizeInternas){
        //Se crean los arreglosde los diferentes semaforos que se van a crear
        sem_t **arraySemLlenos = new sem_t*[i];
        sem_t **arraySemVacios = new sem_t*[i];
        sem_t **arraySemMutex = new sem_t*[i];
        sem_t **arraySemColasIntermediasLlenos = new sem_t*[3];
        sem_t **arraySemColasIntermediasVacios = new sem_t*[3];
        sem_t **arraySemColasIntermediasMutex = new sem_t*[3];

        string nombres[3] = {"Sangre","Piel","Ditritos"};
        //nombres de las memorias que serán creadas para las colas intermedias
        string nombreMemoriaSangre = nombreSeg + "Sangre";
        string nombreMemoriaPiel = nombreSeg + "Piel";
        string nombreMemoriaDitritos = nombreSeg + "Ditritos";

        //nombres de lose semaforos que serán creados
        string nombreSemaforoLlenos = nombreSeg + "Llenos";
        string nombreSemaforoVacios = nombreSeg + "Vacios";
        string nombreSemaforoMutex = nombreSeg + "Mutex";

        string nombreSemaforoSangre = nombreSeg + "Sangre";
        string nombreSemaforoPiel = nombreSeg + "Piel";
        string nombreSemaforoDitritos = nombreSeg + "Ditritos";

        string nombreSemaforoLlenosSalida = nombreSeg + "Lsalida";
        string nombreSemaforoVaciosSalida = nombreSeg + "Vsalida";
        string nombreSemaforoMutexSalida = nombreSeg + "Msalida";

        //ciclo para los semaforos llenos, vacios y mutex de cada cola de entrada, el formato es: nombre del segmento + (Llenos|Vacios|Mutex) + número de la cola
        for (int j=0; j<i; j++)
        {
            string nombreLlenos = nombreSemaforoLlenos + to_string(j);
            string nombreVacios = nombreSemaforoVacios + to_string(j);
            string nombreMutex = nombreSemaforoMutex + to_string(j);
            arraySemLlenos[j] = sem_open(nombreLlenos.c_str(), O_CREAT | O_EXCL, 0660, 0);
            arraySemVacios[j] = sem_open(nombreVacios.c_str(), O_CREAT | O_EXCL, 0660, pos);
            arraySemMutex[j] = sem_open(nombreMutex.c_str(), O_CREAT | O_EXCL, 0660, 1);
        }
        //ciclo para los semaforos llenos, vacios y mutex de cada cola intermedia, el formato es: nombre del segmento  + (Sangre|Piel|Ditritos) + (Llenos|Vacios|Mutex)
        for (int j=0; j<3; j++)
        {
            string nombreLlenos = nombreSeg + nombres[j] + "Llenos";
            string nombreVacios = nombreSeg + nombres[j] + "Vacios";
            string nombreMutex = nombreSeg + nombres[j] + "Mutex";
            arraySemColasIntermediasLlenos[i] = sem_open(nombreLlenos.c_str(), O_CREAT | O_EXCL, 0660, 0);
            arraySemColasIntermediasVacios[i] = sem_open(nombreVacios.c_str(), O_CREAT | O_EXCL, 0660, sizeInternas);
            arraySemColasIntermediasMutex[i] = sem_open(nombreMutex.c_str(), O_CREAT | O_EXCL, 0660, 1);
        }
        //Se crean los semaforos de los reactivos
        sem_t *sangre = sem_open(nombreSemaforoSangre.c_str(), O_CREAT | O_EXCL, 0660, reactSangre);
        sem_t *piel = sem_open(nombreSemaforoPiel.c_str(), O_CREAT | O_EXCL, 0660, reactPiel);
        sem_t *ditritos  = sem_open(nombreSemaforoDitritos.c_str(), O_CREAT | O_EXCL, 0660, reactDetritos);

        //Se crean los semaforos llenos, vacios y mutex de las colas de salida  
        sem_t *llenos = sem_open(nombreSemaforoLlenosSalida.c_str(), O_CREAT | O_EXCL, 0660, 0);
        sem_t *vacios = sem_open(nombreSemaforoVaciosSalida.c_str(), O_CREAT | O_EXCL, 0660, entradasCola);
        sem_t *mutex  = sem_open(nombreSemaforoMutexSalida.c_str(), O_CREAT | O_EXCL, 0660, 1);

        //Se crea la memoria principal
        int mem = shm_open(nombreSeg.c_str(), O_RDWR | O_CREAT | O_EXCL, 0660);

        //Se crean las memorias para las colas intermedias
        int memSangre = shm_open(nombreMemoriaSangre.c_str(), O_RDWR | O_CREAT | O_EXCL, 0660);
        int memPiel = shm_open(nombreMemoriaPiel.c_str(), O_RDWR | O_CREAT | O_EXCL, 0660);
        int memDitritos = shm_open(nombreMemoriaDitritos.c_str(), O_RDWR | O_CREAT | O_EXCL, 0660);

        if (mem < 0){
            cerr << "Error creando la memoria compartida: "
	        << errno << strerror(errno) << endl;
            exit(1);
        }

        if (ftruncate(mem, ((sizeof(struct Entrada)*i*pos)+(sizeof(struct Salida)*entradasCola))) != 0){
            cerr << "Error creando la memoria compartida: "
	        << errno << strerror(errno) << endl;
            exit(1);

        }

        ftruncate(memSangre, (sizeof(struct Entrada)*sizeInternas));
        ftruncate(memPiel, (sizeof(struct Entrada)*sizeInternas));
        ftruncate(memDitritos, (sizeof(struct Entrada)*sizeInternas));

        void *dir;

        if ((dir = mmap(NULL, sizeof(struct Header), PROT_READ | PROT_WRITE, MAP_SHARED,
                mem, 0)) == MAP_FAILED) {
            cerr << "Error mapeando la memoria compartida: "
            << errno << strerror(errno) << endl;
            exit(1);
        }

        //Creacion de la estructura Header el cual tiene los parametros pasados por el usuario en init, los cuales serán usados es otras llamdas
        struct Header *header = (struct Header *) dir;
        header->i = i;
        header->ie = pos;
        header->oe = entradasCola;
        header->q = sizeInternas;
        header->s = reactPiel;
        header->d = reactDetritos;
        header->b = reactSangre;
        strcpy(header->name, nombreSeg.c_str());

        munmap((void *)header, sizeof(struct Header));

        //Creacion de los hilos
        Procs p;
        p.createThreads(i, nombreSeg); 

    }
