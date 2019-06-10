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
#include "bandejas.h"

using namespace std;


void Bandejas::dividirMemoria(string nomseg){
    
    int i, ie, oe, q, mem; 

    string open = "/" + nomseg;
    mem = shm_open(open.c_str(), O_RDWR, 0660);
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
    ban = new char *[i];

    struct Entrada *dir;

    

    if ((dir = (struct Entrada *)mmap(NULL, ((sizeof(struct Entrada)*i*ie)+(sizeof(struct Salida)*oe)), PROT_READ | PROT_WRITE, MAP_SHARED, mem, 0)) == MAP_FAILED) {
        cerr << "Error mapeando la memoria compartida (Dividir Memoria): "
        << errno << strerror(errno) << endl;
        exit(1);
    }

    ban[0] = (((char *)dir) + sizeof(struct Header));
    cout << ban[0] << dir << endl;
    for (int nb = 1; nb < i; nb++){
        ban[nb] = (ban[nb - 1]+ (sizeof( struct Entrada) * ie));
    }
}

char * Bandejas::getPos(int i){
    return ban[i];
}
