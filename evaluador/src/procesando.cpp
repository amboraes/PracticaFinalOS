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
#include "elements.h"
#include "procesando.h"
#include <string>
#include <vector>

using namespace std;


void Procesando::procesar(string nomseg){

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

    struct Entrada entrada;
    entrada.bandEntrada = 0;
    entrada.cantidad = -1;
    entrada.tipo = ' ';
    entrada.ident = 0;

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
            //cout<<pRegistro->tipo<<endl;
            //cout << "why????" << endl;
            if(pRegistro->tipo == 'B'){
                cout << "why????" << endl;
                sangre.push_back(pRegistro);
                cout << sangre.size() << endl;
                pRegistro->bandEntrada = entrada.bandEntrada;
                pRegistro->cantidad = entrada.cantidad;
                pRegistro->ident = entrada.ident;
                pRegistro->tipo = entrada.tipo;
            }else if (pRegistro->tipo == 'S')
            {
                piel.push_back(pRegistro);
                pRegistro->bandEntrada = entrada.bandEntrada;
                pRegistro->cantidad = entrada.cantidad;
                pRegistro->ident = entrada.ident;
                pRegistro->tipo = entrada.tipo;
            }else if (pRegistro->tipo == 'D')
            {
                ditritos.push_back(pRegistro);
                pRegistro->bandEntrada = entrada.bandEntrada;
                pRegistro->cantidad = entrada.cantidad;
                pRegistro->ident = entrada.ident;
                pRegistro->tipo = entrada.tipo;
            }
            m++;
        }
        n++;
    }

}

string Procesando::procesando(){
    string tmp;
    //printf("fuckme ",sangre.size());
    for (int i = 0; i < piel.size(); i++)
    {
        printf("fuckme ",piel.size());
        tmp+=piel[i]->ident +" "+ to_string(piel[i]->bandEntrada)+" "+piel[i]->tipo +" "+to_string(piel[i]->cantidad)/* +" "+piel[i].tiempovida */+"\n";
    }
     for (int i = 0; i < ditritos.size(); i++)
    {
        tmp+=ditritos[i]->ident +" "+ to_string(ditritos[i]->bandEntrada)+" "+ditritos[i]->tipo +" "+to_string(ditritos[i]->cantidad)/* +" "+ditritos[i].tiempovida */+"\n";
    }
    cout << "antes del for de sangre " << sangre.size()<<endl;
    for (int i = 0; i < sangre.size(); i++)
    {
        printf("fuckme ",sangre.size(),"x2");
        tmp+=sangre[i]->ident +" "+ to_string(sangre[i]->bandEntrada)+ " "+sangre[i]->tipo +" "+to_string(sangre[i]->cantidad)/* +" "+sangre[i].tiempovida */+"\n";
    }
    return tmp;
}