#include "stdafx.h"
#include "BitmapFileHandler.h"

using namespace std;
namespace bmpFile
    {
    void WriteToFile(const char * fileName, byte* image, int width, int height)
        {
        ofstream bmpfile;
        bmpfile.open(fileName, ios::out | ios::binary | ios::trunc);
        bmpfile.clear();
        if (!bmpfile.is_open())
            {
            cout << "ERROR: FILE COULD NOT BE OPENED" << endl;
            return;
            }

        BITMAPFILEHEADER fHeader;   //File Header 
        fHeader.bfType = ID_BMP;    //BM
        fHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 2 + sizeof(byte)*width*height;   //File Size info
        fHeader.bfReserved1 = 0;
        fHeader.bfReserved2 = 0;
        fHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 2;//Where we actually start writing the Bitmap data/image

        bmpfile.write((char*) (&fHeader.bfType), sizeof(fHeader.bfType));     //Let's start writing the file
        bmpfile.write((char*) (&fHeader.bfSize), sizeof(fHeader.bfSize));   //Header information
        bmpfile.write((char*) (&fHeader.bfReserved1), sizeof(fHeader.bfReserved1));
        bmpfile.write((char*) (&fHeader.bfReserved2), sizeof(fHeader.bfReserved2));
        bmpfile.write((char*) (&fHeader.bfOffBits), sizeof(fHeader.bfOffBits));

        BITMAPINFOHEADER iHeader;
        iHeader.biSize = sizeof(BITMAPINFOHEADER);
        iHeader.biWidth = width;
        iHeader.biHeight = height;
        iHeader.biPlanes = 1;
        iHeader.biBitCount = 1;
        iHeader.biCompression = BI_RGB;
        iHeader.biSizeImage = 0;
        iHeader.biXPelsPerMeter = 0;
        iHeader.biYPelsPerMeter = 0;
        iHeader.biClrUsed = 0;
        iHeader.biClrImportant = 0;

        RGBQUAD aColors[2];
        aColors[0].rgbBlue = 0;
        aColors[0].rgbGreen = 0;
        aColors[0].rgbRed = 255;
        aColors[0].rgbReserved = 0;

        aColors[1].rgbBlue = 255;
        aColors[1].rgbGreen = 255;
        aColors[1].rgbRed = 0;
        aColors[1].rgbReserved = 0;

        bmpfile.write((char*) &iHeader, sizeof(BITMAPINFOHEADER));   //More Header information being placed in the file
        bmpfile.write((char*) &aColors, sizeof(RGBQUAD) * 2);

        //Write our arrays to the file
        for (int i = 0; i < height*width; i++)
            {
            bmpfile.write((char*) &image[i], sizeof(byte));
            }
        // remove our temp image array

        bmpfile.flush();
        bmpfile.close();
        }

    void ReadFromFile(const char * fileName, byte ** image, int * width, int * height)
        {
        int off = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
        BITMAPFILEHEADER iHeader;
        BITMAPINFOHEADER fHeader;
        RGBQUAD aColors[2];
        ifstream bmpfile;
        bmpfile.open(fileName, ios::in | ios::binary);

        if (!bmpfile.is_open())
            {
            cout << "ERROR: FILE COULD NOT BE OPENED" << endl;
            return;
            }

        bmpfile.read((char*) &iHeader, sizeof(BITMAPFILEHEADER));
        bmpfile.read((char*) &fHeader, sizeof(BITMAPINFOHEADER));
        bmpfile.read((char*) &aColors, sizeof(RGBQUAD) * 2);

        *width = fHeader.biWidth;
        *height = fHeader.biHeight;
        
        int i = 0, size = (*width) * (*height);
        *image = (byte*) malloc(sizeof(byte) * size);
        while (!bmpfile.eof() && i < size)
            {
            image[0][i++] = bmpfile.get();
            }
        }
    }