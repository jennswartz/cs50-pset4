/**
 * resize.c
 *
 * Computer Science 50
 * Problem Set 4
 *
 * Resizes a BMP.
 */
       
#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char* argv[])
{
    //  Set resize variable to value of argument given in the command line
    int resize = atoi(argv[1]);

    // ensure proper usage
    if (argc != 4 || resize < 1 || resize > 100)
    {
        printf("Usage: ./resize n infile outfile. n must be from 1 to 100.\n");
        return 1;
    }

    // remember filenames
    char* infile = argv[2];
    char* outfile = argv[3];

    // open input file 
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE* outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }

    // set variables for resize width and height
    int original_width = bi.biWidth;
    int original_height = bi.biHeight;
    bi.biWidth = bi.biWidth * resize;
    bi.biHeight = bi.biHeight * resize;
    
    // determine padding for scanlines
    int infile_padding =  (4 - (original_width * sizeof(RGBTRIPLE)) % 4) % 4;
    int resize_padding =  (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    
    // determine length of line in bytes to use for offset value in fseek()
    int offset = (original_width * sizeof(RGBTRIPLE)) + infile_padding;
    
    // calculate resized bitmaps biSizeImage and bfSize for updating headers
    bi.biSizeImage = ((bi.biWidth * sizeof(RGBTRIPLE)) + resize_padding) * abs(bi.biHeight);
    bf.bfSize = bi.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);
    
    // iterate over infile's scanlines
    for (int i = 0; i < abs(original_height); i++)
    {
        // iterate resize times over all the pixels in the scanline    
        for (int j = 0; j < resize; j++)
        {
            // iterate over pixels in scanline
            for (int k = 0; k < original_width; k++)
            {
                // temporary storage
                RGBTRIPLE triple;

                // read RGB triple from infile
                fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

                // iterate again to resize horizontally
                for (int l = 0; l < resize; l++)
                {
                    // write RGB triple to outfile
                    fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
                }
            }
        
            // skip over infile padding, if any
            fseek(inptr, infile_padding, SEEK_CUR);

            // then add resized padding
            for (int m = 0; m < resize_padding; m++)
            {
                fputc(0x00, outptr);
            }
            
            // then move cursor back value of offset until the loop completes
            if (j < resize - 1)
            {
                fseek(inptr, -offset, SEEK_CUR);
            }
        }
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // that's all folks
    return 0;
}
