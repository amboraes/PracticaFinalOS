#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <iostream>
#include <cstdlib>
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

int main(int argc, char *argv[])
{
    //void *dir;
    Procesando procesar;
    map<string, int> idMemSeg;
    vector <string> options={"-i","-ie","-oe","-n","-b","-d","-ee","-s","-q"};
    vector <int> ids;
    int numentradas = 5,numeropos=6, entradasCola = 10, reactSangre = 100,
    reactDetritos = 100, reactPiel = 100, sizeInternas = 6;
    string nombreSeg = "evaluator";
    string command = argv[1];

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
        int bandeja, cantmuestra,ident;
        if(strcmp(argv[2],"-n")==0){
            nomsegmem= argv[3];

            //cout << nomsegmem << endl;

        }
        //cout << "aca" << endl;
        if(strcmp(argv[4], "-") == 0){
            cout << "> ";
            while(cin>>bandeja>>tipomuestra>>cantmuestra){
                ident = rand();
                if(tipomuestra== "B" or tipomuestra== "D" or tipomuestra== "S" && (0<cantmuestra<=5)){
                    vector<int>::iterator tempo = find(ids.begin(),ids.end(),ident);
                    while (tempo != ids.end()){
                        vector<int>::iterator tempo = find(ids.begin(),ids.end(),ident);
                        ident = rand();
                    }
                    ids.push_back(ident);
                    cout << ident << endl;
                    reg.registrar(nomsegmem,bandeja,*tipomuestra.c_str(),cantmuestra,ident);
                    procesar.procesar(nomsegmem);

                    cout << "> ";
                }else{
                    cout << ">";
                }
            }
            //printf("antes de llamar procesar");
            //cout << &procesar << " espacio x2 " << endl;
            
            //cout << procesar.sangre.size() << endl;
        }else {
            for (int i = 4; i < argc ;i++){
                file.open(argv[i]);
                nomarchivo = argv[i];
                nomarchivo = nomarchivo.substr(0,nomarchivo.find("."));
                string line;
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
                    ident = rand();
                    cout << ident << endl;
                    contarchivo += to_string(ident)+"\n";
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
                           cout << procesar.sangre.size()<< endl;
                           resultado = ctrl.procesando(name,procesar);
                           
                       }
                       else if(result.at(i+1) == "waiting"){
                            resultado = ctrl.esperando(name);
                       }
                       else if(result.at(i+1) == "reported"){
                            resultado = ctrl.reactivos(name);
                       }
                       else if(result.at(i+1) == "reactive"){
                            resultado = ctrl.reactivos(name);
                       }
                       else if(result.at(i+1) == "all"){
                            resultado = ctrl.all(name);
                       }
                    }
                   else if (result.at(i) == "update"){
                        tipomuestra = result.at(i+1);
                        valormuestra = stoi(result.at(i+2));
                        ctrl.actualizar(tipomuestra,valormuestra);
                    }
                }
                cout <<"> ";
            }
        }
    }

    if(command == "rep"){
        string name,opcion;
        int valor,valorsleep;
        if(strcmp(argv[2],"-n")==0){
            name = argv[3];
            cout << "> ";
            while(cin >>opcion>>valor){
                if(opcion == "-i"){
                    valorsleep=valor;
                    //para lo que tiene que esperar se hace
                    //sleep(rand()%valorsleep +1);
                }
                if(opcion == "-m"){
                    //esperar hasta que el tamaño de la cola de examenes sea igual a valor
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
