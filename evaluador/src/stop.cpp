#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <semaphore.h>
#include <string>
#include "stop.h"
using namespace std;

void Stop::borrar(string memseg){
    sem_unlink("Sangre");
    sem_unlink("Piel");
    sem_unlink("Ditritos");
    shm_unlink('/'+memseg.c_str());
    shm_unlink(memseg.c_str());
}