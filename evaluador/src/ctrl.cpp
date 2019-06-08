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
    return proces.procesando(nomseg);
}

string Ctrl::esperando(string nomseg){
    int n = 0;
    int m = 0;
    int i, ie, oe, q;
    string open = "/" + nomseg;
    string temp="Waiting:\n\n";

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
    
    while(n < i){
        char *posI = (n*ie*sizeof(struct Entrada)) + dir;
        m = 0;
        
        while ( m < ie)
        {
            char *posn = posI + (m * sizeof(struct Entrada));
            struct Entrada *pRegistro = (struct Entrada *) posn;
            //cout<<pRegistro->tipo<<endl;
            if(pRegistro->cantidad > 0){
                temp += to_string(pRegistro->ident) + " " + to_string(pRegistro->bandEntrada) + " " + pRegistro->tipo + " " + to_string(pRegistro->cantidad) + "\n";
            }
            m++;
        }
        n++;
    }
    return temp+"\n";
}

string Ctrl::terminados(string nomseg){
    int n = 0;
    int m = 0;
    int i, ie, oe, q;
    string temp="Reported:\n\n";
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

    char *posFinal = dir+(sizeof(struct Entrada) * i * ie) + sizeof(struct Salida);
    
    while(n < i){
        char *posISalida = (n*ie*sizeof(struct Entrada)) + posFinal;
        m = 0;
        
        while ( m < ie)
        {
            char *posn = posISalida + (m * sizeof(struct Entrada));
            struct Entrada *pRegistro = (struct Entrada *) posn;
            if(pRegistro->cantidad > 0){
                temp += to_string(pRegistro->ident) + " " + to_string(pRegistro->bandEntrada) + " " + pRegistro->tipo + " " + to_string(pRegistro->cantidad) + "\n";
            }
            m++;
        }
        n++;
    }
    
    return temp+"\n";
}

string Ctrl::reactivos(string nomseg){
    string tmp = "Reactives:\n\n";
    string open = "/" + nomseg;
    int mem = shm_open(open.c_str(), O_RDWR, 0660);
    
    if (mem < 0){
        cerr << "Error abriendo la memoria compartida: "
	    << errno << strerror(errno) << endl;
        exit(1);
    }
    string semaforoDitritos = nomseg+"Ditritos";
    string semaforoSangre = nomseg+"Sangre";
    string semaforoPiel = nomseg+"Piel";

    sem_t *ditritos,*sangre,*piel;
    int valditritos,valsangre,valpiel;
    
    ditritos=sem_open(semaforoDitritos.c_str(),0);
    sangre=sem_open(semaforoSangre.c_str(),0);
    piel=sem_open(semaforoPiel.c_str(),0);

    sem_getvalue(ditritos,&valditritos);
    sem_getvalue(sangre,&valsangre);
    sem_getvalue(piel,&valpiel);

    tmp += "B: "+to_string(valsangre)+"\n"+"S: "+to_string(valpiel)+"\n"+"D: "+to_string(valditritos)+"\n";
    return tmp+"\n";
}
string Ctrl::all(string nomseg){
    string open = "/" + nomseg;
    int mem = shm_open(open.c_str(), O_RDWR, 0660);
    Procesando proc;
    string temp = "";
    
    
    if (mem < 0){
        cerr << "Error abriendo la memoria compartida: "
	    << errno << strerror(errno) << endl;
        exit(1);
    }
    temp+=procesando(nomseg,proc);
    temp+=esperando(nomseg);
    temp+=terminados(nomseg);
    temp+=reactivos(nomseg);
    return temp;
}
void Ctrl::actualizar(string nombseg,string tipomuestra, int valormuestra){
    if(tipomuestra=="B"){
        sem_t *sangre = sem_open((nombseg+"Sangre").c_str(), 0);
        for(int i =0;i<valormuestra;i++){
            sem_post(sangre);
        }
    }else  if(tipomuestra=="D"){
        sem_t *ditritos = sem_open((nombseg+"Ditritos").c_str(), 0);
        for(int i =0;i<valormuestra;i++){
            sem_post(ditritos);
        }
    }else  if(tipomuestra=="S"){
        sem_t *piel = sem_open((nombseg+"Piel").c_str(), 0);
        for(int i =0;i<valormuestra;i++){
            sem_post(piel);
        }
    }

}
