#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <iostream>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include "elements.h"
#include <string>
#include "reg.h"

using namespace std;

void Reg::registrar(string nomseg, int bandeja, char tipomuestra, int cantmuestra, int ident, int ie){
    int mem = shm_open(nomseg.c_str(), O_RDWR, 0660);
    //cout << (int *)mem++ << endl;
    //int shmID;
    //shmID = shmget(,0);
    if (mem < 0){
        cerr << "Error abriendo la memoria compartida: "
	    << errno << strerror(errno) << endl;
        exit(1);
    }
    void *dir;

    if ((dir = mmap(NULL, sizeof(struct Entrada), PROT_READ | PROT_WRITE, MAP_SHARED, mem, 0)) == MAP_FAILED) {
        cerr << "Error mapeando la memoria compartida: "
        << errno << strerror(errno) << endl;
        exit(1);
    }

    struct Entrada *entrada = (struct Entrada* ) dir;
    entrada->bandEntrada = bandeja;
    entrada->cantidad = cantmuestra;
    entrada->tipo = tipomuestra;
    entrada->ident = ident;
    cout << dir << endl;
    memcpy(dir, &entrada, sizeof(struct Entrada));
    //cout << dir << " " << entrada->bandEntrada << endl;
}
