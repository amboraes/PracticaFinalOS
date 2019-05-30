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


using namespace std;

vector <string> options={"-i","-ie","-oe","-n","-b","-d","-ee","-s","-q"};

int main(int argc, char *argv[])
{
    int numentradas = 5,numeropos=6, entradasCola = 10, reactSangre = 100, reactDetritos = 100, reactPiel = 100, sizeInternas = 6; 
    string nombreSeg;
    nombreSeg = "evaluator";
    
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
            }else if (strcmp(argv[i],"-ee")==0){
                
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
    
    }
    string nomsegmem,nomarchivo,contarchivo;
    ifstream file;
    ofstream file2;

    if(command == "reg"){
        int bandeja,totalexamenes,ident;
        string tipomuestra;
        for (int i = 4; i < argc ;i++){
            if(strcmp(argv[2],"-n")==0){
                nomsegmem= argv[3];
                //cout << nomsegmem << endl;
            }
            
            file.open(argv[i]);
            nomarchivo = argv[i];
            nomarchivo = nomarchivo.substr(0,nomarchivo.find("."));
            string line;
            while(getline(file,line)){
                istringstream iss(line);
                iss>>bandeja>>tipomuestra>>totalexamenes;
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
                
                contarchivo = to_string(ident)+"\n";
            }
            file.close();
            file2.open(nomarchivo+".spl");
            file2 << contarchivo;
            file2.close();

        }   
    }

    if(command == "ctrl"){

    } 

    if(command == "rep"){

    }   
}
