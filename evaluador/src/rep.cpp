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
#include "rep.h"

//Metodo para liberar la memoria
string Rep::liberar(int tiempo, int numeroexamenes, string nomseg)
{
    int oe, i, ie, valsem;

    bool entra = false;

    string cadena = "Reporte Final\n\n";

    string open = "/" + nomseg;

    int mem = shm_open(open.c_str(), O_RDWR, 0660);

    sem_t *vaciosSalida, *llenosSalida, *mutexSalida;

    string nombreSemaforoLlenosSalida = nomseg + "Lsalida";
    string nombreSemaforoVaciosSalida = nomseg + "Vsalida";
    string nombreSemaforoMutexSalida = nomseg + "Msalida";

    //Se abren los semaforos
    vaciosSalida = sem_open(nombreSemaforoVaciosSalida.c_str(), 0);
    llenosSalida = sem_open(nombreSemaforoLlenosSalida.c_str(), 0);
    mutexSalida = sem_open(nombreSemaforoMutexSalida.c_str(), 0);

    if (mem < 0)
    {
        cerr << "Error abriendo la memoria compartida: "
             << errno << strerror(errno) << endl;
        exit(1);
    }

    struct Header *header = (struct Header *)mmap(NULL, sizeof(struct Header), PROT_READ | PROT_WRITE, MAP_SHARED, mem, 0);
    oe = header->oe;
    ie = header->ie;
    i = header->i;

    munmap((void *)header, sizeof(struct Header));

    char *dir = (char *)mmap(NULL, ((sizeof(struct Entrada) * i * ie) + (sizeof(struct Salida)*oe)), PROT_READ | PROT_WRITE, MAP_SHARED, mem, 0);
    
    //Se obtiene la primera posición de la cola de salida
    char *dirsalida = dir + (sizeof(struct Entrada) * i * ie);
    sem_getvalue(llenosSalida, &valsem);
    int temp = 0,tmp1=0;
    
    //Se itera para recorrer la cola de salida
    while (!entra)
    {
        //Solo se entra si la cantidad de examenes en la cola es mayor o igual a la que
        //se necesitan retirar
        if (valsem >= numeroexamenes)
        {
            while (temp < oe && tmp1<numeroexamenes)
            {
                char *posnsalida = dirsalida + (sizeof(struct Salida) * temp);
                struct Salida *registrosalida = (struct Salida *)posnsalida;
                if (registrosalida->ident > 0)
                {
                    sem_wait(llenosSalida);
                    sem_wait(mutexSalida);
                    cadena += to_string(registrosalida->ident) + " " + to_string(registrosalida->bandeja) + " " + registrosalida->tipo + " " + registrosalida->result + "\n";
                    registrosalida->ident = -1;
                    sem_post(mutexSalida);
                    sem_post(vaciosSalida);
                    entra = true;
                    tmp1++;
                }
                temp++;
            }
            temp=0;
        }else{
            //En caso de que no sea mayor o igual se vuelve a obtener el valor del semaforo y se 
            //espera el tiempo dado
            sem_getvalue(llenosSalida, &valsem);
            sleep(tiempo);
        }
    }
    return cadena;
}