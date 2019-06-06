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

void Procesando::procesar(string nomseg)
{

    int n = 0;
    int m = 0;
    int i, ie, oe, q;
    string open = "/" + nomseg;
    string nombreMemoriaSangre = "/" + nomseg + "Sangre";
    string nombreMemoriaPiel = "/" + nomseg + "Piel";
    string nombreMemoriaDitritos = "/" + nomseg + "Ditritos";
    bool enterSangre = false;
    bool enterPiel = false;
    bool enterDitritos = false;
    int nSangre = 0;
    int nPiel = 0;
    int nDitritos = 0;

    sem_t *vacios, *llenos, *mutex;

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

    char *dir;

    char *dirMemSangre = (char *)mmap(NULL, ((sizeof(struct Entrada) * q)), PROT_READ | PROT_WRITE, MAP_SHARED, memSangre, 0);
    char *dirMemPiel = (char *)mmap(NULL, ((sizeof(struct Entrada) * q)), PROT_READ | PROT_WRITE, MAP_SHARED, memPiel, 0);
    char *dirMemDitritos = (char *)mmap(NULL, ((sizeof(struct Entrada) * q)), PROT_READ | PROT_WRITE, MAP_SHARED, memDitritos, 0);

    struct Entrada entrada;
    entrada.bandEntrada = 0;
    entrada.cantidad = -1;
    entrada.tipo = ' ';
    entrada.ident = 0;

    if ((dir = (char *)mmap(NULL, ((sizeof(struct Entrada) * i * ie) + sizeof(struct Salida) + oe), PROT_READ | PROT_WRITE, MAP_SHARED, mem, 0)) == MAP_FAILED)
    {
        cerr << "Error mapeando la memoria compartida: "
             << errno << strerror(errno) << endl;
        exit(1);
    }
    char *pos0 = dir;

    char *posSangre = dirMemSangre;
    char *posPiel = dirMemPiel;
    char *posDitritos = dirMemDitritos;

    char *posISangre = (q * sizeof(struct Entrada)) + dirMemSangre;
    char *posIPiel = (q * sizeof(struct Entrada)) + dirMemPiel;
    char *posIDitritos = (q * sizeof(struct Entrada)) + dirMemDitritos;

    while (n < i)
    {
        char *posI = (n * ie * sizeof(struct Entrada)) + dir;
        m = 0;

        while (m < ie)
        {

            char *posn = posI + (m * sizeof(struct Entrada));
            struct Entrada *pRegistro = (struct Entrada *)posn;

            string nombreSemaforoLlenos = nomseg + "Llenos" + to_string(pRegistro->bandEntrada);
            string nombreSemaforoVacios = nomseg + "Vacios" + to_string(pRegistro->bandEntrada);
            string nombreSemaforoMutex = nomseg + "Mutex" + to_string(pRegistro->bandEntrada);

            vacios = sem_open(nombreSemaforoVacios.c_str(), 0);
            llenos = sem_open(nombreSemaforoLlenos.c_str(), 0);
            mutex = sem_open(nombreSemaforoMutex.c_str(), 0);

            sem_wait(llenos);
            sem_wait(mutex);

            if (pRegistro->tipo == 'B')
            {
                for (;;)
                {
                    while (!enterSangre)
                    {
                        while (nSangre < q)
                        {
                            char *posnSangre = posISangre + (nSangre * sizeof(struct Entrada));
                            struct Entrada *pRegistroSangre = (struct Entrada *)posnSangre;
                            //cout << nSangre << endl;
                            if (pRegistroSangre->cantidad <= 0)
                            {
                                cout << "entro al if del proceso" << endl;
                                pRegistroSangre->bandEntrada = pRegistro->bandEntrada;
                                pRegistroSangre->cantidad = pRegistro->cantidad;
                                pRegistroSangre->ident = pRegistro->ident;
                                pRegistroSangre->tipo = pRegistro->tipo;
                                enterSangre = true;
                                break;
                            }
                            else
                            {
                                nSangre++;
                            }
                        }
                        if (nSangre >= q && !enterSangre)
                        {
                            cout << "Registro está lleno" << endl;
                            sleep(5);
                            nSangre = 0;
                        }
                    }
                    break;
                }
                pRegistro->bandEntrada = entrada.bandEntrada;
                pRegistro->cantidad = entrada.cantidad;
                pRegistro->ident = entrada.ident;
                pRegistro->tipo = entrada.tipo;
            }
            else if (pRegistro->tipo == 'S')
            {
                for (;;)
                {
                    while (!enterPiel)
                    {
                        while (nPiel < q)
                        {
                            char *posnPiel = posIPiel + (nPiel * sizeof(struct Entrada));
                            struct Entrada *pRegistroSangre = (struct Entrada *)posnPiel;

                            if (pRegistroSangre->cantidad <= 0)
                            {
                                //cout << "entro al if del proceso" << endl;
                                pRegistroSangre->bandEntrada = pRegistro->bandEntrada;
                                pRegistroSangre->cantidad = pRegistro->cantidad;
                                pRegistroSangre->ident = pRegistro->ident;
                                pRegistroSangre->tipo = pRegistro->tipo;
                                enterPiel = true;
                                break;
                            }
                            else
                            {
                                nPiel++;
                            }
                        }
                        if (nPiel >= q && !enterPiel)
                        {
                            cout << "Registro está lleno" << endl;
                            sleep(5);
                            nPiel = 0;
                        }
                    }
                    break;
                }
                pRegistro->bandEntrada = entrada.bandEntrada;
                pRegistro->cantidad = entrada.cantidad;
                pRegistro->ident = entrada.ident;
                pRegistro->tipo = entrada.tipo;
            }
            else if (pRegistro->tipo == 'D')
            {
                for (;;)
                {
                    while (!enterDitritos)
                    {
                        while (nSangre < q)
                        {
                            char *posnDitritos = posIDitritos + (nDitritos * sizeof(struct Entrada));
                            struct Entrada *pRegistroSangre = (struct Entrada *)posnDitritos;

                            if (pRegistroSangre->cantidad <= 0)
                            {
                                //cout << "entro al if del proceso" << endl;
                                pRegistroSangre->bandEntrada = pRegistro->bandEntrada;
                                pRegistroSangre->cantidad = pRegistro->cantidad;
                                pRegistroSangre->ident = pRegistro->ident;
                                pRegistroSangre->tipo = pRegistro->tipo;
                                enterDitritos = true;
                                break;
                            }
                            else
                            {
                                nDitritos++;
                            }
                        }
                        if (nDitritos >= q && !enterDitritos)
                        {
                            cout << "Registro está lleno" << endl;
                            sleep(5);
                            nDitritos = 0;
                        }
                    }
                    break;
                }
                pRegistro->bandEntrada = entrada.bandEntrada;
                pRegistro->cantidad = entrada.cantidad;
                pRegistro->ident = entrada.ident;
                pRegistro->tipo = entrada.tipo;
            }
            m++;
            sem_post(mutex);
            sem_post(vacios);
        }
        n++;
    }
}

string Procesando::procesando(string nomseg)
{
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

    while (nSangre < 3)
    {

        char *posISangre = (nSangre * q * sizeof(struct Entrada)) + dirMemSangre;
        mSangre = 0;

        while (mSangre < q)
        {
            char *posnSangre = posISangre + (mSangre * sizeof(struct Entrada));
            struct Entrada *pRegistro = (struct Entrada *)posnSangre;
            if (pRegistro->cantidad > 0)
            {
                tmp += to_string(pRegistro->ident) + " " + to_string(pRegistro->bandEntrada) + " " + pRegistro->tipo + " " + to_string(pRegistro->cantidad) + "\n";
                //cout<<pRegistro->bandEntrada<<" "<<pRegistro->cantidad<<" "<<pRegistro->tipo<<" "<<pRegistro->ident<<endl;
            }
            mSangre++;
        }
        nSangre++;
    }

    while (nPiel < 3)
    {

        char *posIPiel = (nPiel * q * sizeof(struct Entrada)) + dirMemPiel;
        mPiel = 0;

        while (mPiel < q)
        {
            char *posnPiel = posIPiel + (mPiel * sizeof(struct Entrada));
            struct Entrada *pRegistro = (struct Entrada *)posnPiel;
            if (pRegistro->cantidad > 0)
            {
                tmp += to_string(pRegistro->ident) + " " + to_string(pRegistro->bandEntrada) + " " + pRegistro->tipo + " " + to_string(pRegistro->cantidad) + "\n";
                //cout<<pRegistro->bandEntrada<<" "<<pRegistro->cantidad<<" "<<pRegistro->tipo<<" "<<pRegistro->ident<<endl;
            }
            mPiel++;
        }
        nPiel++;
    }

    while (nDitritos < 3)
    {

        char *posIDitritos = (nDitritos * q * sizeof(struct Entrada)) + dirMemDitritos;
        mDitritos = 0;

        while (mDitritos < q)
        {
            char *posnDitritos = posIDitritos + (mDitritos * sizeof(struct Entrada));
            struct Entrada *pRegistro = (struct Entrada *)posnDitritos;
            if (pRegistro->cantidad > 0)
            {
                tmp += to_string(pRegistro->ident) + " " + to_string(pRegistro->bandEntrada) + " " + pRegistro->tipo + " " + to_string(pRegistro->cantidad) + "\n";
                //cout<<pRegistro->bandEntrada<<" "<<pRegistro->cantidad<<" "<<pRegistro->tipo<<" "<<pRegistro->ident<<endl;
            }
            mDitritos++;
        }
        nDitritos++;
    }
    return tmp;
}