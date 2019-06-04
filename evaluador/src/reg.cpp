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

void Reg::registrar(string nomseg, int bandeja, char tipomuestra, int cantmuestra, int ident, int i, int ie, int oe){
    string open = "/" + nomseg;
    bool enter = false;
    int n = 0;

    int mem = shm_open(open.c_str(), O_RDWR, 0660);
    if (mem < 0){
        cerr << "Error abriendo la memoria compartida: "
	    << errno << strerror(errno) << endl;
        exit(1);
    }

    int *dir;
    
    struct Entrada entrada;
    entrada.bandEntrada = bandeja;
    entrada.cantidad = cantmuestra;
    entrada.tipo = tipomuestra;
    entrada.ident = ident;
    
    if ((dir = (int *)mmap(NULL, ((sizeof(struct Entrada)*i*ie)+sizeof(struct Salida)+oe), PROT_READ | PROT_WRITE, MAP_SHARED, mem, 0)) == MAP_FAILED) {
        cerr << "Error mapeando la memoria compartida: "
        << errno << strerror(errno) << endl;
        exit(1);
    }
    int *pos0 = dir;
    //cout << dir << endl;
    int *posI = (bandeja*ie*sizeof(struct Entrada)) + dir;
    //cout << "posI " << posI << endl;
    for(;;){ 
        while (!enter)
        {
            
            while(n < ie){
                int *posn = posI + (n * sizeof(struct Entrada));
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
        break;
    }
    
}
