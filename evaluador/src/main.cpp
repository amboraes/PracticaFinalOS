#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <iostream>
#include <cstdlib>
#include <ctime>  
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <fstream>
#include <vector>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include <stdlib.h>
#include <time.h>
#include <regex>
#include <map>
#include "init.h"
#include "reg.h"
#include "ctrl.h"
#include "rep.h"
#include "stop.h"
#include "procesando.h"


using namespace std;

void* wrapperProcesar(void* arg){
    Procesando procesaraux;
    struct EstructuraHilo* est = (struct EstructuraHilo *) arg;
    //cout << est->name << endl;
    procesaraux.procesar(est->name,est->i);
    procesaraux.procesado(est->name);
    return NULL;
}

void* wrapperExit(void* arg){

}

int main(int argc, char *argv[])
{
    Procesando procesar;
    map<string, int> idMemSeg;
    vector <string> options={"-i","-ie","-oe","-n","-b","-d","-ee","-s","-q"};
    vector <int> ids;
    int numentradas = 5,numeropos=6, entradasCola = 10, reactSangre = 100,
    reactDetritos = 100, reactPiel = 100, sizeInternas = 6;
    string nombreSeg = "evaluator";
    string command = argv[1];
    pthread_t *procs;
    

    if(command=="init"){
        cout << "enter" << endl;
        for (int i = 2; i < argc ;i++){
            vector<string>::iterator tempo = find(options.begin(),options.end(),argv[i]);
            if(tempo==options.end()){
                cerr <<" Argumento invalido "<<endl;
                exit(1);
            }
            if(strcmp(argv[i],"-i")==0){
                numentradas= stoi(argv[i+1]);
                cout << numentradas << endl;
                //options.erase(remove(options.begin(),options.end(),argv[i]),options.end());
                i++;
            }else if (strcmp(argv[i],"-ie")==0){
                numeropos=stoi(argv[i+1]);
                cout << numeropos << endl;
                i++;
            }else if (strcmp(argv[i],"-oe")==0){
                entradasCola=stoi(argv[i+1]);
                cout << entradasCola << endl;
                i++;
            }else if (strcmp(argv[i],"-n")==0){
                nombreSeg=argv[i+1];
                cout << nombreSeg << endl;
                i++;
            }else if (strcmp(argv[i],"-b")==0){
                reactSangre=stoi(argv[i+1]);
                cout << reactSangre << endl;
                i++;
            }else if (strcmp(argv[i],"-d")==0){
                reactDetritos=stoi(argv[i+1]);
                cout << reactDetritos << endl;
                i++;
            }else if (strcmp(argv[i],"-s")==0){
                reactPiel=stoi(argv[i+1]);
                cout << reactPiel << endl;
                i++;
            }else if (strcmp(argv[i],"-q")==0){
                sizeInternas=stoi(argv[i+1]);
                cout << sizeInternas << endl;
                i++;
            }
        }
        Init init;
        init.inicializar(numentradas,numeropos,entradasCola,nombreSeg,reactSangre,reactDetritos,reactPiel,sizeInternas);
    }

    if(command == "reg"){
        Reg reg;
        string nomsegmem,nomarchivo,contarchivo,tipomuestra;
        ifstream file;
        ofstream file2;
        int i = 0;
        int bandeja, cantmuestra,ident;
        if(strcmp(argv[2],"-n")==0){
            nomsegmem= argv[3];
            string open = "/" + nomsegmem;
            int mem = shm_open(open.c_str(), O_RDWR, 0660);
            struct Header *header =(struct Header *) mmap(NULL, sizeof(struct Header), PROT_READ | PROT_WRITE, MAP_SHARED, mem, 0);
            i = header->i;
            munmap((void *) header, sizeof(struct Header));
        }
        //cout << "aca" << endl;
        if(strcmp(argv[4], "-") == 0){
            cout << "> ";
            pthread_t hiloprocesar[i];
            while(cin>>bandeja>>tipomuestra>>cantmuestra){
                srand(time(NULL));
                ident = rand();
                //printf("fuckme\n" );

                if((tipomuestra== "B" || tipomuestra== "D" || tipomuestra== "S") && (0<cantmuestra<=5)){
                    /* vector<int>::iterator tempo = find(ids.begin(),ids.end(),ident);
                    while (tempo != ids.end()){
                        vector
                    }
                    <int>::iterator tempo = find(ids.begin(),ids.end(),ident);
                    */
                    ident = rand()%RAND_MAX;
                    ids.push_back(ident);
                    cout << ident << endl;
                    reg.registrar(nomsegmem,bandeja,*tipomuestra.c_str(),cantmuestra,ident);
                    struct EstructuraHilo estructura;
                    //estructura.i = bandeja;
                    strcpy(estructura.name,nomsegmem.c_str());
                    pthread_create(&hiloprocesar[0],NULL,wrapperProcesar,&estructura);

                    for(int j=0;j<i;j++){
                        estructura.i = j;
                        pthread_create(&hiloprocesar[j],NULL,wrapperProcesar,&estructura);
                    }

                    //for(int k = 0; k < 3; k++){
                        //pthread_create(procs, NULL, Procesando::procesado , &estructura );
                    //}
                    //wrapperProcesar(&nomsegmem);
                    //procesar.procesar(nomsegmem,bandeja);
                    //procesar.procesado(nomsegmem);

                    cout << "> ";
                    
                }else{
                    cout << "> ";

                }
            }
            //printf("antes de llamar procesar");
            //cout << &procesar << " espacio x2 " << endl;

            //cout << procesar.sangre.size() << endl;
        }else {
            for (int q = 4; q < argc ;q++){
                file.open(argv[q]);
                nomarchivo = argv[q];
                nomarchivo = nomarchivo.substr(0,nomarchivo.find("."));
                string line;
                pthread_t hiloprocesar[i];
                while(getline(file,line)){
                    istringstream iss(line);
                    iss>>bandeja>>tipomuestra>>cantmuestra;
                    /*if(bandeja<0 and bandeja>numentradas){
                        cerr << "Error en el numero de bandeja de entrada" << endl;
                        exit(1);
                    }
                    if(tipomuestra!="B" and tipomuestra!="D" and tipomuestra!="S"){
                        cerr << "Error con el tipo de muestra" << endl;
                        exit(1);
                    }
                    if(bandeja<0 and bandeja>numentradas){
                        cerr << "Error con la cantidad de muestras" << endl;
                        exit(1);
                    }*/
                    srand(time(NULL));
                    ident = rand()%RAND_MAX;
                    /* vector<int>::iterator tempo = find(ids.begin(),ids.end(),ident);
                    while (tempo != ids.end()){
                        ident = rand();
                        vector<int>::iterator tempo = find(ids.begin(),ids.end(),ident);
                    }*/
                    ids.push_back(ident);
                    cout << ident << endl;
                    contarchivo += to_string(ident)+"\n";
                    cout << nomsegmem << endl;
                    reg.registrar(nomsegmem,bandeja,*tipomuestra.c_str(),cantmuestra,ident);
                    //struct EstructuraHilo estructura;
                    //estructura.i = bandeja;
                    //strcpy(estructura.name,nomsegmem.c_str());
                    //pthread_create(&hiloprocesar[0],NULL,wrapperProcesar,&estructura);

                    //for(int j=0;j<i;j++){
                    //    estructura.i = j;
                    //    pthread_create(&hiloprocesar[j],NULL,wrapperProcesar,&estructura);
                    //}
                    procesar.procesar(nomsegmem,bandeja);
                    procesar.procesado(nomsegmem);
                }
                file.close();
                file2.open("../examples/" + nomarchivo+".spl");
                file2 << contarchivo;
                file2.close();
            }

        }
    }

    if(command == "ctrl"){
       // cout << "antes de instanciar ctrl " << procesar.sangre.size()<< endl;
        Ctrl ctrl;
        string name,tmp,tipomuestra;
        int valormuestra;
        vector<string>temp;
        if(strcmp(argv[2],"-n")==0){
            name = argv[3];
            cout <<"> ";
            while(cin){
                getline(cin, tmp);
                regex ws_re("\\s+"); // whitespace
                vector<string> result{
                    sregex_token_iterator(tmp.begin(), tmp.end(), ws_re, -1), {}
                };
                for(int i = 0; i<result.size(); i++){
                   if(result.at(i) == "list"){
                       string resultado;
                       if(result.at(i+1) == "processing"){
                           resultado = ctrl.procesando(name);
                           result.erase(result.begin()+1);
                           cout << resultado << endl;
                        }
                       else if(result.at(i+1) == "waiting"){
                            resultado = ctrl.esperando(name);
                            result.erase(result.begin()+1);
                            cout << resultado << endl;
                        }
                       else if(result.at(i+1) == "reported"){
                            resultado = ctrl.terminados(name);
                            result.erase(result.begin()+1);
                            cout << resultado << endl;
                        }
                       else if(result.at(i+1) == "reactive"){
                            resultado = ctrl.reactivos(name);
                            result.erase(result.begin()+1);
                            cout << resultado << endl;
                        }
                       else if(result.at(i+1) == "all"){
                            resultado = ctrl.all(name);
                            result.erase(result.begin()+1);
                            cout << resultado << endl;
                        }
                    }else if (result.at(i) == "update"){
                        tipomuestra = result.at(i+1);
                        valormuestra = stoi(result.at(i+2));
                        ctrl.actualizar(name,tipomuestra,valormuestra);
                    }else if(result.at(i) != "update" && result.at(i) != "list" && !EOF){
                        cerr << "Sub comando Erroneo" << endl;
                        exit(1);
                    }
                }
                cout <<"> ";
            }
        }
    }

    if(command == "rep"){
        string name,opcion,salida;
        int valor=0,valorsleep=0,cantexam=0;
        Rep rep;
        if(strcmp(argv[2],"-n")==0){
            name = argv[3];
            cout << "> ";
            while(cin >>opcion>>valor){
                if(opcion == "-i"){
                    valorsleep=valor;
                }
                
                if(opcion == "-m"){
                    cantexam=valor;
                }
                if(cantexam>0 && valorsleep>0){
                    salida = rep.liberar(valorsleep,cantexam,name);
                    cout << salida << endl;
                    valorsleep =0;
                    cantexam=0;
                }
                cout << "> ";
            }
        }
    }
    if(command == "stop"){
        string name;
        Stop stop;
        if(strcmp(argv[2],"-n")==0){
            name = argv[3];
            stop.borrar(name);
        }
    }
}
