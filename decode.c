#include<stdio.h>
#include <unistd.h> // For the sleep() function
#include "decode.h"
#include<string.h>
#include "types.h"
#include "common.h"

// Main function to perform decoding
Status do_decoding(Dec_Info *decinfo)
{
    printf("Decoding started!\n");
    // Open the input and output files for decoding
    OperationType ret = open_files_for_decode(decinfo);
    if(ret == e_failure)
        return e_failure;
    sleep(1);
    
    // Skip the BMP header
    ret = skip_header(decinfo->fp_input);
    if(ret == e_failure)
        return e_failure;
    printf("Header skipping completed!\n");
    sleep(1);

    // Decode the magic string to verify the file
    ret = decode_magic_string(decinfo);
    if(ret == e_failure)
        return e_failure;
    sleep(1);

    // Decode the file extension of the hidden data
    ret = decode_extension(decinfo);
    if(ret == e_failure)
        return e_failure;
    sleep(1);

    // Decode the actual hidden data from the image
    ret = decode_data(decinfo);
    if(ret == e_failure)
        return e_failure;
    sleep(1);

    printf("Decoding completed successfully!\n");
    return e_success;
}

// Function to open the required files for decoding
Status open_files_for_decode(Dec_Info *decinfo)
{
    printf("\t\t\t\t\t\t:::::::OPEN FILES STARTED ::::::::\n");
    
    // Open the input image file
    decinfo->fp_input = fopen(decinfo->input_fname, "r");
    if(decinfo->fp_input == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decinfo->input_fname);
        return e_failure;
    }

    // Open the output file to write the decoded data
    decinfo->fp_output = fopen(decinfo->output_fname, "w");
    if(decinfo->fp_output == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decinfo->output_fname);
        return e_failure;
    }
    
    printf("\t\t\t\t\t\t:::::::OPEN FILES COMPLETED ::::::::\n");
    sleep(1); // Delay after file opening
    return e_success;
}

// Function to skip the BMP header
Status skip_header(FILE *fp)
{
    fseek(fp, 54, SEEK_SET); // Skip the first 54 bytes (BMP header)
    int offset = ftell(fp);  // Get the current file position
    printf("offset = %d\n", offset);

    // Verify if the file pointer moved to the correct position
    if(offset == 54)
        return e_success;
    else
        return e_failure;
}

// Function to decode the magic string for file verification
Status decode_magic_string(Dec_Info *decinfo)
{
    printf("\t\t\t\t\t\t:::::::MAGIC STRING DECODE STARTED ::::::::\n");
    
    // Decode the size of the magic string
    decinfo->magic_string_len = decode_size_from_lsb(decinfo->fp_input);
    printf("length of magic string: %d\n", decinfo->magic_string_len);
    
    // Decode the magic string data
    OperationType ret = decode_data_from_image(decinfo->magic_string_len, decinfo->magic_string, decinfo->fp_input, decinfo->fp_output);
    printf("magic string = %s\n", decinfo->magic_string);
    printf("\t\t\t\t\t\t:::::::MAGIC STRING DECODE COMPLETED ::::::::\n");
    sleep(1); // Delay after decoding the magic string

    if(ret == e_failure)
        return e_failure;
    else
        return e_success;
}

// Function to decode the size of data from the least significant bits (LSBs)
int decode_size_from_lsb(FILE *fp)
{
    char buffer[32];
    int len = 0;
    fread(buffer, 32, 1, fp); // Read 32 bits from the image

    // Decode the size from the LSBs
    for(int i = 31; i >= 0; i--)
    {
        if(buffer[31 - i] & 0x01)
            len |= (1 << i); // Set the bit if LSB is 1
        else
            len &= (~(1 << i)); // Clear the bit if LSB is 0
    }
    return len;
}

// Function to decode data from the image
Status decode_data_from_image(int len, char *data, FILE *fp_input, FILE *fp_output)
{
    char buffer[8];
    int i;

    // Decode each byte from the image data
    for(i = 0; i < len; i++)
    {
        fread(buffer, 8, 1, fp_input); // Read 8 bits (1 byte) from the image
        data[i] = decode_byte_from_lsb(buffer, i);
    }
    data[i] = '\0'; // Null-terminate the string
    printf("string = %s\n", data);
    
    // Return success or failure based on the decoding result
    if(i == len)
        return e_success;
    else
        return e_failure;
}

// Function to decode a single byte from LSBs of image data
char decode_byte_from_lsb(char *data, int i)
{
    char ch = 0;

    // Extract bits from the LSBs
    for(int j = 0; j < 8; j++)
    {
        if(data[j] & 0x01)
            ch |= (1 << (7 - j)); // Set the bit if LSB is 1
        else
            ch &= ~(1 << (7 - j)); // Clear the bit if LSB is 0
    }
    return ch;
}

// Function to decode the file extension of the secret file
Status decode_extension(Dec_Info *decinfo)
{
    printf("\t\t\t\t\t\t:::::::EXTENSION DECODE STARTED ::::::::\n");
    
    // Decode the length of the extension
    decinfo->extn_len = decode_size_from_lsb(decinfo->fp_input);
    printf("file extn size = %d\n", decinfo->extn_len);
    
    // Decode the extension data
    OperationType ret = decode_data_from_image(decinfo->extn_len, decinfo->extn, decinfo->fp_input, decinfo->fp_output);
    printf("\t\t\t\t\t\t:::::::EXTENSION DECODE COMPLETED ::::::::\n");
    sleep(1); // Delay after decoding the extension

    if(ret == e_success)
        return e_success;
    else
        return e_failure;
}

// Function to decode the main data from the image
Status decode_data(Dec_Info *decinfo)
{
    printf("\t\t\t\t\t\t:::::::DATA DECODE STARTED ::::::::\n");
    
    // Decode the size of the data
    decinfo->data_len = decode_size_from_lsb(decinfo->fp_input);
    printf("secret data size = %d\n", decinfo->data_len);
    
    // Decode the actual data
    OperationType ret = decode_data_from_image(decinfo->data_len, decinfo->data, decinfo->fp_input, decinfo->fp_output);
    if(ret == e_success)
    {
        fwrite(decinfo->data, decinfo->data_len, 1, decinfo->fp_output); // Write the decoded data to the output file
        printf("\t\t\t\t\t\t:::::::DATA DECODE COMPLETED ::::::::\n");
        sleep(1); // Delay after data decoding
        return e_success;
    }
    else
        return e_failure;
}
