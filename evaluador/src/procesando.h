#pragma once

#include "elements.h"
#include "procesando.h"
#include <string>
#include <vector>

using namespace std;

class Procesando {
public:
    void procesar(string nomseg);
    string procesando(string nomseg);
    vector<struct Entrada *> piel;
    vector<struct Entrada *> ditritos;
    vector<struct Entrada *> sangre;
};
