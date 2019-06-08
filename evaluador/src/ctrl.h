#pragma once

#include <string>
#include "procesando.h"

using namespace std;

class Ctrl {
public:
    string procesando(string nomseg);
    string esperando(string nomseg);
    string terminados(string nomseg);
    string reactivos(string nomseg);
    string all(string nomseg);
    void actualizar(string nomseg,string tipomuestra, int valormuestra);
};
