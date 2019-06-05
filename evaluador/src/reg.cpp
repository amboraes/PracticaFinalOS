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
#include "procesando.h"

using namespace std;

void Reg::registrar(string nomseg, int bandeja, char tipomuestra, int cantmuestra, int ident){
    int i, ie, oe, q;
    string open = "/" + nomseg;
    bool enter = false;
    int n = 0;

    sem_t *vacios, *llenos, *mutex;

    string nombreSemaforoLlenos = nomseg + "Llenos";
    string nombreSemaforoVacios = nomseg + "Vacios";
    string nombreSemaforoMutex = nomseg + "Mutex";

    vacios = sem_open(nombreSemaforoVacios.c_str(), 0);
    llenos = sem_open(nombreSemaforoLlenos.c_str(), 0);
    mutex  = sem_open(nombreSemaforoMutex.c_str(), 0);

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
    
    struct Entrada entrada;
    entrada.bandEntrada = bandeja;
    entrada.cantidad = cantmuestra;
    entrada.tipo = tipomuestra;
    entrada.ident = ident;
    
    if ((dir = (char *)mmap(NULL, (sizeof(struct Header)+(sizeof(struct Entrada)*i*ie)+sizeof(struct Salida)+oe), PROT_READ | PROT_WRITE, MAP_SHARED, mem, 0)) == MAP_FAILED) {
        cerr << "Error mapeando la memoria compartida: "
        << errno << strerror(errno) << endl;
        exit(1);
    }
    char *pos0 = dir;
    //cout << dir << endl;
    char *posI = (bandeja*ie*sizeof(struct Entrada)) + dir;
    //cout << "posI " << posI << endl;
    for(;;){ 
        sem_wait(vacios);
        sem_wait(mutex);

        while (!enter)
        {
            while(n < ie){
                char *posn = posI + (n * sizeof(struct Entrada));
                struct Entrada *pRegistro = (struct Entrada *) posn;
                //cout << "posn " << posn << endl;
                if(pRegistro->cantidad <= 0){
                    cout << "entro al if de registro" << endl;
                    pRegistro->bandEntrada = entrada.bandEntrada;
                    pRegistro->cantidad = entrada.cantidad;
                    pRegistro->ident = entrada.ident;
                    pRegistro->tipo = entrada.tipo;
                    enter = true;
                    //cout <<pRegistro->tipo<<endl;
                    break;
                }
                else{
                    n++;
                }
            }
            if(n >= ie && !enter){
                cout << "Registro está lleno" << endl;
                sleep(5);
                n = 0;
            }

        }
        sem_post(mutex);
        sem_post(llenos);
        break;
    }
    
}
