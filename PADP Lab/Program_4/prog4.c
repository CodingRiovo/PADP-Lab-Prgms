#include <stdio.h>
#include <stdlib.h>
#include <gd.h>
#include <omp.h>

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: %s <in.png> <out.png>\n", argv[0]);
        exit(1);
    }

    FILE *ifp = fopen(argv[1], "rb");
    FILE *ofp = fopen(argv[2], "wb");
    
    gdImagePtr img = gdImageCreateFromPng(ifp);
    
    int w = gdImageSX(img);
    int h = gdImageSY(img);
    
    printf("Image size: %d x %d pixels\n", w, h);
    
    gdImagePtr outImg = gdImageCreateTrueColor(w, h);
    omp_set_num_threads(4);
    double t = omp_get_wtime();

#pragma omp parallel for schedule(dynamic, 20) 
    for (int x = 0; x < w; x++)
    {
        for (int y = 0; y < h; y++)
        {
            int tid = omp_get_thread_num();
            int color = gdImageGetPixel(img, x, y);
            int avgColor = (gdImageRed(img, color) + gdImageGreen(img, color) + gdImageBlue(img, color)) / 3;

            
            if(tid == 0){
                color = gdImageColorAllocate(outImg, avgColor, 0, 0);
                gdImageSetPixel(outImg, x, y, color);
            }
            else if(tid == 1){
                color = gdImageColorAllocate(outImg, 0, avgColor, 0);
                gdImageSetPixel(outImg, x, y, color);
            }
            else if(tid == 2){
                color = gdImageColorAllocate(outImg, 0, 0, avgColor);
                gdImageSetPixel(outImg, x, y, color);
            }
            else{
                color = gdImageColorAllocate(outImg, avgColor, avgColor, avgColor);
                gdImageSetPixel(outImg, x, y, color);
            }
        }
    }

    gdImagePng(outImg, ofp);
    gdImageDestroy(img);
    gdImageDestroy(outImg);

    fclose(ifp);
    fclose(ofp);

    t = omp_get_wtime() - t;
    printf("Time: %f seconds\n", t);

    return 0;
}