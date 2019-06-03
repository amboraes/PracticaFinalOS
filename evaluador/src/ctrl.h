#pragma once

#include <string>

using namespace std; 

class Ctrl {
public:
    string procesando();
    string esperando();
    string terminados();
    string reactivos();
    string all();
    void actualizar(string tipomuestra, int valormuestra);
};