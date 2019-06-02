#pragma once

struct elemento {
  int elemento;
};

struct Buffer {
  int entra;
  int sale;
  int cantidad;
  int tamano;
  struct elemento buffer[];
};