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

using namespace std; 


    
    void Init::inicializar ( int i, int pos, int entradasCola, string nombreSeg, int reacSangre, int reactDetritos, int  reactPiel, int sizeInternas){
        
        sem_t *sangre = sem_open("Sangre", O_CREAT | O_EXCL, 0660, reacSangre);
        sem_t *piel = sem_open("Piel", O_CREAT | O_EXCL, 0660, reactPiel);
        sem_t *ditritos  = sem_open("Ditritos", O_CREAT | O_EXCL, 0660, reactDetritos);

        int mem = shm_open(nombreSeg.c_str(), O_RDWR | O_CREAT | O_EXCL, 0660);

        if (mem < 0){
            cerr << "Error creando la memoria compartida: "
	        << errno << strerror(errno) << endl;
            exit(1);
        }

        int j = ((sizeof(struct Entrada)*i*pos)+sizeof(struct Salida)+entradasCola);

        if (ftruncate(mem, ((sizeof(struct Entrada)*i*pos)+sizeof(struct Salida)+entradasCola)) != 0){
            cerr << "Error creando la memoria compartida: "
	        << errno << strerror(errno) << endl;
            exit(1);
        }

        void *dir;

        if ((dir = mmap(NULL, ((sizeof(struct Entrada)*i*pos)+sizeof(struct Salida)+entradasCola), PROT_READ | PROT_WRITE, MAP_SHARED,
                mem, 0)) == MAP_FAILED) {
            cerr << "Error mapeando la memoria compartida: "
            << errno << strerror(errno) << endl;
            exit(1);
        }

        close(mem);

        //return EXIT_SUCCESS;
    }

