#pragma once
#include "stdafx.h"
#include <fstream>

#ifndef _RLE
#define _RLE

namespace RLE
    {
    const unsigned char RLE_OVERFLOW = 0xfe;
    const unsigned char RLE_MASK = 1;

    namespace ROW
        {
        void compress(unsigned char * im, int height, int width);
        void uncompress(unsigned char ** im, int *w, int * h);
        }

    namespace COLUMN
        {
        void compress(unsigned char * im, int height, int width);
        void uncompress(unsigned char ** im, int *w, int * h);
        }

    namespace PERIMETER
        {
        void compress(unsigned char * im, int height, int width);
        void uncompress(unsigned char ** im, int *w, int * h);
        }
    }

#endif