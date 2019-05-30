#include <fcntl.h>
#include <cerrno>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <semaphore.h>
#include <iostream>
#include <cstdlib>
#include <cstring>

using namespace std; 

class Init{
    
    int inicializar ( char* n ){
        int mem = shm_open(n, O_RDWR | O_CREAT | O_EXCL, 0660);
        if (mem < 0){
            cerr << "Error creando la memoria compartida: "
	        << errno << strerror(errno) << endl;
            exit(1);
        }

        //Truncate? 
    }

};