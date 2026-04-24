#include <stdio.h>
#include "decode.h"
#include "types.h"
#include <string.h>
#include "common.h"

/* Function Definitions */

/* Read and validate decoding arguments
 * Input: argc, argv, DecodeInfo structure
 * Output: Fills DecodeInfo with valid file names
 * Description:
 * Validates the stego image file, checks its extension (.bmp),
 * and sets the output secret file name (default or user-provided).
 */
Status read_and_validate_decode_args(int argc,char *argv[] , DecodeInfo *decInfo)
{
    // Validate argument count
    if (argc != 3 && argc != 4)  
    {
        printf("\033[1;36m❌ ERROR: Usage: ./decode <stego.bmp> [output_file]\033[0m\n");
        return e_failure;
    }

    // Validate stego image file (must end with .bmp)
    int len = strlen(argv[2]);
    if (len < 3 || strcmp(argv[2] + len - 4,".bmp") != 0) 
    {
        printf("\033[1;36m❌ ERROR: Stego image must be .bmp\033[0m\n");
        return e_failure;
    }

    // Copy filename safely into the fixed-size array 
    strcpy(decInfo->stego_image_fname,argv[2]);

    // Set output file name 
    if(argc == 4) 
    {
        strcpy(decInfo->secret_fname, argv[3]);
    }
    else 
    {
        strcpy(decInfo->secret_fname, "output_stego");
    }

    printf("\033[1;36m🟢 Decoding inputs validated successfully.\033[0m\n");
    return e_success ;
}

/* Perform decoding process */
Status do_decoding(DecodeInfo *decInfo)
{
    printf("\033[1;36m📂 Opening stego image: %s\033[0m\n", decInfo->stego_image_fname);

    if (skip_bmp_header(decInfo) != e_success)
    {
        printf("\033[1;36m❌ ERROR: Failed to skip BMP header\033[0m\n");
        return e_failure;
    }

    if (decode_magic_string(MAGIC_STRING, decInfo) != e_success)
    {
        printf("\033[1;36m❌ ERROR: Magic string mismatch\033[0m\n");
        return e_failure;
    }
    printf("\033[1;36m🔑 Magic string verified\033[0m\n");

    if (decode_secret_file_extn_size(decInfo) != e_success)
    {
        printf("\033[1;36m❌ ERROR: Failed to decode extension file size\033[0m\n");
        return e_failure;
    }

    if (decode_secret_file_extn(decInfo) != e_success)
    {
        printf("\033[1;36m❌ ERROR: Failed to decode extension\033[0m\n");
        return e_failure;
    }

    if (decode_secret_file_size(decInfo) != e_success)
    {
        printf("\033[1:36m❌ ERROR: Failed to decode secret file size\033[0m\n");
        return e_failure;
    }

    if (decode_secret_file_data(decInfo) != e_success)
    {
        printf("\033[1;36m❌ ERROR: Failed to decode secret data\033[0m\n");
        return e_failure;
    }

    printf("\033[1;36m🏆 SUCCESS: Decoding completed! Saved as '%s'\033[0m\n", decInfo->secret_fname);
    return e_success;
}

/* Skip BMP header */
Status skip_bmp_header(DecodeInfo *decInfo)
{
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "rb");

    if (!decInfo->fptr_stego_image)
    {
        perror("\033[1;36m❌ ERROR: fopen stego image\033[0m");
        return e_failure;
    }

    // Already correct: 54 bytes
    if (fseek(decInfo->fptr_stego_image, 54L, SEEK_SET) != 0)
    {
        printf("\033[1;36m❌ ERROR: Failed to seek past BMP header\033[0m\n");
        return e_failure;
    }

    // skips 54 bytes which is header file and msg is printed
    printf("\033[1;36m📄 Skipped 54-byte BMP header\033[0m\n");

    return e_success;
}

/* Decode magic string */
Status decode_magic_string(char *magic_string,DecodeInfo *decInfo)
{
    int len = strlen(magic_string);
    char buffer[16] = {0};

    if (decode_data_from_image(buffer, len, decInfo->fptr_stego_image) != e_success)
        return e_failure;

    if (strcmp(buffer, magic_string) != 0)
    {
        printf("\033[1;36m❌ ERROR: Expected magic '%s', got '%s'\033[0m\n", magic_string, buffer);
        return e_failure;
    }
    return e_success;
}

/* Decode secret file extn size */
Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    unsigned char buffer[32];

    if(fread(buffer,1,32,decInfo->fptr_stego_image) != 32)
        return e_failure;

    if (decode_size_from_lsb(&decInfo->size_secret_file_extn, buffer) != e_success)
        return e_failure;

    printf("\033[1;36m📝 Decoded extension size = %ld\033[0m\n", decInfo->size_secret_file_extn);
    if (decInfo->size_secret_file_extn <= 0 || decInfo->size_secret_file_extn >= MAX_FILE_SUFFIX_) {
        printf("\033[1;36m❌ ERROR: invalid decoded extension size %ld\033[0m\n", decInfo->size_secret_file_extn);
        return e_failure;
    }

    return e_success;
}

/* Decode secret file extn */
Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    memset(decInfo->extn_secret_file, 0, sizeof(decInfo->extn_secret_file));

    if (decode_data_from_image(decInfo->extn_secret_file, (int)decInfo->size_secret_file_extn, decInfo->fptr_stego_image) != e_success) 
    {
        printf("\033[1;36m❌ ERROR: Failed to decode secret file extension\033[0m\n");
        return e_failure;
    }

    decInfo->extn_secret_file[decInfo->size_secret_file_extn] = '\0';

    strcat(decInfo->secret_fname, decInfo->extn_secret_file);
    printf("\033[1;36m📁 Output file = '%s'\033[0m\n", decInfo->secret_fname);

    decInfo->fptr_secret = fopen(decInfo->secret_fname, "wb");
    if (!decInfo->fptr_secret)
    {
        perror("\033[1;36m❌ ERROR: fopen output file\033[0m");
        return e_failure;
    }

    return e_success;
}

/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo)
{
    unsigned char buffer[32];

    if(fread(buffer,1,32,decInfo->fptr_stego_image) != 32) 
        return e_failure;

    if(decode_size_from_lsb(&decInfo->size_secret_file,buffer) != e_success)
        return e_failure;

    printf("\033[1;36m📦 Secret file size = %ld bytes\033[0m\n", decInfo->size_secret_file);
    if (decInfo->size_secret_file < 0)
        return e_failure;

    return e_success;
}

/* Decode secret file data */
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    if (decInfo->size_secret_file <= 0)
        return e_failure;
        
    unsigned char buffer[8];
    char ch;

    for (long i = 0; i < decInfo->size_secret_file; i++)
    {
        if (fread(buffer, 1, 8, decInfo->fptr_stego_image) != 8)
            return e_failure;

        if (decode_byte_from_lsb(&ch, (char *)buffer) != e_success)
            return e_failure;

        if (fwrite(&ch, 1, 1, decInfo->fptr_secret) != 1)
            return e_failure;
    }

    return e_success;
}

/* Decode data from image */
Status decode_data_from_image(char *output, int size, FILE *fptr_src_image)
{
    unsigned char img_buffer[8];
    char ch;
    for (int i = 0; i < size; i++)
    {
        if (fread(img_buffer, 1, 8, fptr_src_image) != 8)
        {
            printf("\033[1;36m❌ ERROR: Not enough image data to decode byte %d\033[0m\n", i);
            return e_failure;
        }
        if (decode_byte_from_lsb(&ch, img_buffer) != e_success)
        {
            printf("\033[1;36m❌ ERROR: Failed to decode byte %d\033[0m\n", i);
            return e_failure;
        }
        output[i] = ch;
    }
    output[size] = '\0';
    return e_success;
}

/* Decode byte from LSBs */
Status decode_byte_from_lsb(char *data, unsigned char *image_buffer)
{
    *data = 0;

    for (int i = 0; i < 8; i++)
        *data |= ((image_buffer[i] & 1) << (7 - i));

    return e_success;
}

/* Decode size from LSBs */
Status decode_size_from_lsb(long int *data,unsigned char *buffer)
{
    *data = 0;

    for(int i = 0 ; i < 32 ; i++)
        *data = (*data) | ((buffer[i] & 1) << (31 - i));

    return e_success;
}
