#pragma once
#include <cstring>

struct Entrada {
  int bandEntrada;
  int cantidad;
  char tipo;
  int ident;
};

struct Salida {
  int tamano;
  struct  Entrada examenes[];
  
};

