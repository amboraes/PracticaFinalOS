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
        sem_t **arraySemLlenos = new sem_t*[i];
        sem_t **arraySemVacios = new sem_t*[i];
        sem_t **arraySemMutex = new sem_t*[i];
        
        string nombreMemoriaSangre = nombreSeg + "Sangre";
        string nombreMemoriaPiel = nombreSeg + "Piel";
        string nombreMemoriaDitritos = nombreSeg + "Ditritos";

        string nombreSemaforoLlenos = nombreSeg + "Llenos";
        string nombreSemaforoVacios = nombreSeg + "Vacios";
        string nombreSemaforoMutex = nombreSeg + "Mutex";

        for (int j=0; j<i; j++)
        {
            string nombreLlenos = nombreSemaforoLlenos + to_string(j);
            string nombreVacios = nombreSemaforoVacios + to_string(j);
            string nombreMutex = nombreSemaforoMutex + to_string(j);
            arraySemLlenos[i] = sem_open(nombreLlenos.c_str(), O_CREAT | O_EXCL, 0660, 0);
            arraySemVacios[i] = sem_open(nombreVacios.c_str(), O_CREAT | O_EXCL, 0660, pos);
            arraySemMutex[i] = sem_open(nombreMutex.c_str(), O_CREAT | O_EXCL, 0660, 1);
        }
        
        string nombreSemaforoSangre = nombreSeg + "Sangre";
        string nombreSemaforoPiel = nombreSeg + "Piel";
        string nombreSemaforoDitritos = nombreSeg + "Ditritos";

        sem_t *sangre = sem_open(nombreSemaforoSangre.c_str(), O_CREAT | O_EXCL, 0660, reactSangre);
        sem_t *piel = sem_open(nombreSemaforoPiel.c_str(), O_CREAT | O_EXCL, 0660, reactPiel);
        sem_t *ditritos  = sem_open(nombreSemaforoDitritos.c_str(), O_CREAT | O_EXCL, 0660, reactDetritos);

        int mem = shm_open(nombreSeg.c_str(), O_RDWR | O_CREAT | O_EXCL, 0660);

        int memSangre = shm_open(nombreMemoriaSangre.c_str(), O_RDWR | O_CREAT | O_EXCL, 0660);
        int memPiel = shm_open(nombreMemoriaPiel.c_str(), O_RDWR | O_CREAT | O_EXCL, 0660);
        int memDitritos = shm_open(nombreMemoriaDitritos.c_str(), O_RDWR | O_CREAT | O_EXCL, 0660);

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
