// lyg_final.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "BitmapFileHandler.h"
#include "RLE.h"
#include <time.h>
#include <string>
#include <Windows.h>

void showHelp()
    {
    printf("Run lenght c++ implementation\n");
    printf("It parses bmp file with 2 rgb colors\n");
    printf("\t -f \t File path of bitmap to parse\n");
    printf("\t -u \t Uncompress file\n");
    printf("\t -g \t Generates random bmp file at given path\n");
    printf("\t -W \t Width of image (DEFAULT 4096)\n");
    printf("\t -H \t Height of image (DEFAULT 4096)\n");
    printf("\t -t \t RLE Parsing type from: (ROW, COLUMN, PERIMETER)\n");
    printf("\t -s \t Show statistics\n");
    printf("\t -h \t Shows this message\n");
    }

int main(int argc, char * argv[])
    {
    bool generateFile = false;
    bool showStats = false;
    bool compress = true;
    char * parseType = nullptr;
    char * filePath = nullptr;
    int width = 4096, height = 4096;
    double duration;

    if (argc == 1)
        {
        showHelp();
        return 0;
        }
    for (int arg = 1; arg < argc; arg++)
        {
        std::string argument = std::string(argv[arg]);

        if (argument == "-u")
            {
            compress = false;
            }
        if (argument == "-f")
            {
            filePath = argv[arg + 1];
            }
        if (argument == "-W")
            {
            width = atoi(argv[arg + 1]);
            }
        if (argument == "-H")
            {
            height = atoi(argv[arg + 1]);
            }
        if (argument == "-g")
            {
            generateFile = true;
            }
        if (argument == "-t")
            {
            parseType = argv[arg + 1];
            }
        if (argument == "-s")
            {
            showStats = true;
            }
        if (argument == "-h")
            {
            showHelp();
            return 0;
            }
        }
    int size = width * height;

    if (filePath == nullptr)
        {
        printf("No file path specified\n");
        showHelp();
        return 0;
        }

    if (parseType == nullptr)
        {
        printf("No parser type specified\n");
        showHelp();
        return 0;
        }

    if (!(std::string(parseType) == "ROW" || std::string(parseType) == "COLUMN" || std::string(parseType) == "PERIMETER"))
        {
        printf("Parser type not found\n");
        showHelp();
        return 0;
        }

    if (generateFile)
        {
        byte * imageFromFile = new byte[size];
        srand(clock());
        for (int i = 0; i < width * height; i++)
            {
            if (rand() % 2)
                imageFromFile[i] = 0;
            else
                imageFromFile[i] = 255;
            }
        bmpFile::WriteToFile(filePath, imageFromFile, width, height);
        delete imageFromFile;
        }

    if (compress)
        {
        byte * imageFromFile = NULL;
        bmpFile::ReadFromFile(filePath, &imageFromFile, &width, &height);
        WIN32_FILE_ATTRIBUTE_DATA binInfo;
        WIN32_FILE_ATTRIBUTE_DATA imageInfo;
        GetFileAttributesEx(filePath, GetFileExInfoStandard, &imageInfo);

        duration = double(clock());
        if (std::string(parseType) == "ROW")
            {
            RLE::ROW::compress(imageFromFile, height, width);
            GetFileAttributesEx("compress_row.bin", GetFileExInfoStandard, &binInfo);
            }
        if (std::string(parseType) == "COLUMN")
            {
            RLE::COLUMN::compress(imageFromFile, height, width);
            GetFileAttributesEx("compress_column.bin", GetFileExInfoStandard, &binInfo);
            }
        if (std::string(parseType) == "PERIMETER")
            {
            RLE::PERIMETER::compress(imageFromFile, height, width);
            GetFileAttributesEx("compress_perimeter.bin", GetFileExInfoStandard, &binInfo);
            }
        duration = double(clock()) - duration;
        if (imageFromFile != NULL)
            {
            bmpFile::WriteToFile(filePath, imageFromFile, width, height);
            free(imageFromFile);
            }
        if (showStats)
            {
            printf("Decompression time using %s - %.4f\n", parseType, duration / CLOCKS_PER_SEC);
            printf("Compression rate %.2f \n", double(binInfo.nFileSizeLow) / imageInfo.nFileSizeLow * 100);
            }
        }
    else
        {
        byte * imageFromFile = NULL;
        duration = double(clock());

        WIN32_FILE_ATTRIBUTE_DATA binInfo;
        WIN32_FILE_ATTRIBUTE_DATA imageInfo;

        if (std::string(parseType) == "ROW")
            {
            RLE::ROW::uncompress(&imageFromFile, &width, &height);
            GetFileAttributesEx("compress_row.bin", GetFileExInfoStandard, &binInfo);
            }
        if (std::string(parseType) == "COLUMN")
            {
            RLE::COLUMN::uncompress(&imageFromFile, &width, &height);
            GetFileAttributesEx("compress_column.bin", GetFileExInfoStandard, &binInfo);
            }
        if (std::string(parseType) == "PERIMETER")
            {
            RLE::PERIMETER::uncompress(&imageFromFile, &width, &height);
            GetFileAttributesEx("compress_perimeter.bin", GetFileExInfoStandard, &binInfo);
            }
        duration = double(clock()) - duration;
        if (imageFromFile != NULL)
            {
            bmpFile::WriteToFile(filePath, imageFromFile, width, height);
            GetFileAttributesEx(filePath, GetFileExInfoStandard, &imageInfo);
            free(imageFromFile);
            }
        if (showStats)
            {
            printf("Decompression time using %s - %.4f\n", parseType, duration / CLOCKS_PER_SEC);
            printf("Compression rate %.2f \n", double(binInfo.nFileSizeLow)/ imageInfo.nFileSizeLow * 100);
            }
            
        }

    return 0;
    }
