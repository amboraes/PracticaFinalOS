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
    int mem = shm_open(nomseg.c_str(), O_RDWR, 0660);
    //int shmID;
    //shmID = shmget(,0);
    if (mem < 0){
        cerr << "Error abriendo la memoria compartida: "
	    << errno << strerror(errno) << endl;
        exit(1);
    }
}
string Ctrl::esperando(string nomseg){
    int mem = shm_open(nomseg.c_str(), O_RDWR, 0660);
    //int shmID;
    //shmID = shmget(,0);
    if (mem < 0){
        cerr << "Error abriendo la memoria compartida: "
	    << errno << strerror(errno) << endl;
        exit(1);
    }
}
string Ctrl::terminados(string nomseg){
    int mem = shm_open(nomseg.c_str(), O_RDWR, 0660);
    //int shmID;
    //shmID = shmget(,0);
    if (mem < 0){
        cerr << "Error abriendo la memoria compartida: "
	    << errno << strerror(errno) << endl;
        exit(1);
    }
}
string Ctrl::reactivos(string nomseg){
    int mem = shm_open(nomseg.c_str(), O_RDWR, 0660);
    //int shmID;
    //shmID = shmget(,0);
    if (mem < 0){
        cerr << "Error abriendo la memoria compartida: "
	    << errno << strerror(errno) << endl;
        exit(1);
    }
}
string Ctrl::all(string nomseg){
   int mem = shm_open(nomseg.c_str(), O_RDWR, 0660);
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
