#pragma once

#include <string>

using namespace std;

class Ctrl {
public:
    string procesando(string nomseg,int i,int ie,int oe);
    string esperando(string nomseg,int i,int ie,int oe);
    string terminados(string nomseg,int i,int ie,int oe);
    string reactivos(string nomseg,int i,int ie,int oe);
    string all(string nomseg,int i,int ie,int oe);
    void actualizar(string tipomuestra, int valormuestra);
};
