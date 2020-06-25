#pragma once

#define PNG_DEBUG 3
#include <png.h>

class CPNGHelper
{
public:
    CPNGHelper(void);
    CPNGHelper(CPNGHelper& obj);
    CPNGHelper(float w,float h, png_byte color_type, png_byte bit_depth);
    ~CPNGHelper(void);

    bool LoadData(const char *file_name);
    bool UnLoadData(const char *file_name);
    bool WriteData(const char *file_name);
    bool CPNGHelper::WriteData(const char *file_name, unsigned char**data,  png_byte color_type_in, png_byte bit_depth_in, int w,int h);

    int getHeight();
    int getWidth();
    png_byte getColorType();
    png_byte getBitDepth();

    unsigned char** getData();

private:
    const char *mFilePath;

    bool mDataLoaded;
    int width, height;
    png_byte color_type;
    png_byte bit_depth;

    png_structp png_ptr;
    png_infop info_ptr;
    int number_of_passes;
    png_bytep * row_pointers;
};

