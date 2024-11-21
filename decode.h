#include<stdio.h>
#include<stdlib.h>

#ifndef DECODE_H
#define DECODE_H
#include<stdlib.h>

#include "types.h"

#define MAG_SIZE 100
#define EXTEN_LEN 4
#define DATA_LEN 500

typedef struct _DecodeInfo
{
    // The name of the encoded image file (input file)
    char *input_fname;  // A string to hold the filename of the encoded image
    FILE *fp_input;     // File pointer to the encoded image for reading

    // Information related to the decoding process
    int magic_string_len;  // Length of the magic string used for identifying the steganography format
    char magic_string[MAG_SIZE];  // The magic string used to identify the stego image (e.g., "STEG")
    
    int extn_len;        // Length of the file extension of the secret data (e.g., ".txt")
    char extn[EXTEN_LEN]; // The file extension of the secret data that was hidden in the image
    
    int data_len;        // Length of the secret data that was embedded in the image
    char data[DATA_LEN]; // Buffer to store the decoded secret data (the actual hidden message)
    
    // Information about the decoded output
    char *output_fname;  // The name of the output file where the decoded secret data will be saved
    FILE *fp_output;     // File pointer for the output file (where the secret data is written)
} Dec_Info;


//to validate command line arguments
Status read_and_validate(char *argv[],Dec_Info *decinfo);

//decoding function
Status do_decoding(Dec_Info *decinfo);

//open the required file pointers
Status open_files_for_decode(Dec_Info *decinfo);

//skip_header and craete pointer for file
Status skip_header(FILE *fp_input);

//to decode magic string and magic string length
Status decode_magic_string(Dec_Info *decinfo);

//to decode extension length and extension data
Status decode_extension(Dec_Info *decinfo);

//to decode data from encoded image to output file
Status decode_data(Dec_Info *decinfo);

//to decode size(int) from encoded image
int decode_size_from_lsb(FILE *fp);

//to decode data char by char from encoded image
Status decode_data_from_image(int len,char *data,FILE *fp_input,FILE *fp_output);

//to decode data(string) from encoded image
char decode_byte_from_lsb(char *data,int i);

#endif