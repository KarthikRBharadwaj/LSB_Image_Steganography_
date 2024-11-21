/*


Name: Karthik R Bharadwaj

Date of Submission: 11-11-2024

Project Name:LSB Image Steganography

Description:

Steganography is the art of hiding the fact  that communication  is  taking  place, by  hiding  information  in  other  information. 
Many different carrier file formats can be used, but digital images are the most popular because of their frequency on the internet.
For hiding secret  information in images, there exists a large variety of steganography techniques some are more complex than others 
and all of them  have respective  strong  and weak points. Different applications may require  absolute  invisibility  of the secret 
information,while others require a large secret message to be hidden. This project is called LSB Steganography because we are hiding
the secret data in the Least Significant Bit (LSB) of the innocent source image.
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
#include <string.h>

// Main function to handle encoding and decoding based on command line arguments
int main(int argc, char *argv[]) {
    EncodeInfo encInfo; // Structure to store encoding information
    Dec_Info decinfo; // Structure to store decoding information

    // Check operation type (either encoding or decoding)
    OperationType res = check_operation_type(argc,argv);
    // If operation is encoding
    if (res == e_encode) {
        // Check if there are enough arguments for encoding
        if (argc >= 4) {
            res = read_and_validate_encode_args(argv, &encInfo);
            if (res == e_success) {
                // Proceed with encoding process
                res = do_encoding(&encInfo);
                if (res == e_success)
                    printf(":::::::ENCODING SUCCESSFUL::::::!\n");
                else
                    printf(":::::::ENCODING FAILED::::::!\n");
            } else
                printf("\t\t\t\t\t\t:::::::VALIDATION FAILED :::::::\n");
        } else
            printf("Please give proper arguments for encoding\n");
    }
    // If operation is decoding
    else if (res == e_decode) {
        // Check if there are enough arguments for decoding
        if (argc >= 3) {
            res = read_and_validate(argv, &decinfo);
            if (res == e_success) {
                // Proceed with decoding process
                res = do_decoding(&decinfo);
                if (res == e_success)
                    printf(":::::::DECODING SUCCESSFUL::::::!\n");
                else
                    printf(":::::::DECODING FAILED::::::!\n");
            }
        } else
            printf("Please give proper arguments for decoding\n");
    }
    // If the operation type is unsupported
    else {
        printf("Check arguments, unsupported operation type\n");
    }
}

// Function to check the operation type based on command line arguments
OperationType check_operation_type(int argc,char *argv[])
{ 
    if (!strcmp(argv[1], "-e")) // Check if the argument indicates encoding
    {
        if(argc < 4)
        {
            printf("INFO : For Encoding Please pass minimum 4 arguments like ./a.out -e source_image_file secret_data_file [Destination_image_file]\n");
            return e_unsupported;
        }
        return e_encode;
    }
    else if (!strcmp(argv[1], "-d")) // Check if the argument indicates decoding
    {
        if(argc < 3)
        {
            printf("INFO : For Decoding Please pass minimum 3 arguments like ./a.out -d source_image_file [Destination_image_file]\n");
            return e_unsupported;
        }
        return e_decode;
    }
    else
        return e_unsupported; // Return unsupported if neither
}

// Function to read and validate the arguments for encoding
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo) {
    printf("\t\t\t\t\t\t:::::::VALIDATION STARTED :::::::\n");

    // Allocate memory for file names
    encInfo->src_image_fname = malloc(256 * sizeof(char));
    encInfo->secret_fname = malloc(256 * sizeof(char));
    encInfo->stego_image_fname = malloc(256 * sizeof(char));

    // Check for successful memory allocation
    if (encInfo->src_image_fname == NULL || encInfo->secret_fname == NULL || encInfo->stego_image_fname == NULL) {
        printf("Memory allocation failed\n");
        return e_failure;
    }

    // Validate source image file extension
    char *p = strstr(argv[2], ".bmp");
    if (p == NULL) {
        printf("ERROR: Source image must have a .bmp extension\n");
        return e_failure;
    }
    strcpy(encInfo->src_image_fname, argv[2]);

    // Validate secret file extension
    p = strstr(argv[3], ".txt");
    if (p == NULL) {
        printf("ERROR: Secret data file must have a .txt extension\n");
        return e_failure;
    }
    strcpy(encInfo->secret_fname, argv[3]);

    // Check if the fourth argument is NULL (if not passed)
    if (argv[4] == NULL) {
        // Use the default "output.bmp" for stego image
        strcpy(encInfo->stego_image_fname, "output.bmp");
    } else {
        // Otherwise, use the provided argument
        strcpy(encInfo->stego_image_fname, argv[4]);
    }

    printf("\t\t\t\t\t\t:::::::VALIDATION COMPLETED :::::::\n");
    return e_success;
}


// Function to read and validate the arguments for decoding
Status read_and_validate(char *argv[], Dec_Info *decinfo) {
    printf("\t\t\t\t\t\t:::::::VALIDATION STARTED :::::::\n");

    // Allocate memory for input and output file names
    decinfo->input_fname = malloc(256 * sizeof(char));
    decinfo->output_fname = malloc(256 * sizeof(char));

    // Check for successful memory allocation
    if (decinfo->input_fname == NULL || decinfo->output_fname == NULL) {
        printf("Memory allocation failed\n");
        return e_failure;
    }

    // Validate input file extension
    char *p = strstr(argv[2], ".bmp");
    if (p == NULL) {
        printf("ERROR: Input file must have a .bmp extension\n");
        return e_failure;
    }
    strcpy(decinfo->input_fname, argv[2]);

    // Set output file name or use default
    if (argv[3] == NULL) {
        // If the third argument (output file) is not passed, use default "secret_output.txt"
        strcpy(decinfo->output_fname, "secret_output.txt");
    } else {
        // Otherwise, use the provided file name for the output
        strcpy(decinfo->output_fname, argv[3]);
    }

    printf("\t\t\t\t\t\t:::::::VALIDATION COMPLETED :::::::\n");
    return e_success;
}

