#pragma once
#include "math_helper.h"
#include "CTaskProcessor.h"
#include "CBilinearTask.h"
#include "CLinearTask.h"

//#define __DEBUG__ 1
#ifdef __DEBUG__
#include "CPNGHelper.h"
#define D_PRINT(fmt, ...) \
        do { \
            if (__DEBUG__) \
                printf("%s(%d):" fmt, __FUNCTION__, __LINE__, __VA_ARGS__); \
        } \
        while (0)
#else
#define D_PRINT(var)
#endif

namespace image_library
{
    void linearInterpolate(const unsigned char**bi_high_rezied, const unsigned char**bi_low_rezied, unsigned char**dest_data, int x, int y, float h3);
    void bilinearInterpolation(const unsigned char**source_data, int w_source, int h_source, unsigned char**dest_data, Point2D& destPoint, float scaleX, float scaleY);
    bool trilinearFiltering(const unsigned char** bi_low, int w_low, int h_low, const unsigned char** bi_high, int w_high, int h_high, unsigned char**dest_data, int w, int h);

    unsigned char**memoryAllocator(int w, int h);
    unsigned char**memoryDeallocator(int w, int h);

    unsigned char**memoryAllocator(int w, int h)
    {
        int y;
        unsigned char** mem_chunk = NULL;
        // Try to allocate memory
        mem_chunk = (unsigned char**) malloc(sizeof(unsigned char*) * h );
        if ( mem_chunk == NULL )
            return NULL;
        for (y=0; y<h ; y++)
        {
            mem_chunk[y] = (unsigned char*) malloc(w);
            if ( mem_chunk[y] == NULL )
            {
                for (int x=0; x < y; x++)
                    free(mem_chunk[x]);
                free(mem_chunk);
                return NULL;
            }
        }

        return mem_chunk;
    }

    void memoryDeallocator(unsigned char**mem_chunk, int h)
    {
        if ( mem_chunk == NULL )
            return;

        int y;
        for (y=0; y<h; y++)
            free(mem_chunk[y]);
        free(mem_chunk);
        mem_chunk = NULL;
    }

    void linearInterpolate(const unsigned char**bi_high_rezied, const unsigned char**bi_low_rezied, unsigned char**dest_data, int x, int y, float h3)
    {
        Point4D P_high (bi_high_rezied, x,y);
        Point4D P_low  (bi_low_rezied,  x,y);

        unsigned char* dest_row = dest_data[y];
        unsigned char* dest_col = &(dest_row[x*4]);

        Point4D dest_value = P_high + (P_low - P_high) * h3;
        dest_col[0] = dest_value.x;
        dest_col[1] = dest_value.y;
        dest_col[2] = dest_value.z;
        dest_col[3] = dest_value.w;
    }

    bool trilinearFiltering(
        const unsigned char** bi_low,    int w_low, int h_low,
        const unsigned char** bi_high,   int w_high, int h_high,
        unsigned char**dest_data, int w, int h)
    {
        int y;
        const int w_dest = w;
        const int h_dest = h;

        CTaskProcessor tp;  // Task Processor, submit tasks and it will execute on startProcess()

        // Downsize/ScaleDown large image
        unsigned char** bi_high_rezied = memoryAllocator(4 * w, h);
        if ( bi_high_rezied == NULL )
        {
            D_PRINT("Not enough memory [%dx%d]\n", 4*w,h);
            return false;
        }

        const int maxThread = tp.maxTaskCount();
        int max_height_per_thread = 0;
        if ( h_dest % maxThread == 0 )
            max_height_per_thread = max(h_dest/maxThread, 1);
        else max_height_per_thread = (max(h_dest/maxThread, 1)) + 1;

        ITask **t1;         // List of Tasks
        int taskindex = 0;  // Task IDs
        t1 = new ITask*[maxThread];
        taskindex = 0;
        tp.clearAllTask();
        for (y=0; y<h_dest; y+=max_height_per_thread) {
            t1[taskindex] = new CBilinearTask(taskindex, image_library::bilinearInterpolation,
                bi_high, w_high, h_high,
                y, min((y+max_height_per_thread),h_dest), w_dest,
                bi_high_rezied,
                w/(float)w_high, h/(float)h_high);

            tp.submitTask(t1[taskindex]);
            taskindex++;
        }

        tp.startProcess();
        tp.clearAllTask();

        // Cleanup task
        for ( int i = 0; i < taskindex; i++)
        {
            delete t1[i];
            t1[i] = NULL;
        }

        // Upsize/ScaleUp small image
        unsigned char** bi_low_rezied = memoryAllocator(4*w, h);
        if ( bi_low_rezied == NULL )
        {
            D_PRINT("Not enough memory [%dx%d]\n", 4*w,h);
            return false;
        }

        max_height_per_thread = 0;
        if ( h_dest % maxThread == 0 )
            max_height_per_thread = max(h_dest/maxThread, 1);
        else max_height_per_thread = (max(h_dest/maxThread, 1)) + 1;

        t1 = new ITask*[maxThread];
        taskindex = 0;
        tp.clearAllTask();
        for (y=0; y<h_dest; y+=max_height_per_thread) {
            t1[taskindex] = new CBilinearTask(taskindex, image_library::bilinearInterpolation,
                bi_low, w_low, h_low,
                y, min((y+max_height_per_thread),h_dest), w_dest,
                bi_low_rezied,
                w/(float)w_low, h/(float)h_low);

            tp.submitTask(t1[taskindex]);
            taskindex++;
        }

        tp.startProcess();
        tp.clearAllTask();

        // Cleanup task
        for ( int i = 0; i < taskindex; i++)
        {
            delete t1[i];
            t1[i] = NULL;
        }


#ifdef __DEBUG__
        // Dump intermediate data
        {
            CPNGHelper png_dest;
            png_dest.WriteData("E:\\Pratice_Prgs\\NVidia\\Source\\data\\out\\out_DS.png", bi_high_rezied, 6, 8, w,h);
            png_dest.WriteData("E:\\Pratice_Prgs\\NVidia\\Source\\data\\out\\out_US.png", bi_low_rezied, 6, 8, w,h);

        }
#endif
        // Linear interpolate two textures
        max_height_per_thread = 0;
        if ( h % maxThread == 0 )
            max_height_per_thread = max(h/maxThread, 1);
        else max_height_per_thread = (max(h/maxThread, 1)) + 1;

        const float h3 = (w_high - w)/(float)(w_high-w_low);    // Pixel distance from bigger mipmap

        t1 = new ITask*[maxThread];
        taskindex = 0;
        tp.clearAllTask();
        for (y=0; y<h; y+=max_height_per_thread) {

            t1[taskindex] = new CLinearTask(taskindex, image_library::linearInterpolate,
                (const unsigned char**)bi_high_rezied, (const unsigned char**)bi_low_rezied,
                dest_data,
                y, min((y+max_height_per_thread),h), w,
                h3);

            tp.submitTask(t1[taskindex]);
            taskindex++;
        }

        tp.startProcess();
        tp.clearAllTask();

        // Cleanup task
        for ( int i = 0; i < taskindex; i++)
        {
            delete t1[i];
            t1[i] = NULL;
        }
        delete t1;

        // Cleanup memery
        memoryDeallocator(bi_high_rezied, h);
        memoryDeallocator(bi_low_rezied, h);

        return true;
    }

    void bilinearInterpolation(
        const unsigned char**  source_data, int w_source, int h_source,
        unsigned char**  dest_data,
        Point2D& destPoint, float scaleX, float scaleY)
    {
        Point4D Q11, Q12, Q21, Q22;
        Point4D R1, R2;
        Point4D P;

        float x1, y1, x2, y2;
        float x_dest2source,y_dest2source;
        x_dest2source =  destPoint.x/scaleX;
        y_dest2source =  destPoint.y/scaleY;

        x1 = (int)x_dest2source;
        y1 = (int)y_dest2source;
        x2 = min(x1 + 1, w_source-1);
        y2 = min(y1 + 1, h_source-1);

        // Get the color value                          // Q11 _____R1___ Q21
        Q11 = Point4D(source_data, x1, y1);             //    |     |    |
        Q12 = Point4D(source_data, x1, y2);             //    |_____|P___|
        Q21 = Point4D(source_data, x2, y1);             //    |     |    |
        Q22 = Point4D(source_data, x2, y2);             //    |_____R2___|
                                                        // Q12            Q22
        // There are several ways equivalent ways to calculate the value of P.
        // An easy way to calculate the value of P would be to first calculate the value of the two blue dots, R2, and R1.
        // R2 is effectively a weighted average of Q12 and Q22, while R1 is a weighted average of Q11 and Q21.

        R1 = ( Q11*(x2 - x_dest2source) + Q21*(x_dest2source - x1) )/(x2-x1);

        R2 = ( Q12*(x2 - x_dest2source) + Q22*(x_dest2source - x1) )/(x2-x1);

        //After the two R values are calculated, the value of P can finally be calculated by a weighted average of R1 and R2.
        P = ( R1*(y2 - y_dest2source) + R2*(y_dest2source - y1))/(y2 - y1);

        // Copy Color Data to dest buffer
        unsigned char* dest_row = dest_data[destPoint.y];
        unsigned char* dest_col = &(dest_row[destPoint.x*4]);
        dest_col[0] = P.x;
        dest_col[1] = P.y;
        dest_col[2] = P.z;
        dest_col[3] = P.w;
    }

    bool scale_image(const unsigned char** source_data, int w_source, int h_source, unsigned char** dest_data, float scaleX, float scaleY, bool useTrilinear = false)
    {
        bool bStatus = false;
        CTaskProcessor tp;  // Task Processor, submit tasks and it will execute on startProcess()

        int y;
        const int w_dest = w_source * scaleX;
        const int h_dest = h_source * scaleY;
        if ( !useTrilinear )
        {
            const int maxThread = tp.maxTaskCount();
            int max_height_per_thread = 0;
            if ( h_dest % maxThread == 0 )
                max_height_per_thread = max(h_dest/maxThread, 1);
            else max_height_per_thread = (max(h_dest/maxThread, 1)) + 1;

            ITask **t1;         // List of Tasks
            int taskindex = 0;  // Task IDs
            t1 = new ITask*[maxThread];
            taskindex = 0;
            tp.clearAllTask();
            // Split task
            for (y=0; y<h_dest; y+=max_height_per_thread) {
                t1[taskindex] = new CBilinearTask(taskindex, image_library::bilinearInterpolation,
                    source_data, w_source, h_source,
                    y, min((y+max_height_per_thread),h_dest), w_dest,
                    dest_data,
                    scaleX, scaleY );

                tp.submitTask(t1[taskindex]);
                taskindex++;
            }

            tp.startProcess();
            tp.clearAllTask();

            for ( int i = 0; i < taskindex; i++)
            {
                delete t1[i];
                t1[i] = NULL;
            }

            delete t1;

            bStatus = true;
        }
        else
        {
            const float low_ScaleX = scaleX * 0.9f;
            const float low_ScaleY = scaleY * 0.9f;

            const float high_ScaleX = scaleX * 1.1f;
            const float high_ScaleY = scaleY * 1.1f;

            // Step 1:
            // Generate Low Mipmap using bilinear
            const int w_low = w_source*low_ScaleX;
            const int h_low = h_source*low_ScaleY;
            unsigned char** low_mipmap_data = memoryAllocator(4 * w_low, h_low);
            if ( low_mipmap_data == NULL )
            {
                D_PRINT("Not enough memory [%dx%d]\n", 4 * w_low, h_low);
                return false;
            }

            const int maxThread = tp.maxTaskCount();
            int max_height_per_thread_low = 0;
            if ( h_low % maxThread == 0 )
                max_height_per_thread_low = max(h_low/maxThread, 1);
            else max_height_per_thread_low = (max(h_low/maxThread, 1)) + 1;

            ITask **t1;         // List of Tasks
            int taskindex;      // Task IDs
            t1 = new ITask*[maxThread];
            taskindex = 0;
            tp.clearAllTask();
            // Split task
            for (y=0; y<h_low; y+=max_height_per_thread_low) {
                t1[taskindex] = new CBilinearTask(taskindex, image_library::bilinearInterpolation,
                    source_data, w_source, h_source,
                    y, min((y+max_height_per_thread_low),h_low), w_low,
                    low_mipmap_data,
                    low_ScaleX, low_ScaleY );

                tp.submitTask(t1[taskindex]);
                taskindex++;
            }

            // Task cleanup
            tp.startProcess();
            tp.clearAllTask();

            for ( int i = 0; i < taskindex; i++)
            {
                delete t1[i];
                t1[i] = NULL;
            }

            // Step 2:
            // Generate High Mipmap using bilinear
            const int w_high = w_source*high_ScaleX;
            const int h_high = h_source*high_ScaleY;
            unsigned char** high_mipmap_data = memoryAllocator(4 * w_high, h_high);
            if ( high_mipmap_data == NULL )
            {
                D_PRINT("Not enough memory [%dx%d]\n", 4 * w_high, h_high);
                return false;
            }
            int max_height_per_thread_high = 0;
            if ( h_high % maxThread == 0 )
                max_height_per_thread_high = max(h_high/maxThread, 1);
            else max_height_per_thread_high = (max(h_high/maxThread, 1)) + 1;

            t1 = new ITask*[maxThread];
            taskindex = 0;
            tp.clearAllTask();
            // Split task
            for (y=0; y<h_high; y+=max_height_per_thread_high) {
                t1[taskindex] = new CBilinearTask(taskindex, image_library::bilinearInterpolation,
                    source_data, w_source, h_source,
                    y, min((y+max_height_per_thread_high),h_high), w_high,
                    high_mipmap_data,
                    high_ScaleX, high_ScaleY );

                tp.submitTask(t1[taskindex]);
                taskindex++;
            }

            // Task cleanup
            tp.startProcess();
            tp.clearAllTask();

            for ( int i = 0; i < taskindex; i++)
            {
                delete t1[i];
                t1[i] = NULL;
            }
            delete t1;

            // Step 3:
            // Generate actual using trilinear
            bStatus = image_library::trilinearFiltering(
                (const unsigned char**)low_mipmap_data,  w_low,  h_low,
                (const unsigned char**)high_mipmap_data, w_high, h_high,
                dest_data, w_dest, h_dest);

            // Cleanup memery
            // Deallocate all temp mem
            memoryDeallocator(high_mipmap_data, h_high);
            memoryDeallocator(low_mipmap_data, h_low);
        }

        return bStatus;
    }
}