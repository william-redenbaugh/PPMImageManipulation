#ifndef __PPM_MANAGMENT_H
#define __PPM_MANAGMENT_H

// Including the libraries that we wanna deal with
// Standard input output libary that will let us accces files 
#include <stdio.h> 
// String library for random string information
#include <string.h> 
// Error and debug testing
#include <assert.h> 
// Standard library that has a bunch of declarations n shit
#include <stdint.h> 
#include <stdlib.h> 

// Image infomration
typedef struct{
    // Size of image
    uint32_t x; 
    uint32_t y; 
    
    // Pointer array to the image
    uint8_t** image; 
}image_data_t;

typedef struct{
    uint32_t x; 
    uint32_t y; 
}image_params_t; 

image_data_t process_image(FILE *fp, image_params_t image_data);

typedef enum{
    IMAGE_UNPACK_SUCCESS, 
    IMAGE_UNPACK_FAIL_FILE_DNE, 
    IMAGE_UNPACK_FAIL_TYPE_FAIL, 
    IMAGE_UNPACK_FAIL_RES_UNPACK_FAIL
}image_unpack_status_t; 

typedef struct{
    // Image unpack status
    image_unpack_status_t unpack_status; 
    
    // Image size
    uint32_t x_size; 
    uint32_t y_size;

    // PPM file type 
    uint8_t p_val; 

    // Pointer to file object. 
    FILE *file_ptr;
    // Size of the file
    size_t file_size; 
}unpack_image_info_t;

unpack_image_info_t unpack_image(char *file_path, size_t file_path_len);
#endif 