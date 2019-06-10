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
#include <ctime>


//Metodo para procesar los registros en bandejas de entrada a las diferentes colas internas
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

    //Apertura de las diferentes memorias compartidas 

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

    if ((dir = (char *)mmap(NULL, ((sizeof(struct Entrada) * i * ie) + (sizeof(struct Salida) * oe)), PROT_READ | PROT_WRITE, MAP_SHARED, mem, 0)) == MAP_FAILED)
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

    //Llamada a los semaforos de las bandejas de entrada e internas 

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
    int semval;
    
    //Se inicia el ciclo infinito que hará el recorrido por la memoria de entrada
    for (;;)
    {
        while (m < ie)
        {
            sleep(2);
            char *posn = posI + (m * sizeof(struct Entrada));
            struct Entrada *pRegistro = (struct Entrada *)posn;

            string nombreSemaforoLlenos = nomseg + "Llenos" + to_string(pRegistro->bandEntrada);
            string nombreSemaforoVacios = nomseg + "Vacios" + to_string(pRegistro->bandEntrada);
            string nombreSemaforoMutex = nomseg + "Mutex" + to_string(pRegistro->bandEntrada);
            vacios = sem_open(nombreSemaforoVacios.c_str(), 0);
            llenos = sem_open(nombreSemaforoLlenos.c_str(), 0);
            mutex = sem_open(nombreSemaforoMutex.c_str(), 0);
            //Si hay algun registro adentro se clasifica
            if (pRegistro->tipo == 'B')
            {       //Se revisa la memoria comaprtida de sangre
                    while (nSangre < q)
                    {  
                        char *posnSangre = posISangre + (nSangre * sizeof(struct Entrada));
                        struct Entrada *pRegistroSangre = (struct Entrada *)posnSangre;
                        //Si se encuentra un lugar para meter el registro
                        if (pRegistroSangre->cantidad <= 0)
                        { 
                            //Revisión semaforos (Productor-consumidor)
                            sem_wait(llenos);
                            sem_wait(mutex);
                            
                            sleep(2);
                            sem_wait(vaciosS);
                            sem_wait(mutexS);
                            pRegistroSangre->bandEntrada = pRegistro->bandEntrada;
                            pRegistroSangre->cantidad = pRegistro->cantidad;
                            pRegistroSangre->ident = pRegistro->ident;
                            pRegistroSangre->tipo = pRegistro->tipo;
                            enterSangre = true;
                            sem_post(mutexS);
                            sem_post(llenosS);
                            
                            sem_post(mutex);
                            sem_post(vacios);   
                            break;
                        }
                        else
                        {
                            nSangre++;
                        }
                    }
                    
                
                nSangre = 0;
                //Se borra el registro de la memoria de entrada
                pRegistro->bandEntrada = entrada.bandEntrada;
                pRegistro->cantidad = entrada.cantidad;
                pRegistro->ident = entrada.ident;
                pRegistro->tipo = entrada.tipo;
            }
            //Si hay algun registro adentro se clasifica
            else if (pRegistro->tipo == 'S')
            {       //Se revisa la memoria comaprtida de piel
                    while (nPiel < q)
                    {
                        
                        char *posnPiel = posIPiel + (nPiel * sizeof(struct Entrada));
                        struct Entrada *pRegistroSangre = (struct Entrada *)posnPiel;
                        //Si se encuentra un lugar para meter el registro
                        if (pRegistroSangre->cantidad <= 0)
                        {
                            //Revisión semaforos (Productor-consumidor)
                            sem_wait(llenos);
                            sem_wait(mutex);
                            sleep(2);
                            sem_wait(vaciosP);
                            sem_wait(mutexP);
                            pRegistroSangre->bandEntrada = pRegistro->bandEntrada;
                            pRegistroSangre->cantidad = pRegistro->cantidad;
                            pRegistroSangre->ident = pRegistro->ident;
                            pRegistroSangre->tipo = pRegistro->tipo;
                            enterPiel = true;
                            sem_post(mutexP);
                            sem_post(llenosP);

                            sem_post(mutex);
                            sem_post(vacios);
                            break;
                        }
                        else
                        {
                            nPiel++;
                        }
                    }
                    
                
                nPiel = 0;
                //Se borra el registro de la memoria de entrada
                pRegistro->bandEntrada = entrada.bandEntrada;
                pRegistro->cantidad = entrada.cantidad;
                pRegistro->ident = entrada.ident;
                pRegistro->tipo = entrada.tipo;
            }
            //Si hay algun registro adentro se clasifica
            else if (pRegistro->tipo == 'D')
            {       //Se revisa la memoria comaprtida de ditritos
                    while (nDitritos < q)
                    {
                        
                        char *posnDitritos = posIDitritos + (nDitritos * sizeof(struct Entrada));
                        struct Entrada *pRegistroSangre = (struct Entrada *)posnDitritos;
                        //Si se encuentra un lugar para meter el registro
                        if (pRegistroSangre->cantidad <= 0)
                        {
                            
                            //Revisión semaforos (Productor-consumidor)
                            sem_wait(llenos);
                            sem_wait(mutex);
                            
                            sleep(2);
                            sem_wait(vaciosD);
                            sem_wait(mutexD);
                            pRegistroSangre->bandEntrada = pRegistro->bandEntrada;
                            pRegistroSangre->cantidad = pRegistro->cantidad;
                            pRegistroSangre->ident = pRegistro->ident;
                            pRegistroSangre->tipo = pRegistro->tipo;
                            enterDitritos = true;
                            sem_post(mutexD);
                            sem_post(llenosD);

                            sem_post(mutex);
                            sem_post(vacios);
                            break;
                        }
                        else
                        {
                            nDitritos++;
                        }
                    }
                    
                nDitritos = 0;
                //Se borra el registro de la memoria de entrada
                pRegistro->bandEntrada = entrada.bandEntrada;
                pRegistro->cantidad = entrada.cantidad;
                pRegistro->ident = entrada.ident;
                pRegistro->tipo = entrada.tipo;
            }
            m++;
        }
        m = 0;
    }

}

//Metodo para procesar los examenes desde las colas intermedias hasta la de salida
void Procesando::procesado(string nomseg, int bandeja)
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

    //Apertura de diferentes memorias 

    int mem = shm_open(open.c_str(), O_RDWR, 0660);
    int memSangre = shm_open(nombreMemoriaSangre.c_str(), O_RDWR, 0660);
    int memPiel = shm_open(nombreMemoriaPiel.c_str(), O_RDWR, 0660);
    int memDitritos = shm_open(nombreMemoriaDitritos.c_str(), O_RDWR, 0660);
    sem_t *semsangre, *semditritos, *sempiel;
    sem_t *vaciosSalida, *llenosSalida, *mutexSalida, *vaciosS, *llenosS, *mutexS, *vaciosP, *llenosP, *mutexP, *vaciosD, *llenosD, *mutexD;

    semsangre = sem_open(nombreSemSangre.c_str(), 0);
    semditritos = sem_open(nombreSemDitritos.c_str(), 0);
    sempiel = sem_open(nombreSemPiel.c_str(), 0);

    string nombreSemaforoLlenosSalida = nomseg + "Lsalida";
    string nombreSemaforoVaciosSalida = nomseg + "Vsalida";
    string nombreSemaforoMutexSalida = nomseg + "Msalida";

    string nombreSemaforoLlenosS = nomseg + nombres[0] + "Llenos";
    string nombreSemaforoVaciosS = nomseg + nombres[0] + "Vacios";
    string nombreSemaforoMutexS = nomseg + nombres[0] + "Mutex";

    string nombreSemaforoLlenosP = nomseg + nombres[1] + "Llenos";
    string nombreSemaforoVaciosP = nomseg + nombres[1] + "Vacios";
    string nombreSemaforoMutexP = nomseg + nombres[1] + "Mutex";

    string nombreSemaforoLlenosD = nomseg + nombres[2] + "Llenos";
    string nombreSemaforoVaciosD = nomseg + nombres[2] + "Vacios";
    string nombreSemaforoMutexD = nomseg + nombres[2] + "Mutex";

    //Se instancian los semaforos necesarios
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

    char *dir = (char *)mmap(NULL, ((sizeof(struct Entrada) * i * ie) + (sizeof(struct Salida) * oe)), PROT_READ | PROT_WRITE, MAP_SHARED, mem, 0);

    char *dirsalida = dir + (sizeof(struct Entrada) * i * ie);

    int temp = 0, val;
    srand(time(NULL));

    char *posISangre = (q * sizeof(struct Entrada)) + dirMemSangre;
    mSangre = 0;
    int valsem;

    //Condicionales para que los hilos sepan que cola interna recorrer
    if (bandeja == 0)
    {
        //Se inicia el ciclo infinito que hará el recorrido por la memoria de sangre
        for (;;)
        {
            while (mSangre < q)
            {
                char *posnSangre = posISangre + (mSangre * sizeof(struct Entrada));
                struct Entrada *pRegistrosangre = (struct Entrada *)posnSangre;
                //Si encuentra algun registro en la cola de sangre 
                if (pRegistrosangre->cantidad > 0)
                {
                    while (temp < oe)
                    {
                        char *posnsalida = dirsalida + (sizeof(struct Salida) * temp);
                        struct Salida *registrosalida = (struct Salida *)posnsalida;
                        //Si encuentra algun espacio en la cola de salida para meter el regitro
                        if (registrosalida->ident <= 0)
                        {
                            sem_wait(llenosS);
                            sem_wait(mutexS);
                            val = rand() % (50 + 1);
                            //Se verifica si el examen es valido, inconcluso o negativo
                            if (val <= 15 && val >= 0)
                            {
                                registrosalida->result = '?';
                            }
                            else if (val >= 16 && val <= 35)
                            {
                                registrosalida->result = 'N';
                            }
                            else if (val >= 36 && val <= 50)
                            {
                                registrosalida->result = 'P';
                            }
                            //Se retira la cantdad del reactivo que especifica el examen
                            for (int i = 0; i < pRegistrosangre->cantidad; i++)
                            {
                                int ran = 1 + rand() % (7 - 1);
                                for (int j = 0; j < ran; j++)
                                {
                                    sem_wait(semsangre);
                                }
                            }

                            //Revision de semaforos(Productor)
                            sem_wait(vaciosSalida);
                            sem_wait(mutexSalida);
                            registrosalida->ident = pRegistrosangre->ident;
                            registrosalida->tipo = pRegistrosangre->tipo;
                            registrosalida->bandeja = pRegistrosangre->bandEntrada;
                            pRegistrosangre->cantidad = -1;
                            sem_post(mutexSalida);
                            sem_post(llenosSalida);
                            sem_post(mutexS);
                            sem_post(vaciosS);
                            break;
                        }
                        temp++;
                    }
                }
                mSangre++;
            }
            temp = 0;
            mSangre = 0;
        }
    }

    char *posIPiel = (q * sizeof(struct Entrada)) + dirMemPiel;
    mPiel = 0;
    if (bandeja == 1)
    {
        //Se inicia el ciclo infinito que hará el recorrido por la memoria de piel
        for(;;)
        {
            while (mPiel < q)
            {
                sleep(2);
                char *posnPiel = posIPiel + (mPiel * sizeof(struct Entrada));
                struct Entrada *pRegistro = (struct Entrada *)posnPiel;

                if (pRegistro->cantidad > 0)
                {
                    while (temp < oe)
                    {
                        char *posnsalida = dirsalida + (sizeof(struct Salida) * temp);
                        struct Salida *registrosalida = (struct Salida *)posnsalida;
                        if (registrosalida->ident <= 0)
                        {
                            sem_wait(llenosP);
                            sem_wait(mutexP);
                            val = 1 + rand() % (50 + 1);
                            //Se verifica si el examen es valido, inconcluso o negativo
                            if (val <= 15 && val >= 0)
                            {
                                registrosalida->result = '?';
                            }
                            else if (val >= 16 && val <= 35)
                            {
                                registrosalida->result = 'N';
                            }
                            else if (val >= 36 && val <= 50)
                            {
                                registrosalida->result = 'P';
                            }
                            //Se retira la cantdad del reactivo que especifica el examen
                            for (int i = 0; i < pRegistro->cantidad; i++)
                            {
                                int ran = 5 + rand() % (20 - 5);
                                for (int j = 0; j < ran; j++)
                                {
                                    sem_wait(sempiel);
                                }
                            }
                            sem_wait(vaciosSalida);
                            sem_wait(mutexSalida);
                            registrosalida->ident = pRegistro->ident;
                            registrosalida->tipo = pRegistro->tipo;
                            registrosalida->bandeja = pRegistro->bandEntrada;
                            pRegistro->cantidad = -1;
                            sem_post(mutexSalida);
                            sem_post(llenosSalida);
                            sem_post(mutexP);
                            sem_post(vaciosP);
                            break;
                        }
                        temp++;
                    }
                }
                mPiel++;
            }
            temp = 0;
            mPiel = 0;
        }
    }

    char *posIDitritos = (q * sizeof(struct Entrada)) + dirMemDitritos;
    mDitritos = 0;

    if (bandeja == 2)
    {
        //Se inicia el ciclo infinito que hará el recorrido por la memoria de ditritos
        for (;;)
        {
            while (mDitritos < q)
            {
                sleep(2);
                char *posnDitritos = posIDitritos + (mDitritos * sizeof(struct Entrada));
                struct Entrada *pRegistro = (struct Entrada *)posnDitritos;

                if (pRegistro->cantidad > 0)
                {
                    while (temp < oe)
                    {
                        char *posnsalida = dirsalida + (sizeof(struct Salida) * temp);
                        struct Salida *registrosalida = (struct Salida *)posnsalida;
                        if (registrosalida->ident <= 0)
                        {
                            sem_wait(llenosD);
                            sem_wait(mutexD);
                            val = rand() % (50 + 1);
                            //Se verifica si el examen es valido, inconcluso o negativo
                            if (val <= 15 && val >= 0)
                            {
                                registrosalida->result = '?';
                            }
                            else if (val >= 16 && val <= 35)
                            {
                                registrosalida->result = 'N';
                            }
                            else if (val >= 36 && val <= 50)
                            {
                                registrosalida->result = 'P';
                            }
                            //Se retira la cantdad del reactivo que especifica el examen
                            for (int i = 0; i < pRegistro->cantidad; i++)
                            {
                                int ran = 8 + rand() % (25 - 8);
                                for (int j = 0; j < ran; j++)
                                {
                                    sem_wait(semditritos);
                                }
                            }
                            sem_wait(vaciosSalida);
                            sem_wait(mutexSalida);
                            registrosalida->ident = pRegistro->ident;
                            registrosalida->tipo = pRegistro->tipo;
                            registrosalida->bandeja = pRegistro->bandEntrada;
                            pRegistro->cantidad = -1;
                            sem_post(mutexSalida);
                            sem_post(llenosSalida);

                            sem_post(mutexD);
                            sem_post(vaciosD);
                            break;
                        }
                        temp++;
                    }
                }
                mDitritos++;
            }
            temp = 0;
            mDitritos = 0;
        }
    }
}