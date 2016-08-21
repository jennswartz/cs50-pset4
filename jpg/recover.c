/**
 * recover.c
 *
 * Computer Science 50
 * Problem Set 4
 *
 * Recovers JPEGs from a forensic image.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>


#define BUFFER_SIZE 512


int main(int argc, char* argv[])
{

    unsigned char buffer[BUFFER_SIZE];
    int counter = 000;
    FILE* outptr = NULL;
    char file_name[sizeof("000.jpg")];
        

    // name card.raw the inptr variable and open it
    FILE* inptr = fopen("card.raw", "r");
    if (inptr == NULL)
    {
        printf("Could not open file.");
        return 1;
    }

    else
    {
        
        // read a block into the buffer
        fread(buffer, sizeof(BUFFER_SIZE), 1, inptr);
        

        // create while loop for creating the files
        while (fread(buffer, sizeof(BUFFER_SIZE), 1, inptr) == 1)
        {

            // note: I can't get it to work if I use something to try to check individual bits in buffer[3]. Not sure why, but this works.
            if(buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && (buffer[3] == 0xe0 || buffer[3] == 0xe1))
            {
                // if a jpg was found from prior loop, close the outptr file
                if (outptr)
                {
                    fclose(outptr);
                }

                // create the file name, and add to the counter for the next file, 
                sprintf(file_name, "%03d.jpg", counter);
                counter++;

                // open new outptr file with most recent file name
                outptr = fopen(file_name, "w");
                
                // Check for error in outptr file
                if(!outptr)
                {
                    printf("Error opening outptr file.");
                    return 2;
                }

                // write buffer to the opened file
                fwrite(buffer, sizeof(BUFFER_SIZE), 1, outptr);
        
            }
        
            // print rest of blocks to the outptr file    
            else if (outptr)
            {
                fwrite(buffer, sizeof(BUFFER_SIZE), 1, outptr);
            }
        }
    }
        // close infile and outfile
        fclose(inptr);
        if (outptr)
        {
            fclose(outptr);
        }
    
        // that's all folks
        return 0;
}
