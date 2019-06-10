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

//Metodo para mostrar las colas internas del sistema
string Ctrl::procesando(string nomseg)
{
    string tmp = "Processing:\n\n";
    int i, ie, oe, q;

    //Declaracion de los nombres de los semaforos a utilizar

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

    //Apertura de las diferentes memorias compartidas

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

    //Acceso a la estructura header en la memoria compartida para utilizar los parametros ingresados en el Init

    struct Header *header = (struct Header *)mmap(NULL, sizeof(struct Header), PROT_READ | PROT_WRITE, MAP_SHARED, mem, 0);
    i = header->i;
    ie = header->ie;
    oe = header->oe;
    q = header->q;

    munmap((void *)header, sizeof(struct Header));

    //Mapeo de las memorias compartidas de las diferentes memorias compartidas usadas por las colas internas

    char *dirMemSangre = (char *)mmap(NULL, ((sizeof(struct Entrada) * q)), PROT_READ | PROT_WRITE, MAP_SHARED, memSangre, 0);
    char *dirMemPiel = (char *)mmap(NULL, ((sizeof(struct Entrada) * q)), PROT_READ | PROT_WRITE, MAP_SHARED, memPiel, 0);
    char *dirMemDitritos = (char *)mmap(NULL, ((sizeof(struct Entrada) * q)), PROT_READ | PROT_WRITE, MAP_SHARED, memDitritos, 0);

    char *posSangre = dirMemSangre;
    char *posPiel = dirMemPiel;
    char *posDitritos = dirMemDitritos;

    //Calculo de la posición inicial de la cola intera para los registros de sangre en la memoria compartida respectiva

    char *posISangre = (q * sizeof(struct Entrada)) + dirMemSangre;
    mSangre = 0;

    //Recorrido completo de la memoria compartida de sangre
    //usando 'mSangre' para iterar en las 'q' posiciones de la cola
    while (mSangre < q)
    {
        //Calculo de la posicion de memoria sumando la posicion inicial de la cola con la multiplicacion del iterador por el tamaño de cada posicion en la cola
        //representada por el tamaño de la estructura que se ingresa (struct Entrada)
        char *posnSangre = posISangre + (mSangre * sizeof(struct Entrada));
        //Se toma la estructura que hay en la posicion calculada
        //Si la variable 'cantidad' que hay en este registro es mayor a 0, significa que esta posicion contiene un registro válido y se puede mostrar
        struct Entrada *pRegistro = (struct Entrada *)posnSangre;
        if (pRegistro->cantidad > 0)
        {
            tmp += to_string(pRegistro->ident) + " " + to_string(pRegistro->bandEntrada) + " " + pRegistro->tipo + " " + to_string(pRegistro->cantidad) + "\n";
        }
        mSangre++;
    }

    //Calculo de la posición inicial de la cola intera para los registros de piel en la memoria compartida respectiva
    char *posIPiel = (q * sizeof(struct Entrada)) + dirMemPiel;
    mPiel = 0;

    //Iteracion en la cola intermedia
    while (mPiel < q)
    {
        //Posicion de memoria en la iteracion actual
        char *posnPiel = posIPiel + (mPiel * sizeof(struct Entrada));
        //Revision de la validez en la posicion de memoria
        struct Entrada *pRegistro = (struct Entrada *)posnPiel;
        if (pRegistro->cantidad > 0)
        {
            tmp += to_string(pRegistro->ident) + " " + to_string(pRegistro->bandEntrada) + " " + pRegistro->tipo + " " + to_string(pRegistro->cantidad) + "\n";
        }
        mPiel++;
    }

    //Calculo de la posición inicial de la cola intera para los registros de piel en la memoria compartida respectiva
    char *posIDitritos = (q * sizeof(struct Entrada)) + dirMemDitritos;
    mDitritos = 0;

    //Iteracion en la cola intermedia
    while (mDitritos < q)
    {
        //Posicion de memoria en la iteracion actual
        char *posnDitritos = posIDitritos + (mDitritos * sizeof(struct Entrada));
        //Revision de la validez en la posicion de memoria
        struct Entrada *pRegistro = (struct Entrada *)posnDitritos;
        if (pRegistro->cantidad > 0)
        {
            tmp += to_string(pRegistro->ident) + " " + to_string(pRegistro->bandEntrada) + " " + pRegistro->tipo + " " + to_string(pRegistro->cantidad) + "\n";
        }
        mDitritos++;
    }

    return tmp;
}

//Metodo para mostrar las colas de entrada en el sistema
string Ctrl::esperando(string nomseg)
{
    int n = 0;
    int m = 0;
    int i, ie, oe, q;
    string open = "/" + nomseg;
    string temp = "Waiting:\n\n";

    //Apertura de memoria compartida donde se encuentran las colas de entrada

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

    if ((dir = (char *)mmap(NULL, ((sizeof(struct Entrada) * i * ie) + sizeof(struct Salida) * oe), PROT_READ | PROT_WRITE, MAP_SHARED, mem, 0)) == MAP_FAILED)
    {
        cerr << "Error mapeando la memoria compartida: "
             << errno << strerror(errno) << endl;
        exit(1);
    }

    //Iteracion en todas las colas de entrada en la memoria compartida
    while (n < i)
    {

        //Calculo de la posicion inicial de cada bandeja de entrada en la que se itera
        char *posI = (n * ie * sizeof(struct Entrada)) + dir;
        m = 0;

        //Iteracion en las diferentes posiciones de cada bandeja de entrada
        while (m < ie)
        {
            char *posn = posI + (m * sizeof(struct Entrada));
            struct Entrada *pRegistro = (struct Entrada *)posn;
            if ((pRegistro->cantidad > 0) && (pRegistro->tipo != 'd'))
            {
                temp += to_string(pRegistro->ident) + " " + to_string(pRegistro->bandEntrada) + " " + pRegistro->tipo + " " + to_string(pRegistro->cantidad) + "\n";
            }
            m++;
        }
        n++;
    }
    return temp + "\n";
}

//Metodo para mostrar la cola de salida del sistema
string Ctrl::terminados(string nomseg)
{
    int n = 0;
    int m = 0;
    int i, ie, oe, q;
    string temp = "Reported:\n\n";
    string open = "/" + nomseg;

    //Apertura de memoria compartida donde se encuentra la cola de salida
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

    if ((dir = (char *)mmap(NULL, ((sizeof(struct Entrada) * i * ie) + (sizeof(struct Salida) * oe)), PROT_READ | PROT_WRITE, MAP_SHARED, mem, 0)) == MAP_FAILED)
    {
        cerr << "Error mapeando la memoria compartida: "
             << errno << strerror(errno) << endl;
        exit(1);
    }

    //Calculo posicion inicial de la cola de salida 

    char *posFinal = dir + (sizeof(struct Entrada) * i * ie);

    char *posISalida = (n * ie * sizeof(struct Entrada)) + posFinal;
    m = 0;

    //Iteracion en la cola de salida 
    while (m < oe)
    {
        char *posn = posISalida + (m * sizeof(struct Salida));
        struct Salida *pRegistro = (struct Salida *)posn;
        if (pRegistro->ident > 0)
        {
            temp += to_string(pRegistro->ident) + " " + to_string(pRegistro->bandeja) + " " + pRegistro->tipo + " " + pRegistro->result + "\n";
        }
        m++;
    }

    return temp + "\n";
}
//Metodo para listar los reactivos que se encuentran disponibles en el sistema
string Ctrl::reactivos(string nomseg)
{
    string tmp = "Reactives:\n\n";
    string open = "/" + nomseg;
    int mem = shm_open(open.c_str(), O_RDWR, 0660);

    if (mem < 0)
    {
        cerr << "Error abriendo la memoria compartida: "
             << errno << strerror(errno) << endl;
        exit(1);
    }
    string semaforoDitritos = nomseg + "Ditritos";
    string semaforoSangre = nomseg + "Sangre";
    string semaforoPiel = nomseg + "Piel";

    sem_t *ditritos, *sangre, *piel;
    int valditritos, valsangre, valpiel;

    ditritos = sem_open(semaforoDitritos.c_str(), 0);
    sangre = sem_open(semaforoSangre.c_str(), 0);
    piel = sem_open(semaforoPiel.c_str(), 0);


    //Este metodo nos devuelve el valor que hay actualmente en los semaforos de los reactivos
    sem_getvalue(ditritos, &valditritos);
    sem_getvalue(sangre, &valsangre);
    sem_getvalue(piel, &valpiel);

    tmp += "B: " + to_string(valsangre) + "\n" + "S: " + to_string(valpiel) + "\n" + "D: " + to_string(valditritos) + "\n";
    return tmp + "\n";
}
//Metodo para listar todos los examenes y reactivos presentes en el sistema en el siguiente orden
//Waiting | Processing | Reported | Reactive
string Ctrl::all(string nomseg)
{
    string open = "/" + nomseg;
    int mem = shm_open(open.c_str(), O_RDWR, 0660);
    string temp = "";

    if (mem < 0)
    {
        cerr << "Error abriendo la memoria compartida: "
             << errno << strerror(errno) << endl;
        exit(1);
    }
    temp += esperando(nomseg);
    temp += procesando(nomseg);
    temp += terminados(nomseg);
    temp += reactivos(nomseg);
    return temp+"\n";
}
//Metodo para actualizar la cantidad de reactivos en el sistema
void Ctrl::actualizar(string nombseg, string tipomuestra, int valormuestra)
{
    if (tipomuestra == "B")
    {
        //Metodo para aumentar la cantidad de Blood
        sem_t *sangre = sem_open((nombseg + "Sangre").c_str(), 0);
        for (int i = 0; i < valormuestra; i++)
        {
            sem_post(sangre);
        }
    }
    else if (tipomuestra == "D")
    {
        //Metodo para aumentar la cantidad de Ditritos
        sem_t *ditritos = sem_open((nombseg + "Ditritos").c_str(), 0);
        for (int i = 0; i < valormuestra; i++)
        {
            sem_post(ditritos);
        }//Metodo para aumentar la cantidad de Blood
    }
    else if (tipomuestra == "S")
    {
        //Metodo para aumentar la cantidad de Skin
        sem_t *piel = sem_open((nombseg + "Piel").c_str(), 0);
        for (int i = 0; i < valormuestra; i++)
        {
            sem_post(piel);
        }
    }
}
