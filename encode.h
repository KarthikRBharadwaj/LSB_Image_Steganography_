#ifndef ENCODE_H
#define ENCODE_H

#include "types.h" // Contains user defined types

/* 
 * Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 100
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _EncodeInfo
{
    /* Source Image info */
    char *src_image_fname;      // Filename of the source image (the image into which data will be hidden)
    FILE *fptr_src_image;       // File pointer to the source image, used to open and read the image
    uint image_capacity;        // The total size of the image in bytes (capacity for hiding data)
    uint bits_per_pixel;        // The number of bits used to represent each pixel in the image (e.g., 24-bit for RGB)
    char image_data[MAX_IMAGE_BUF_SIZE]; // Buffer to store the image data for encoding (e.g., pixel values)

    /* Secret File Info */
    char *secret_fname;         // Filename of the secret data file (the file that will be hidden inside the image)
    FILE *fptr_secret;          // File pointer to the secret file, used to open and read the file to be hidden
    char extn_secret_file[MAX_FILE_SUFFIX]; // Extension of the secret file (e.g., ".txt", ".jpg")
    char secret_data[MAX_SECRET_BUF_SIZE]; // Buffer to store the secret data to be hidden inside the image
    long size_secret_file;      // Size of the secret file (in bytes), used to determine how much data will be hidden

    /* Stego Image Info */
    char *stego_image_fname;    // Filename of the resulting stego image (image that will contain the hidden data)
    FILE *fptr_stego_image;     // File pointer to the stego image, used to open and write the resulting image after encoding
} EncodeInfo;



/* Encoding function prototype */

/* Check operation type */
OperationType check_operation_type(int argc,char *argv[]);

/* Read and validate Encode args from argv */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo);

/* Perform the encoding */
Status do_encoding(EncodeInfo *encInfo);

/* Get File pointers for i/p and o/p files */
Status open_files(EncodeInfo *encInfo);

/* check capacity */
Status check_capacity(EncodeInfo *encInfo);

/* Get image size */
uint get_image_size_for_bmp(FILE *fptr_image);

/* Get file size */
uint get_file_size(FILE *fptr);

/* Copy bmp image header */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image);

/* Store Magic String */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo);

/* Encode secret file extenstion */
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo);

/* Encode secret file size */
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo);

/* Encode secret file data*/
Status encode_secret_file_data(EncodeInfo *encInfo);

/* Encode function, which does the real encoding */
Status encode_data_to_image(const char *data,long int size, FILE *fptr_src_image, FILE *fptr_stego_image);

/* Encode a byte into LSB of image data array */
Status encode_byte_to_lsb(char data, char *image_buffer);

Status encode_size_to_lsb(int data, FILE *fptr_src_image,FILE *fptr_dest_image);
/* Encode a size into LSB of image data array */

/* Copy remaining image bytes from src to stego image after encoding */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest);

#endif
