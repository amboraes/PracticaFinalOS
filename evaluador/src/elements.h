#pragma once
#include <cstring>

struct Entrada {
  int bandEntrada;
  int cantidad;
  char tipo;
  int ident;
};

struct Salida {
  int ident;
  int bandeja;
  char tipo;
  char result;
};

struct Header {
  int i;
  int ie;
  int oe;
  int q;
  int s;
  int b;
  int d;
  char name[30];
};

struct EstructuraHilo{
  int i;
  char name[30];
};