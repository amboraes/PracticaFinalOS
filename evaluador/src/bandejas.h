#pragma once

#include <string>
#include <map>

using namespace std;

class Bandejas {
public:
    Bandejas(string nomseg);
    void dividirMemoria();
    char * getPos(int i);

private:
    string nomseg;
    char **ban[];
};