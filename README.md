"# ImageScaler" 
/******************************************************
    README : ImageScale
    Author : SANKHA RAY
    Date   : 01-May-2016
********************************************************/

Dependencies (Libraries):
========================
    1. lpng1621
    2. zlib-1.2.8
    3. Visual Studio 2010

Description:
=============
    The application takes a png image and scales it to destination png image.
    Application uses Bilinear interpolation and Trilinear filtering to perform image scaling
    Usage: app_name.exe <input.png> <scaleX> <scaleY> <output.png>
        e.g> ImageScale.exe cat.png 2.50 2.50 cat_out.png   // Upscale
             ImageScale.exe cat.png 0.75 0.75 cat_out.png   // Downscale

Design Docs:
============
    Design Document is available at Docs\Design.docx

Setup and Build:
=================
    1. Untar the source packet (ImageScale.7z) using 7-zip or relevant software.
    2. Goto Source/lpng1621/projects/vstudio Directory.
    3. Open Solution File vstudio.sln using Visual Studio 2010
    4. Build the Solution (It will also build libpng and zlib as per build dependencies)
    5. After successful build, executable should be generated at Source/ImageScale/Executable/ImageScale.exe

Prebuilt_Prgram_Executable:
===========================
    Prebuilt executable program is available under Prebuilt_Prgram_Executable directory for quick reference.
    Goto the directory in cmd prompt and check usage to execute the program exe

Sample Data:
============
    Sample input image data is available at Source\data

Execution:
==========
    Execute the Binary using Ctrl + F5 or F5 in Visual Studio 2010 after successful build.
    Notes: Copy libpng16.dll at same directory (Executable)

    Alternately, Source/ImageScale/Executable/ImageScale.exe can be executed outside Visual Studio
    Notes:
    1. Copy data(images) to Source/ImageScale/Executable/
    2. Copy libpng16.dll at same directory(Debug)
    4. Execute ImageScale.exe
        Usage: See Desciption
