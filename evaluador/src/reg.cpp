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

//Metodo para realizar el ingreso de registros en las bandejas de entrada 
void Reg::registrar(string nomseg, int bandeja, char tipomuestra, int cantmuestra, int ident)
{
    int i, ie, oe, q;
    string open = "/" + nomseg;
    bool enter = false;
    int n = 0;

    //Apertura de memoria 

    int mem = shm_open(open.c_str(), O_RDWR, 0660);
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

    char *dir;

    struct Entrada entrada;
    entrada.bandEntrada = bandeja;
    entrada.cantidad = cantmuestra;
    entrada.tipo = tipomuestra;
    entrada.ident = ident;

    if ((dir = (char *)mmap(NULL, ((sizeof(struct Entrada) * i * ie) + (sizeof(struct Salida) * oe)), PROT_READ | PROT_WRITE, MAP_SHARED, mem, 0)) == MAP_FAILED)
    {
        cerr << "Error mapeando la memoria compartida: "
             << errno << strerror(errno) << endl;
        exit(1);
    }

    //Llamada a los semaforos de la bandeja a utilizar 

    sem_t *vacios, *llenos, *mutex;

    string nombreSemaforoLlenos = nomseg + "Llenos" + to_string(bandeja);
    string nombreSemaforoVacios = nomseg + "Vacios" + to_string(bandeja);
    string nombreSemaforoMutex = nomseg + "Mutex" + to_string(bandeja);

    vacios = sem_open(nombreSemaforoVacios.c_str(), 0);
    llenos = sem_open(nombreSemaforoLlenos.c_str(), 0);
    mutex = sem_open(nombreSemaforoMutex.c_str(), 0);
    char *pos0 = dir;
    char *posI = (bandeja * ie * sizeof(struct Entrada)) + dir;

    int tmp1;

    //Iteracion en la bandeja de entrada llamada 
    while (n < ie)
    {
        char *posn = posI + (n * sizeof(struct Entrada));
        struct Entrada *pRegistro = (struct Entrada *)posn;

        if (pRegistro->cantidad <= 0)
        {
            //Llamada a los semaforos (Productor)
            sem_wait(vacios);
            sem_wait(mutex);
            pRegistro->bandEntrada = entrada.bandEntrada;
            pRegistro->cantidad = entrada.cantidad;
            pRegistro->ident = entrada.ident;
            pRegistro->tipo = entrada.tipo;
            enter = true;
            sem_post(mutex);
            sem_post(llenos);
            break;
        }
        else
        {
            n++;
        }
    }
}