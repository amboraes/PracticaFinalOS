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

using namespace std;

int main(int argc, char *argv[])
{
    if (strcmp(argv[1], "init") == 0)
    {
        if (argv[2] != NULL)
        {
            if (strcmp(argv[2],"-i") == 0)
            {
                if (argv[3] != NULL)
                {
                    cout << argv[1] << " " << argv[2] << " " << argv[3] << endl;
                }
            }
            else if (strcmp(argv[2],"-ie") == 0)
            {
                if (argv[3] != NULL)
                {
                    cout << argv[1] << " " << argv[2] << " " << argv[3] << endl;
                }
            }
            else if (strcmp(argv[2],"-oe") == 0)
            {
                if (argv[3] != NULL)
                {
                    cout << argv[1] << " " << argv[2] << " " << argv[3] << endl;
                }
            }
            else if (strcmp(argv[2],"-n") == 0)
            {
                if (argv[3] != NULL)
                {
                    cout << argv[1] << " " << argv[2] << " " << argv[3] << endl;
                }
            }
            else if (strcmp(argv[2],"-b") == 0)
            {
                if (argv[3] != NULL)
                {
                    cout << argv[1] << " " << argv[2] << " " << argv[3] << endl;
                }
            }
            else if (strcmp(argv[2],"-d") == 0)
            {
                if (argv[3] != NULL)
                {
                    cout << argv[1] << " " << argv[2] << " " << argv[3] << endl;
                }
            }
            else if (strcmp(argv[2],"-ee") == 0)
            {
                if (argv[3] != NULL)
                {
                    cout << argv[1] << " " << argv[2] << " " << argv[3] << endl;
                }
            }
            else if (strcmp(argv[2],"-s") == 0)
            {
                if (argv[3] != NULL)
                {
                    cout << argv[1] << " " << argv[2] << " " << argv[3] << endl;
                }
            }
            else if (strcmp(argv[2],"-q") == 0)
            {
                if (argv[3] != NULL)
                {
                    cout << argv[1] << " " << argv[2] << " " << argv[3] << endl;
                }
            }
            else
            {
                cerr << "Argumento invalido" << endl;
                exit(1);
            }
        }
        else
        {
            cerr << "Se esperaban mas argumentos" << endl;
            exit(1);
        }
    }
    else if (strcmp(argv[1], "reg") == 0)
    {
        if (argv[2] != NULL)
        {
            if (strcmp(argv[2], "-n") == 0)
            {
                if (argv[3] != NULL)
                {
                    string memoriaCompartida = argv[3];
                    cout << memoriaCompartida << endl;
                    if (argv[4] != NULL)
                    {
                        ifstream archivoEval;
                        archivoEval.open(argv[4]);
                        if (!archivoEval && isdigit(argv[4][0]))
                        {
                            if (argv[5] != NULL)
                            {
                                if (strcmp(argv[5], "B") == 0)
                                {
                                    cout << "tipo" << argv[5] << endl;
                                    if (argv[6] != NULL)
                                    {
                                        cout << argv[1] << " " << argv[2] << " " << argv[3] << " " << argv[4] << " " << argv[5] << " " << argv[6] << endl;
                                    }
                                }
                                else if (strcmp(argv[5], "D") == 0)
                                {
                                    cout << "tipo" << argv[5] << endl;
                                    if (argv[6] != NULL)
                                    {
                                        cout << argv[1] << " " << argv[2] << " " << argv[3] << " " << argv[4] << " " << argv[5] << " " << argv[6] << endl;
                                    }
                                }
                                else if (strcmp(argv[5], "S") == 0)
                                {
                                    cout << "tipo" << argv[5] << endl;
                                    if (argv[6] != NULL)
                                    {
                                        cout << argv[1] << " " << argv[2] << " " << argv[3] << " " << argv[4] << " " << argv[5] << " " << argv[6] << endl;
                                    }
                                }
                                else
                                {
                                    cerr << "Argumento invalido" << endl;
                                    exit(1);
                                }
                            }
                        }
                        else if (!archivoEval && !isdigit(argv[4][0]))
                        {
                            cerr << "Unable to open file";
                            exit(1);
                        }
                        else if (archivoEval)
                        {
                            string comprobar = argv[4];
                            if (comprobar.find(".extension") != std::string::npos)
                            {
                                string archivoEntrada = argv[4];
                                int posicion = archivoEntrada.find_first_of(".");
                                string nombreArchivo = archivoEntrada.substr(0, posicion);
                                string archivoSample = nombreArchivo.append(".spl");
                            }
                        }
                    }
                }
                else
                {
                    cerr << "Argumento invalido" << endl;
                    exit(1);
                }
            }
            else
            {
                cerr << "Se esperaban mas argumentos" << endl;
                exit(1);
            }
        }
        else if (strcmp(argv[1], "ctrl") == 0)
        {
            if (argv[2] != NULL)
            {
            }
            else
            {
                cerr << "Se esperaban mas argumentos" << endl;
                exit(1);
            }
        }
        else if (strcmp(argv[1], "rep") == 0)
        {
            if (argv[2] != NULL)
            {
            }
            else
            {
                cerr << "Se esperaban mas argumentos" << endl;
                exit(1);
            }
        }
    }
}
