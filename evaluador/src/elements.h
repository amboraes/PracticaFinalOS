#pragma once
#include <cstring>

//Estructura de registros de entrada
struct Entrada {
  int bandEntrada;
  int cantidad;
  char tipo;
  int ident;
};

//Estructura de registros de salida
struct Salida {
  int ident;
  int bandeja;
  char tipo;
  char result;
};

//Estructura para header 
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

//Estructura auxiliar para el manejo de hilos 
struct EstructuraHilo{
  int i;
  char name[30];
};