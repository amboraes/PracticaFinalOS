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


string Ctrl::procesando(string nomseg){
    string tmp = "Processing:\n\n";
    int i, ie, oe, q;

    int nSangre = 0;
    int nPiel = 0;
    int nDitritos = 0;

    int mSangre = 0;
    int mPiel = 0;
    int mDitritos = 0;
    string open = "/" + nomseg;
    string nombreMemoriaSangre = "/" + nomseg + "Sangre";
    string nombreMemoriaPiel = "/" + nomseg + "Piel";
    string nombreMemoriaDitritos = "/" + nomseg + "Ditritos";

    int mem = shm_open(open.c_str(), O_RDWR, 0660);
    int memSangre = shm_open(nombreMemoriaSangre.c_str(), O_RDWR, 0660);
    int memPiel = shm_open(nombreMemoriaPiel.c_str(), O_RDWR, 0660);
    int memDitritos = shm_open(nombreMemoriaDitritos.c_str(), O_RDWR, 0660);

    if (mem < 0)
    {
        cerr << "Error abriendo la memoria compartida: "
             << errno << strerror(errno) << endl;
        exit(1);
    }

    struct Header *header = (struct Header *)mmap(NULL, sizeof(struct Header), PROT_READ | PROT_WRITE, MAP_SHARED, mem, 0);
    i = header->i;
    ie = header->ie;
    oe = header->oe;
    q = header->q;

    munmap((void *)header, sizeof(struct Header));

    char *dirMemSangre = (char *)mmap(NULL, ((sizeof(struct Entrada) * q)), PROT_READ | PROT_WRITE, MAP_SHARED, memSangre, 0);
    char *dirMemPiel = (char *)mmap(NULL, ((sizeof(struct Entrada) * q)), PROT_READ | PROT_WRITE, MAP_SHARED, memPiel, 0);
    char *dirMemDitritos = (char *)mmap(NULL, ((sizeof(struct Entrada) * q)), PROT_READ | PROT_WRITE, MAP_SHARED, memDitritos, 0);

    char *posSangre = dirMemSangre;
    char *posPiel = dirMemPiel;
    char *posDitritos = dirMemDitritos;

    char *posISangre = (q * sizeof(struct Entrada)) + dirMemSangre;
    mSangre = 0;

    while (mSangre < q)
    {
        char *posnSangre = posISangre + (mSangre * sizeof(struct Entrada));
        struct Entrada *pRegistro = (struct Entrada *)posnSangre;
        if (pRegistro->cantidad > 0)
        {
            tmp += to_string(pRegistro->ident) + " " + to_string(pRegistro->bandEntrada) + " " + pRegistro->tipo + " " + to_string(pRegistro->cantidad) + "\n";
        }
        mSangre++;
    }

    char *posIPiel = (q * sizeof(struct Entrada)) + dirMemPiel;
    mPiel = 0;

    while (mPiel < q)
    {
        char *posnPiel = posIPiel + (mPiel * sizeof(struct Entrada));
        struct Entrada *pRegistro = (struct Entrada *)posnPiel;
        if (pRegistro->cantidad > 0)
        {
            tmp += to_string(pRegistro->ident) + " " + to_string(pRegistro->bandEntrada) + " " + pRegistro->tipo + " " + to_string(pRegistro->cantidad) + "\n";
        }
        mPiel++;
    }

    char *posIDitritos = (q * sizeof(struct Entrada)) + dirMemDitritos;
    mDitritos = 0;

    while (mDitritos < q)
    {
        char *posnDitritos = posIDitritos + (mDitritos * sizeof(struct Entrada));
        struct Entrada *pRegistro = (struct Entrada *)posnDitritos;
        if (pRegistro->cantidad > 0)
        {
            tmp += to_string(pRegistro->ident) + " " + to_string(pRegistro->bandEntrada) + " " + pRegistro->tipo + " " + to_string(pRegistro->cantidad) + "\n";
        }
        mDitritos++;
    }

    return tmp;
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

    if ((dir = (char *)mmap(NULL,((sizeof(struct Entrada)*i*ie)+sizeof(struct Salida)*oe), PROT_READ | PROT_WRITE, MAP_SHARED, mem, 0)) == MAP_FAILED) {
        cerr << "Error mapeando la memoria compartida: "
        << errno << strerror(errno) << endl;
        exit(1);
    }
    while(n < i){
        char *posI = (n*ie*sizeof(struct Entrada))+dir;
        m = 0;
        
        while ( m < ie)
        {
            char *posn = posI + (m * sizeof(struct Entrada));
            struct Entrada *pRegistro = (struct Entrada *) posn;
            if((pRegistro->cantidad > 0) && (pRegistro->tipo != ' ')){
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

    if ((dir = (char *)mmap(NULL, ((sizeof(struct Entrada)*i*ie)+(sizeof(struct Salida)*oe)), PROT_READ | PROT_WRITE, MAP_SHARED, mem, 0)) == MAP_FAILED) {
        cerr << "Error mapeando la memoria compartida: "
        << errno << strerror(errno) << endl;
        exit(1);
    }

    char *posFinal = dir+(sizeof(struct Entrada) * i * ie);
    
    
        char *posISalida = (n*ie*sizeof(struct Entrada)) + posFinal;
        m = 0;
        
        while ( m < oe)
        {
            char *posn = posISalida + (m * sizeof(struct Salida));
            struct Salida *pRegistro = (struct Salida *) posn;
            if(pRegistro->ident > 0){
                temp += to_string(pRegistro->ident) + " " + to_string(pRegistro->bandeja) + " " + pRegistro->tipo + " " + pRegistro->result + "\n";
            }
            m++;
        }
        n++;
    
    
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
    string temp = "";
    
    if (mem < 0){
        cerr << "Error abriendo la memoria compartida: "
	    << errno << strerror(errno) << endl;
        exit(1);
    }
    temp+=esperando(nomseg);
    temp+=procesando(nomseg);
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
