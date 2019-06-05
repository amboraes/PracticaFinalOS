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
#include "bandejas.h"

using namespace std;

    void Init::inicializar (int i, int pos, int entradasCola, string nombreSeg,int reactSangre, int reactDetritos, int  reactPiel, int sizeInternas){

        string nombreSemaforoLlenos = nombreSeg + "Llenos";
        string nombreSemaforoVacios = nombreSeg + "Vacios";
        string nombreSemaforoMutex = nombreSeg + "Mutex";
        string nombreSemaforoSangre = nombreSeg + "Sangre";
        string nombreSemaforoPiel = nombreSeg + "Piel";
        string nombreSemaforoDitritos = nombreSeg + "Ditritos";

        sem_t *sangre = sem_open(nombreSemaforoSangre.c_str(), O_CREAT | O_EXCL, 0660, reactSangre);
        sem_t *piel = sem_open(nombreSemaforoPiel.c_str(), O_CREAT | O_EXCL, 0660, reactPiel);
        sem_t *ditritos  = sem_open(nombreSemaforoDitritos.c_str(), O_CREAT | O_EXCL, 0660, reactDetritos);

        sem_t *llenos  = sem_open(nombreSemaforoLlenos.c_str(), O_CREAT | O_EXCL, 0660, 0);
        sem_t *vacios  = sem_open(nombreSemaforoVacios.c_str(), O_CREAT | O_EXCL, 0660, (i*pos));
        sem_t *mutex  = sem_open(nombreSemaforoMutex.c_str(), O_CREAT | O_EXCL, 0660, 1);

        int mem = shm_open(nombreSeg.c_str(), O_RDWR | O_CREAT | O_EXCL, 0660);

        if (mem < 0){
            cerr << "Error creando la memoria compartida: "
	        << errno << strerror(errno) << endl;
            exit(1);
        }

        if (ftruncate(mem, ((sizeof(struct Entrada)*i*pos)+sizeof(struct Salida)+entradasCola)) != 0){
            cerr << "Error creando la memoria compartida: "
	        << errno << strerror(errno) << endl;
            exit(1);
        }
        
        void *dir;

        if ((dir = mmap(NULL, sizeof(struct Header), PROT_READ | PROT_WRITE, MAP_SHARED,
                mem, 0)) == MAP_FAILED) {
            cerr << "Error mapeando la memoria compartida: "
            << errno << strerror(errno) << endl;
            exit(1);
        }
        cout << dir << endl;
        cout << (i*pos) << endl;
        struct Header *header = (struct Header *) dir;
        header->i = i;
        header->ie = pos;
        header->oe = entradasCola;
        header->q = sizeInternas; 
        header->s = reactPiel;
        header->d = reactDetritos;
        header->b = reactSangre;
        strcpy(header->name, nombreSeg.c_str());

        //Bandejas bandejas;
        //bandejas.dividirMemoria(nombreSeg);

        close(mem);

        //return dir;
        //return EXIT_SUCCESS;*/
    }
