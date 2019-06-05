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
#include "procesando.h"

using namespace std;


string Ctrl::procesando(string nomseg,Procesando proces){    
    //printf("jueputa");
    cout << &proces << endl;
    return proces.procesando();
}

string Ctrl::esperando(string nomseg){
    int n = 0;
    int m = 0;
    int i, ie, oe, q;
    string open = "/" + nomseg;

    int mem = shm_open(open.c_str(), O_RDWR, 0660);
    
    if (mem < 0){
        cerr << "Error abriendo la memoria compartida: "
	    << errno << strerror(errno) << endl;
        exit(1);
    }

    struct Header *header =(struct Header *) mmap(NULL, sizeof(struct Header), PROT_READ | PROT_WRITE, MAP_SHARED, mem, 0);
    i = header->i;
    ie = header->ie;
    oe = header->oe;
    q = header->q;
    
    munmap((void *) header, sizeof(struct Header));
    
    char *dir;

    if ((dir = (char *)mmap(NULL, ((sizeof(struct Entrada)*i*ie)+sizeof(struct Salida)+oe), PROT_READ | PROT_WRITE, MAP_SHARED, mem, 0)) == MAP_FAILED) {
        cerr << "Error mapeando la memoria compartida: "
        << errno << strerror(errno) << endl;
        exit(1);
    }
    char *pos0 = dir;
    //cout << dir << endl;
    while(n < i){
        //cout << "n " << n << " i " << i << endl;  
        char *posI = (n*ie*sizeof(struct Entrada)) + dir;
        m = 0;
        //cout << "posI " << posI << endl; 
        while ( m < ie)
        {
            //cout << "m " << m << " ie " << ie << endl;
            char *posn = posI + (m * sizeof(struct Entrada));
            //cout << "posn " << posn << endl;
            struct Entrada *pRegistro = (struct Entrada *) posn;
            cout<<pRegistro->tipo<<endl;
            m++;
        }
        n++;
    }
    cout << "salio del while" << endl;
    return "algo";
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
