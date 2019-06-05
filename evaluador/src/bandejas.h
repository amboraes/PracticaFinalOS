#pragma once

#include <string>
#include <map>

using namespace std;

class Bandejas {
public:
    void dividirMemoria(string nombreSeg);
    char * getPos(int i);
};