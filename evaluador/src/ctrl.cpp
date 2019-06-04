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
#include "ctrl.h"

using namespace std;


string Ctrl::procesando(string nomseg){
    string open = "/" + nomseg;
    int mem = shm_open(open.c_str(), O_RDWR, 0660);
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
    for (int i = 0; i < 10; i++)
    {
        cout << "entro al processing" << endl;
        cout << entrada << endl;
    }
    
}
string Ctrl::esperando(string nomseg){
    string open = "/" + nomseg;
    int mem = shm_open(open.c_str(), O_RDWR, 0660);
    //int shmID;
    //shmID = shmget(,0);
    if (mem < 0){
        cerr << "Error abriendo la memoria compartida: "
	    << errno << strerror(errno) << endl;
        exit(1);
    }
}
string Ctrl::terminados(string nomseg){
    string open = "/" + nomseg;
    int mem = shm_open(open.c_str(), O_RDWR, 0660);
    //int shmID;
    //shmID = shmget(,0);
    if (mem < 0){
        cerr << "Error abriendo la memoria compartida: "
	    << errno << strerror(errno) << endl;
        exit(1);
    }
}
string Ctrl::reactivos(string nomseg){
    string open = "/" + nomseg;
    int mem = shm_open(open.c_str(), O_RDWR, 0660);
    //int shmID;
    //shmID = shmget(,0);
    if (mem < 0){
        cerr << "Error abriendo la memoria compartida: "
	    << errno << strerror(errno) << endl;
        exit(1);
    }
}
string Ctrl::all(string nomseg){
    string open = "/" + nomseg;
    int mem = shm_open(open.c_str(), O_RDWR, 0660);
    //int shmID;
    //shmID = shmget(,0);
    if (mem < 0){
        cerr << "Error abriendo la memoria compartida: "
	    << errno << strerror(errno) << endl;
        exit(1);
    }
}
void Ctrl::actualizar(string tipomuestra, int valormuestra){

}
