#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <Windows.h>
#include <wingdi.h>
#include <ctime>

#ifndef ID_BMP
#define ID_BMP 0x4D42
#endif 

namespace bmpFile
    {
    void WriteToFile(const char * fileName, byte* image, int width, int height);
    void ReadFromFile(const char * fileName, byte** image, int* width, int* height);
    }