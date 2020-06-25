// ImageScale.cpp : main project file.

#include "stdafx.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "image_library.h"

#define PNG_DEBUG 3
#include <png.h>

#include "CPNGHelper.h"

using namespace System;

//#define __DEBUG__ 1

static void abort_(const char * s, ...)
{
    va_list args;
    va_start(args, s);
    vfprintf(stderr, s, args);
    fprintf(stderr, "\n");
    va_end(args);
    abort();
}

unsigned int getMaxHeapSize(void)
{
    unsigned int size=1;
    unsigned int *p = &size;

    while (p) {
        p = (unsigned int *)malloc(size);
        if (p) {
            free(p);
            size *= 2;
        }
    }
    return size;
}

int main(int argc, char **argv)
{
    if (argc != 5)
        abort_("Usage: program_name <file_in> <scaleX> <scaleY> <file_out>");

    using namespace image_library;

    float scaleX = atof(argv[2]);
    float scaleY = atof(argv[3]);

    printf("\n\n\n");

    if ( scaleX <= 0.0f )
    {
        printf("Invalid scaleX %0.4f, Must be greater than 0.0f\n", scaleX);
        exit(0);
    }
    if ( scaleY <= 0.0f )
    {
        printf("Invalid scaleY %0.4f, Must be greater than 0.0f\n", scaleY);
        exit(0);
    }

    if ( scaleX == 1.0f && scaleY == 1.0f )
    {
        printf("Nothing to scale(1.0,1.0). Scale factors are same as original\n");
        exit(0);
    }

    CPNGHelper png_source;
    png_source.LoadData(argv[1]);

    const int source_w = png_source.getWidth();
    const int source_h = png_source.getHeight();

    if ( source_w * scaleX < 2.0f )
    {
        printf("Usage: Source Image Width[%d] is too small. Can't scale upto %0.4f", source_w, scaleX);
        exit(0);
    }

    if ( source_h * scaleY < 2.0f )
    {
        printf("Usage: Source Image Height[%d] is too small. Can't scale upto %0.4f", source_h, scaleY);
        exit(0);
    }

    // TODO: memory check
    //unsigned int size = getMaxHeapSize();
    //printf("Map heap size %u\n", size);

    printf("\nSource Image : %s\n", argv[1]);
    printf("Image Height: %d\n", source_h);
    printf("Image Width: %d\n", source_w);
    printf("ScaleX : %0.4f\n", scaleX);
    printf("ScaleY : %0.4f\n", scaleX);
    printf("Output Image : %s\n", argv[4]);
    printf("Image Height: %d\n", (int)(source_w * scaleX));
    printf("Image Width: %d\n", (int)(source_h * scaleY));
    printf("\nImage is processing. Please wait..... \n");

    CPNGHelper png_dest(png_source.getWidth()*scaleX, png_source.getHeight()*scaleY, png_source.getColorType(), png_source.getBitDepth() );

    if ( true == image_library::scale_image((const unsigned char**)png_source.getData(), png_source.getWidth(), png_source.getHeight(), png_dest.getData(), scaleX, scaleY, true))
    {
        png_dest.WriteData(argv[4]);
        printf("Image generated at %s\n", argv[4]);
    }
    else
        printf("Cannot scale image upto [%0.2fx%0.2f]. Please Try smaller scale.\n", scaleX, scaleY);

    return 0;
}


