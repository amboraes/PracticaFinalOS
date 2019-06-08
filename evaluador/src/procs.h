#pragma once

#include <string>
#include <map>

using namespace std;

class Procs {
public:
    Procs(int i, string nomSeg);
    Procs* getInstance(int i, string nomSeg);
    //char * get(int i);
private:
    
};

