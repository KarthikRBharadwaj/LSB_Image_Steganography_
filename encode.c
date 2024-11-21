#include <stdio.h>
#include <unistd.h> // For sleep()
#include<string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

Status do_encoding(EncodeInfo *encInfo)
{
    // Start of the encoding process
    printf("INFO : Encoding started!\n");

    // Open necessary files for reading and writing
    printf("INFO : Open files started!\n");
    OperationType res = open_files(encInfo);
    if (res == e_failure)
        return e_failure;
    printf("INFO : Open files Completed!\n");
    sleep(1); // Delay for better visibility

    // Check if the image has enough capacity to hold the secret data
    printf("INFO : Check Capacity Started!\n");
    res = check_capacity(encInfo);
    if (res == e_failure)
        return e_failure;
    printf("INFO : Check Capacity Completed!\n");
    sleep(1); // Delay for better visibility

    // Copy BMP header from the source to the stego image
    printf("INFO : Copy bmp header Started!\n");
    res = copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image);
    if (res == e_failure)
        return e_failure;
    printf("INFO : Copy bmp header Completed!\n");
    sleep(1); // Delay for better visibility

    // Encode the magic string to identify the start of the secret data
    printf("INFO : Encoding magic string Started!\n");
    res = encode_magic_string(MAGIC_STRING, encInfo);
    if (res == e_failure)
        return e_failure;
    printf("INFO : Encoding magic string Completed!\n");
    sleep(1); // Delay for better visibility

    // Check and extract the extension of the secret file
    char *p = strstr(encInfo->secret_fname, ".txt");
    if (p == NULL)
        return e_failure; // Fail if not a .txt file
    strcpy(encInfo->extn_secret_file, p);

    // Encode the file extension into the image
    printf("INFO : Encoding secret file extn Started!\n");
    res = encode_secret_file_extn(encInfo->extn_secret_file, encInfo);
    if (res == e_failure)
        return e_failure;
    printf("INFO : Encoding secret file extn Completed!\n");
    sleep(1); // Delay for better visibility

    // Move the file pointer to the end to get the size of the secret file
    rewind(encInfo->fptr_secret);
    fseek(encInfo->fptr_secret, 0, SEEK_END);
    int file_size = ftell(encInfo->fptr_secret);

    // Encode the size of the secret file
    printf("INFO : Encoding secret file size Started!\n");
    res = encode_secret_file_size(file_size, encInfo);
    if (res == e_failure)
        return e_failure;
    printf("INFO : Encoding secret file size Completed!\n");
    sleep(1); // Delay for better visibility

    // Encode the actual content of the secret file
    printf("INFO : Encoding secret file data Started!\n");
    res = encode_secret_file_data(encInfo);
    if (res == e_failure)
        return e_failure;
    printf("INFO : Encoding secret file data Completed!\n");
    sleep(1); // Delay for better visibility

    // Copy the remaining image data to the stego image
    printf("INFO : Copy remaining data Started!\n");
    res = copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image);
    if (res == e_failure)
        return e_failure;
    printf("INFO : Copy remaining data Completed!\n");
    sleep(1); // Delay for better visibility

    // Indicate success
    printf("INFO : Encoding Successful!\n");
    return e_success;
}


/* Get the image size from a BMP file */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to the position where width is stored (18th byte)
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (4 bytes)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (4 bytes)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Calculate and return the total image size (width * height * 3 bytes/pixel)
    return width * height * 3;
}

/* Open the necessary files for encoding */
Status open_files(EncodeInfo *encInfo)
{
    // Open the source image file in read mode
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);
        return e_failure;
    }

    // Open the secret file in read mode
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);
        return e_failure;
    }

    // Open the stego image file in write mode
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);
        return e_failure;
    }

    // Return success if all files are opened correctly
    return e_success;
}

/* Check if the image has enough capacity to hold the secret data */
Status check_capacity(EncodeInfo *encInfo)
{
    //printf("Check Capacity Started!\n");

    // Get the total capacity of the image
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    // Get the size of the secret file
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
   // printf("secret file size -> %ld\n", encInfo->size_secret_file);

    // Calculate the required space for the encoded data
    int len = strlen(MAGIC_STRING);
    int len_ext = 4; // For ".txt"
    int temp = 54 + (8 * (len + sizeof(int) + len_ext + sizeof(int) + encInfo->size_secret_file));
   // printf("Temp = %d\n", temp);

    // Check if the image capacity is sufficient
    if (encInfo->image_capacity > temp)
        return e_success;
    else
        return e_failure;
}

/* Get the size of a file */
uint get_file_size(FILE *fptr)
{
    fseek(fptr, 0, SEEK_END);
    uint size = ftell(fptr); // Get the file size
    return size;
}

/* Copy the BMP header from the source image to the stego image */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
   // printf("Header copying started!\n");
    char s[54]; // Buffer to store the header

    // Move to the beginning of the source image and read 54 bytes (header)
    fseek(fptr_src_image, 0, SEEK_SET);
    fread(s, 54, 1, fptr_src_image);

    // Check for read failure
    if (s == NULL)
        return e_failure;

    // Write the header to the stego image
    fwrite(s, 54, 1, fptr_dest_image);

    // Confirm if the header is correctly written
    if (ftell(fptr_dest_image) == 54)
        return e_success;
    else
        return e_failure;
}

/* Encode the magic string into the image */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
   // printf("Encode magic string started!\n");
    int len = strlen(magic_string);
   // printf("len -> %d\n", len);

    // Encode the length of the magic string
    OperationType ret = encode_size_to_lsb(len, encInfo->fptr_src_image, encInfo->fptr_stego_image);
   // printf("%ld %ld\n", ftell(encInfo->fptr_src_image), ftell(encInfo->fptr_stego_image));
    if (ret == e_failure)
        return e_failure;

    // Encode the actual magic string data
    ret = encode_data_to_image(magic_string, len, encInfo->fptr_src_image, encInfo->fptr_stego_image);
    if (ret == e_failure)
        return e_failure;

    return e_success;
}

/* Encode data into the image */
Status encode_data_to_image(const char *data, long int len, FILE *fptr_src_image, FILE *fptr_stego_image)
{
   // printf("Encoding data to image started!\n");
    char image_buffer[8];
    int i;

  //  printf("Encoding byte to lsb started!\n");
//printf("secret data in data to image -> %s\n", data);
   // printf("len = %ld\n", len);

    // Loop through each byte of the data and encode it
    for (i = 0; i < len; i++)
    {
       // printf("i = %d\n", i);
        fread(image_buffer, 8, 1, fptr_src_image);
        OperationType ret = encode_byte_to_lsb(data[i], image_buffer);
        if (ret == e_success)
            fwrite(image_buffer, 8, 1, fptr_stego_image);
        else
            return e_failure;
    }

   // printf("Encoding byte to lsb completed!\n");
   // printf("Encoding data to image completed!\n");

    // Confirm that all data has been encoded
    if (i == len)
        return e_success;
    else
        return e_failure;
}

/* Encode an integer (size) to the LSB of 32 bytes */
Status encode_size_to_lsb(int data, FILE *fptr_src_image, FILE *fptr_stego_image)
{
   // printf("Encoding size to lsb started!\n");
    char image_buffer[32];

    // Read 32 bytes from the source image
    fread(image_buffer, 32, 1, fptr_src_image);

    // Encode each bit of the integer into the LSBs of the buffer
    for (int i = 31; i >= 0; i--)
    {
        if (data & (1 << i))
            image_buffer[31 - i] |= 0x01; // Set LSB to 1
        else
            image_buffer[31 - i] &= 0xFE; // Set LSB to 0
    }

    // Write the modified buffer to the stego image
    fwrite(image_buffer, 32, 1, fptr_stego_image);
   // printf("Encoding size to lsb completed!\n");

    return e_success;
}

/* Encode a single byte of data to the LSBs of an 8-byte buffer */
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    // Encode each bit of the byte into the LSBs of the buffer
    for (int i = 7; i >= 0; i--)
    {
        if (data & (1 << i))
            image_buffer[7 - i] |= 0x01; // Set LSB to 1
        else
            image_buffer[7 - i] &= 0xFE; // Set LSB to 0
    }

    return e_success;
}

/* Encode the file extension of the secret file */
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
   // printf("Encoding file extension started!\n");
    int len = strlen(file_extn);

    // Encode the length of the file extension
    OperationType res = encode_size_to_lsb(len, encInfo->fptr_src_image, encInfo->fptr_stego_image);
    if (res == e_failure)
        return e_failure;

    // Encode the actual file extension
    res = encode_data_to_image(file_extn, len, encInfo->fptr_src_image, encInfo->fptr_stego_image);
   // printf("Encoding file extension completed!\n");

    if (res == e_failure)
        return e_failure;
    else
        return e_success;
}

/* Encode the size of the secret file */
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
   // printf("Encoding secret file size\n");
    int res = encode_size_to_lsb(file_size, encInfo->fptr_src_image, encInfo->fptr_stego_image);
    if (res)
        return e_failure;
    else
        return e_success;
}

/* Encode the actual data of the secret file */
Status encode_secret_file_data(EncodeInfo *encInfo)
{
   // printf("Encoding secret data started\n");
    fseek(encInfo->fptr_secret, 0, SEEK_SET); // Move to the start of the secret file

    // Read the secret data into memory
   // printf("len of the file -> %ld\n", encInfo->size_secret_file);
    int l = encInfo->size_secret_file;
    fread(encInfo->secret_data, encInfo->size_secret_file, 1, encInfo->fptr_secret);
   // printf("data -> %s\n", encInfo->secret_data);
    encInfo->size_secret_file = l;
   // printf("len of the file -> %ld\n", encInfo->size_secret_file);

    // Encode the secret data to the image
    OperationType res = encode_data_to_image(encInfo->secret_data, encInfo->size_secret_file, encInfo->fptr_src_image, encInfo->fptr_stego_image);
    if (res == e_failure)
        return e_failure;
    else
        return e_success;
}

/* Copy the remaining image data after encoding */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch;

    // Copy byte by byte until the end of the source image
    while (fread(&ch, 1, 1, fptr_src))
        fwrite(&ch, 1, 1, fptr_dest);

   // printf("Encoding process completed!\n");
    return e_success;
}

