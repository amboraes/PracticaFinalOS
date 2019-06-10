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

//Lectura de comandos ingresados en consola 

int main(int argc, char *argv[])
{

    map<string, int> idMemSeg;
    vector <string> options={"-i","-ie","-oe","-n","-b","-d","-ee","-s","-q"};
    vector <int> ids;
    //Inicialización de variables con valores por default 
    int numentradas = 5,numeropos=6, entradasCola = 10, reactSangre = 100,
    reactDetritos = 100, reactPiel = 100, sizeInternas = 6;
    string nombreSeg = "evaluator";
    string command = argv[1];
    
    //Inicio del comando de inicialización al ingresar "init" como comando
    if(command=="init"){
        for (int i = 2; i < argc ;i++){
            vector<string>::iterator tempo = find(options.begin(),options.end(),argv[i]);
            //Revisión de opciones válidas para el comando "init"
            if(tempo==options.end()){
                cerr <<" Argumento invalido "<<endl;
                exit(1);
            }
            //Opción que determina cuantas colas de entrada se van a crear
            if(strcmp(argv[i],"-i")==0){
                numentradas= stoi(argv[i+1]);
                i++;
            //Opción que determina cuantas posiciones va a tener cada cola de entrada 
            }else if (strcmp(argv[i],"-ie")==0){
                numeropos=stoi(argv[i+1]);
                i++;
            //Opción que determina cuantas posiciones va a tener la cola de salida
            }else if (strcmp(argv[i],"-oe")==0){
                entradasCola=stoi(argv[i+1]);
                i++;
            //Opción que determina el nombre 
            }else if (strcmp(argv[i],"-n")==0){
                nombreSeg=argv[i+1];
                i++;
            }else if (strcmp(argv[i],"-b")==0){
                reactSangre=stoi(argv[i+1]);
                i++;
            }else if (strcmp(argv[i],"-d")==0){
                reactDetritos=stoi(argv[i+1]);
                i++;
            }else if (strcmp(argv[i],"-s")==0){
                reactPiel=stoi(argv[i+1]);
                i++;
            }else if (strcmp(argv[i],"-q")==0){
                sizeInternas=stoi(argv[i+1]);
                i++;
            }
        }
        Init init;
        init.inicializar(numentradas,numeropos,entradasCola,nombreSeg,reactSangre,reactDetritos,reactPiel,sizeInternas);
        for(;;){};
    }

    if(command == "reg"){
        Reg reg;
        string nomsegmem,nomarchivo,contarchivo,tipomuestra;
        ifstream file;
        ofstream file2;
        int bandeja, cantmuestra,ident,verificacion;
        if(strcmp(argv[2],"-n")==0){
            nomsegmem= argv[3];
            string open = "/" + nomsegmem;
            int mem = shm_open(nombreSeg.c_str(), O_RDWR | O_CREAT | O_EXCL, 0660);
            struct Header *header = (struct Header *) mmap(NULL, sizeof(struct Header), PROT_READ | PROT_WRITE, MAP_SHARED, mem, 0);
            header->i = verificacion;
        }
        if(strcmp(argv[4], "-") == 0){
            cout << "> ";
            while(cin>>bandeja>>tipomuestra>>cantmuestra){
                srand(time(NULL));
                ident = rand();
                if((tipomuestra== "B" || tipomuestra== "D" || tipomuestra== "S") && (0<cantmuestra<=5) && (bandeja<verificacion)){
                    ident = rand()%RAND_MAX;
                    ids.push_back(ident);
                    reg.registrar(nomsegmem,bandeja,*tipomuestra.c_str(),cantmuestra,ident);
                    cout << ident << endl;
                    cout << "> ";
                    
                }else{
                    cout << "> ";

                }
            }
        }else {
            for (int i = 4; i < argc; i++){
                file.open(argv[i]);
                nomarchivo = argv[i];
                nomarchivo = nomarchivo.substr(0,nomarchivo.find("."));
                string line;
                while(getline(file,line)){
                    istringstream iss(line);
                    iss>>bandeja>>tipomuestra>>cantmuestra;
                    
                    srand(time(NULL));
                    ident = rand()%RAND_MAX;
                    
                    ids.push_back(ident);
                    cout << ident << endl;
                    contarchivo += to_string(ident)+"\n";
                    sleep(1);
                    reg.registrar(nomsegmem,bandeja,*tipomuestra.c_str(),cantmuestra,ident);
                }
                file.close();
                file2.open("../examples/" + nomarchivo+".spl");
                file2 << contarchivo;
                file2.close();
            }

        }
    }

    if(command == "ctrl"){
        Ctrl ctrl;
        string name,tmp,tipomuestra;
        int valormuestra;
        vector<string>temp;
        if(strcmp(argv[2],"-n")==0){
            name = argv[3];
            cout <<"> ";
            while(cin){
                getline(cin, tmp);
                regex ws_re("\\s+");
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


