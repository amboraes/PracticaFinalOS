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
#include <pthread.h>

using namespace std;

void Procesando::procesar(string nomseg, int bandeja)
{
    int m = 0;
    int i, ie, oe, q;
    string open = "/" + nomseg;
    string nombres[3] = {"Sangre", "Piel", "Ditritos"};
    string nombreMemoriaSangre = "/" + nomseg + "Sangre";
    string nombreMemoriaPiel = "/" + nomseg + "Piel";
    string nombreMemoriaDitritos = "/" + nomseg + "Ditritos";
    bool enterSangre = false;
    bool enterPiel = false;
    bool enterDitritos = false;
    int nSangre = 0;
    int nPiel = 0;
    int nDitritos = 0;

    sem_t *vacios, *llenos, *mutex, *vaciosS, *llenosS, *mutexS, *vaciosP, *llenosP, *mutexP, *vaciosD, *llenosD, *mutexD;

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

    char *posI = (bandeja * ie * sizeof(struct Entrada)) + dir;
    m = 0;

    string nombreSemaforoLlenosS = nomseg + nombres[0] + "Llenos";
    string nombreSemaforoVaciosS = nomseg + nombres[0] + "Vacios";
    string nombreSemaforoMutexS = nomseg + nombres[0] + "Mutex";

    string nombreSemaforoLlenosP = nomseg + nombres[1] + "Llenos";
    string nombreSemaforoVaciosP = nomseg + nombres[1] + "Vacios";
    string nombreSemaforoMutexP = nomseg + nombres[1] + "Mutex";

    string nombreSemaforoLlenosD = nomseg + nombres[2] + "Llenos";
    string nombreSemaforoVaciosD = nomseg + nombres[2] + "Vacios";
    string nombreSemaforoMutexD = nomseg + nombres[2] + "Mutex";

    vaciosS = sem_open(nombreSemaforoVaciosS.c_str(), 0);
    llenosS = sem_open(nombreSemaforoLlenosS.c_str(), 0);
    mutexS = sem_open(nombreSemaforoMutexS.c_str(), 0);

    vaciosP = sem_open(nombreSemaforoVaciosP.c_str(), 0);
    llenosP = sem_open(nombreSemaforoLlenosP.c_str(), 0);
    mutexP = sem_open(nombreSemaforoMutexP.c_str(), 0);

    vaciosD = sem_open(nombreSemaforoVaciosD.c_str(), 0);
    llenosD = sem_open(nombreSemaforoLlenosD.c_str(), 0);
    mutexD = sem_open(nombreSemaforoMutexD.c_str(), 0);

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

        if (pRegistro->tipo == 'B')
        {
            sem_wait(llenos);
            sem_wait(mutex);
            for (;;)
            {

                while (!enterSangre)
                {
                    while (nSangre < q)
                    {
                        char *posnSangre = posISangre + (nSangre * sizeof(struct Entrada));
                        struct Entrada *pRegistroSangre = (struct Entrada *)posnSangre;
                        
                        if (pRegistroSangre->cantidad <= 0)
                        {
                            sem_wait(vaciosS);
                            sem_wait(mutexS);
                            pRegistroSangre->bandEntrada = pRegistro->bandEntrada;
                            pRegistroSangre->cantidad = pRegistro->cantidad;
                            pRegistroSangre->ident = pRegistro->ident;
                            pRegistroSangre->tipo = pRegistro->tipo;
                            enterSangre = true;
                            sem_post(mutexS);
                            sem_post(llenosS);
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
            sem_post(mutex);
            sem_post(vacios);
        }
        else if (pRegistro->tipo == 'S')
        {
            sem_wait(llenos);
            sem_wait(mutex);
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
                            sem_wait(vaciosP);
                            sem_wait(mutexP);
                            pRegistroSangre->bandEntrada = pRegistro->bandEntrada;
                            pRegistroSangre->cantidad = pRegistro->cantidad;
                            pRegistroSangre->ident = pRegistro->ident;
                            pRegistroSangre->tipo = pRegistro->tipo;
                            enterPiel = true;
                            sem_post(mutexP);
                            sem_post(llenosP);
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
            sem_post(mutex);
            sem_post(vacios);
        }
        else if (pRegistro->tipo == 'D')
        {
            sem_wait(llenos);
            sem_wait(mutex);
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
                            sem_wait(vaciosD);
                            sem_wait(mutexD);
                            pRegistroSangre->bandEntrada = pRegistro->bandEntrada;
                            pRegistroSangre->cantidad = pRegistro->cantidad;
                            pRegistroSangre->ident = pRegistro->ident;
                            pRegistroSangre->tipo = pRegistro->tipo;
                            enterDitritos = true;
                            sem_post(mutexD);
                            sem_post(llenosD);
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
            sem_post(mutex);
            sem_post(vacios);
        }
        m++;
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

    char *posISangre = (q * sizeof(struct Entrada)) + dirMemSangre;
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

    char *posIPiel = (q * sizeof(struct Entrada)) + dirMemPiel;
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

    char *posIDitritos = (q * sizeof(struct Entrada)) + dirMemDitritos;
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

    return tmp;
}

void Procesando::procesado(string nomseg)
{
    int i, ie, oe, q;
    string nombres[3] = {"Sangre", "Piel", "Ditritos"};

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
    string nombreSemSangre = nomseg + "Sangre";
    string nombreSemPiel = nomseg + "Piel";
    string nombreSemDitritos = nomseg + "Ditritos";

    int mem = shm_open(open.c_str(), O_RDWR, 0660);
    int memSangre = shm_open(nombreMemoriaSangre.c_str(), O_RDWR, 0660);
    int memPiel = shm_open(nombreMemoriaPiel.c_str(), O_RDWR, 0660);
    int memDitritos = shm_open(nombreMemoriaDitritos.c_str(), O_RDWR, 0660);
    sem_t *semsangre, *semditritos, *sempiel;
    sem_t *vaciosSalida, *llenosSalida, *mutexSalida, *vaciosS, *llenosS, *mutexS, *vaciosP, *llenosP, *mutexP, *vaciosD, *llenosD, *mutexD;

    semsangre = sem_open(nombreSemSangre.c_str(), 0);
    semditritos = sem_open(nombreSemDitritos.c_str(), 0);
    sempiel = sem_open(nombreSemPiel.c_str(), 0);

    string nombreSemaforoLlenosSalida = nomseg + "LlenosSalida";
    string nombreSemaforoVaciosSalida = nomseg + "VaciosSalida";
    string nombreSemaforoMutexSalida = nomseg + "MutexSalida";

    string nombreSemaforoLlenosS = nomseg + nombres[0] + "Llenos";
    string nombreSemaforoVaciosS = nomseg + nombres[0] + "Vacios";
    string nombreSemaforoMutexS = nomseg + nombres[0] + "Mutex";

    string nombreSemaforoLlenosP = nomseg + nombres[1] + "Llenos";
    string nombreSemaforoVaciosP = nomseg + nombres[1] + "Vacios";
    string nombreSemaforoMutexP = nomseg + nombres[1] + "Mutex";

    string nombreSemaforoLlenosD = nomseg + nombres[2] + "Llenos";
    string nombreSemaforoVaciosD = nomseg + nombres[2] + "Vacios";
    string nombreSemaforoMutexD = nomseg + nombres[2] + "Mutex";

    vaciosSalida = sem_open(nombreSemaforoVaciosSalida.c_str(), 0);
    llenosSalida = sem_open(nombreSemaforoLlenosSalida.c_str(), 0);
    mutexSalida = sem_open(nombreSemaforoMutexSalida.c_str(), 0);

    vaciosS = sem_open(nombreSemaforoVaciosS.c_str(), 0);
    llenosS = sem_open(nombreSemaforoLlenosS.c_str(), 0);
    mutexS = sem_open(nombreSemaforoMutexS.c_str(), 0);

    vaciosP = sem_open(nombreSemaforoVaciosP.c_str(), 0);
    llenosP = sem_open(nombreSemaforoLlenosP.c_str(), 0);
    mutexP = sem_open(nombreSemaforoMutexP.c_str(), 0);

    vaciosD = sem_open(nombreSemaforoVaciosD.c_str(), 0);
    llenosD = sem_open(nombreSemaforoLlenosD.c_str(), 0);
    mutexD = sem_open(nombreSemaforoMutexD.c_str(), 0);

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

    char *dir = (char *)mmap(NULL, ((sizeof(struct Entrada) * i * ie) + sizeof(struct Salida) + oe), PROT_READ | PROT_WRITE, MAP_SHARED, mem, 0);

    char *dirsalida = dir + (sizeof(struct Entrada) * i * ie);

    int temp = 0;

    char *posISangre = (q * sizeof(struct Entrada)) + dirMemSangre;
    mSangre = 0;

    while (mSangre < q)
    {
        char *posnSangre = posISangre + (mSangre * sizeof(struct Entrada));
        struct Entrada *pRegistrosangre = (struct Entrada *)posnSangre;
        if (pRegistrosangre->cantidad > 0)
        {
            sem_wait(llenosS);
            sem_wait(mutexS);
            while (temp < oe)
            {
                char *posnsalida = dirsalida + (sizeof(struct Entrada) * temp);
                struct Entrada *registrosalida = (struct Entrada *)posnsalida;
                if (registrosalida->cantidad <= 0)
                {   
                    int temp1,temp2,temp3;
                    sem_getvalue(vaciosSalida,&temp1);
                    cout <<"valor semvacios: " << temp1 << endl;
                    sem_wait(vaciosSalida);
                    sem_wait(mutexSalida);
                    registrosalida->cantidad = pRegistrosangre->cantidad;
                    registrosalida->ident = pRegistrosangre->ident;
                    registrosalida->tipo = pRegistrosangre->tipo;
                    registrosalida->bandEntrada = pRegistrosangre->bandEntrada;
                    pRegistrosangre->cantidad = -1;
                    for (int i = 0; i < registrosalida->cantidad; i++)
                    {
                        sem_wait(semsangre);
                    }
                    sem_getvalue(mutexSalida,&temp2);
                    cout <<"valor mutexvacio: " << temp2 << endl;
                    sem_post(mutexSalida);
                    sem_getvalue(vaciosSalida,&temp3);
                    cout <<"valor llenos salida: " << temp3 << endl;
                    sem_post(llenosSalida);
                    break;
                }
                temp++;
            }
            sem_post(mutexS);
            sem_post(vaciosS);
        }
        mSangre++;
    }
    temp = 0;

    char *posIPiel = (q * sizeof(struct Entrada)) + dirMemPiel;
    mPiel = 0;
    
    while (mPiel < q)
    {
        char *posnPiel = posIPiel + (mPiel * sizeof(struct Entrada));
        struct Entrada *pRegistro = (struct Entrada *)posnPiel;
        
        if (pRegistro->cantidad > 0)
        {
            sem_wait(llenosP);
            sem_wait(mutexP);
            while (temp < oe)
            {
                char *posnsalida = dirsalida + (sizeof(struct Entrada) * temp);
                struct Entrada *registrosalida = (struct Entrada *)posnsalida;
                if (registrosalida->cantidad <= 0)
                {
                    //sem_wait(vaciosSalida);
                    //sem_wait(mutexSalida);   
                    registrosalida->cantidad = pRegistro->cantidad;
                    registrosalida->ident = pRegistro->ident;
                    registrosalida->tipo = pRegistro->tipo;
                    registrosalida->bandEntrada = pRegistro->bandEntrada;
                    pRegistro->cantidad = -1;
                    for (int i = 0; i < registrosalida->cantidad; i++)
                    {
                        sem_wait(sempiel);
                    }
                    //sem_post(mutexSalida);
                    //sem_post(llenosSalida);
                    break;
                }
                temp++;
            }
            sem_post(mutexP);
            sem_post(vaciosP);
        }
        mPiel++;
    }
    temp = 0;

    char *posIDitritos = (q * sizeof(struct Entrada)) + dirMemDitritos;
    mDitritos = 0;
    
    while (mDitritos < q)
    {
        char *posnDitritos = posIDitritos + (mDitritos * sizeof(struct Entrada));
        struct Entrada *pRegistro = (struct Entrada *)posnDitritos;
       
        if (pRegistro->cantidad > 0)
        {
            sem_wait(llenosD);
            sem_wait(mutexD);
            while (temp < oe)
            {
                char *posnsalida = dirsalida + (sizeof(struct Entrada) * temp);
                struct Entrada *registrosalida = (struct Entrada *)posnsalida;
                if (registrosalida->cantidad <= 0)
                {
                    //sem_wait(vaciosSalida);
                    //sem_wait(mutexSalida);
                    registrosalida->cantidad = pRegistro->cantidad;
                    registrosalida->ident = pRegistro->ident;
                    registrosalida->tipo = pRegistro->tipo;
                    registrosalida->bandEntrada = pRegistro->bandEntrada;
                    pRegistro->cantidad = -1;
                    for (int i = 0; i < registrosalida->cantidad; i++)
                    {
                        sem_wait(semditritos);
                    }
                    //sem_post(mutexSalida);
                    //sem_post(llenosSalida);
                    break;
                }
                temp++;
            }
            sem_post(mutexD);
            sem_post(vaciosD);
        }
        mDitritos++;
    }
}