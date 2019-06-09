#pragma once

#include <string>
#include <map>

using namespace std;

class Bandejas {
public:
    void dividirMemoria(string nomseg);
    char* getPos(int i);

private:
    char **ban;
};