#include "stdafx.h"
#include "RLE.h"

void RLE::ROW::compress(unsigned char * im, int height, int width)
    {
    std::ofstream output;
    output.open("compress_row.bin", std::ofstream::out | std::ofstream::trunc | std::ofstream::binary);
    output.write((char*) &height, sizeof(height));
    output.write((char*) &width, sizeof(width));

    int i = 1, size = height * width;
    unsigned char rle = 1;
    unsigned char current = im[0];
    bool commited = false;
    for (; i < size; i++)
        {
        commited = false;
        if (rle << 1 == RLE_OVERFLOW) //char overflow
            {
            rle = rle << 1;
            if (current)
                rle++;
            output.write((char *) &rle, sizeof(rle));
            current = im[i];
            rle = 1;
            }

        if (current == im[i])
            {
            rle++;
            }
        else
            {
            rle = rle << 1;
            if (current)
                rle++;
            output.write((char *) &rle, sizeof(rle));
            current = im[i];
            rle = 1;
            commited = true;
            }
        }
    if (!commited)
        {
        rle = rle << 1;
        if (current)
            rle++;
        output.write((char *) &rle, sizeof(rle));
        }
    output.close();
    }

void RLE::ROW::uncompress(unsigned char ** im, int * w, int * h)
    {
    std::ifstream input;
    input.open("compress_row.bin", std::ifstream::in | std::ifstream::binary);

    int height, width;
    input.read((char *) &height, sizeof(height));
    input.read((char *) &width, sizeof(width));

    *w = width;
    *h = height;

    int i = 0, size = height * width;
    *im = (unsigned char *) malloc(sizeof(unsigned char) * size);
    while (!input.eof())
        {
        unsigned char rle;
        input.read((char *) &rle, sizeof(rle));

        unsigned char currentChar = (rle & RLE_MASK) ? 0xff : 0;
        rle = rle >> 1;
        for (int j = 0; j < rle && i < size; j++)
            im[0][i++] = currentChar;
        }
    input.close();
    }

void RLE::COLUMN::compress(unsigned char * im, int height, int width)
    {
    std::ofstream output;
    output.open("compress_column.bin", std::ofstream::out | std::ofstream::trunc | std::ofstream::binary);
    output.write((char*) &height, sizeof(height));
    output.write((char*) &width, sizeof(width));

    int h = 1, w = 0;
    bool commited = false;
    unsigned char rle = 1;
    unsigned char current = im[0];
    for (; w < width; w++)
        {
        for (; h < height; h++)
            {
            commited = false;
            if (rle << 1 == RLE_OVERFLOW)
                {
                rle = rle << 1;
                if (current)
                    rle++;
                output.write((char *) &rle, sizeof(rle));
                current = im[width * h + w];
                rle = 1;
                }
            if (current == im[width * h + w])
                {
                rle++;
                }
            else
                {
                rle = rle << 1;
                if (current)
                    rle++;
                output.write((char *) &rle, sizeof(rle));
                current = im[width * h + w];
                rle = 1;
                commited = true;
                }
            }
        h = 0;
        }
    if (!commited)
        {
        rle = rle << 1;
        if (current)
            rle++;
        output.write((char *) &rle, sizeof(rle));
        }
    output.close();
    }

void RLE::COLUMN::uncompress(unsigned char ** im, int * w, int * h)
    {
    std::ifstream input;
    input.open("compress_column.bin", std::ifstream::in | std::ifstream::binary);

    int height, width;
    input.read((char *) &height, sizeof(height));
    input.read((char *) &width, sizeof(width));

    *w = width;
    *h = height;

    *im = (unsigned char *) malloc(sizeof(unsigned char) * height* width);

    int i = 0;
    int _h = 0, _w = 0;

    bool full = false;

    while (!input.eof() && !full)
        {
        unsigned char rle;
        input.read((char *) &rle, sizeof(rle));

        unsigned char currentChar = (rle & RLE_MASK) ? 0xff : 0;
        rle = rle >> 1;
        for (int j = 0; j < rle; j++)
            {
            if (_h < height &&  _w < width)
                {
                im[0][width * _h + _w] = currentChar;
                _h++;
                if (_h >= height)
                    {
                    _h = 0;
                    _w++;
                    }
                }
            else
                {
                full = true;
                break;
                }
            }
        }
    input.close();
    }

namespace RLE
    {
    namespace PERIMETER
        {
        enum direction
            {
            up,
            down,
            right,
            left
            };

        void getIndex(direction * dir, int* mw, int* mh, int* lw, int* lh, int* c, int* r)
            {
            switch (*dir)
                {
                    case direction::right:
                        if (*c < *mw) (*c)++;
                        else
                            {
                            (*lh)++;
                            *dir = direction::down;
                            getIndex(dir, mw, mh, lw, lh, c, r);
                            } break;
                    case direction::down:
                        if (*r < *mh) (*r)++;
                        else
                            {
                            (*mw)--;
                            *dir = direction::left;
                            getIndex(dir, mw, mh, lw, lh, c, r);
                            } break;
                    case direction::left:
                        if (*c > *lw) (*c)--;
                        else
                            {
                            (*mh)--;
                            *dir = direction::up;
                            getIndex(dir, mw, mh, lw, lh, c, r);
                            } break;
                    case direction::up:
                        if (*r > *lh) (*r)--;
                        else
                            {
                            (*lw)--;
                            *dir = direction::right;
                            getIndex(dir, mw, mh, lw, lh, c, r);
                            } break;
                }
            }
        }
    }

void RLE::PERIMETER::compress(unsigned char * im, int height, int width)
    {
    std::ofstream output;
    output.open("compress_perimeter.bin", std::ofstream::out | std::ofstream::trunc | std::ofstream::binary);

    output.write((char*) &height, sizeof(height));
    output.write((char*) &width, sizeof(width));

    direction direction = direction::right;

    int low_height = 0, max_height = height - 1, low_width = 0, max_width = width - 1;
    int size = height * width;
    int row = 0, column = 1, i = 0;

    bool commited = false;

    unsigned char rle = 1;
    unsigned char current = im[0];

    while (i < size)
        {
        commited = false;
        if (rle << 1 == RLE_OVERFLOW) //char overflow
            {
            rle = rle << 1;
            if (current)
                rle++;
            output.write((char *) &rle, sizeof(rle));
            current = im[width * row + column];
            rle = 1;
            }
        if (current == im[width * row + column])
            {
            rle++;
            }
        else
            {
            rle = rle << 1;
            if (current)
                rle++;
            output.write((char *) &rle, sizeof(rle));
            current = im[width * row + column];
            rle = 1;
            commited = true;
            }

        getIndex(&direction, &max_width, &max_height, &low_width, &low_height, &column, &row);
        i++;
        }

    if (!commited)
        {
        rle = rle << 1;
        if (current)
            rle++;
        output.write((char *) &rle, sizeof(rle));
        }
    output.close();
    }

void RLE::PERIMETER::uncompress(unsigned char ** im, int * w, int * h)
    {
    std::ifstream input;
    input.open("compress_perimeter.bin", std::ifstream::in | std::ifstream::binary);

    int height, width;
    input.read((char *) &height, sizeof(height));
    input.read((char *) &width, sizeof(width));

    *w = width;
    *h = height;

    *im = (unsigned char *) malloc(sizeof(unsigned char) * height* width);

    direction direction = direction::right;
    int low_height = 0, max_height = height - 1, low_width = 0, max_width = width - 1;
    int size = height * width;
    int row = 0, column = 0, i = 0;

    while (!input.eof() && i < size)
        {
        unsigned char rle;
        input.read((char *) &rle, sizeof(rle));

        unsigned char currentChar = (rle & RLE_MASK) ? 0xff : 0;
        rle = rle >> 1;
        for (int j = 0; j < rle; j++)
            {
            im[0][width * row + column] = currentChar;
            getIndex(&direction, &max_width, &max_height, &low_width, &low_height, &column, &row);
            i++;
            }
        }
    input.close();
    }
