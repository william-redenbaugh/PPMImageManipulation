#ifndef __PPM_MANAGMENT_H
#define __PPM_MANAGMENT_H

#include <stdio.h> 
#include <string.h> 
#include <assert.h> 
#include <stdint.h> 
#include <stdlib.h> 

// If we want a specific height 
//#define SPECIFIC_HIEGHT
#ifdef SPECIFIC_HIEGHT
#define WIDTH   600
#define HIEGHT  400
#endif

// Image infomration
typedef struct{
    // Size of image
    uint32_t x; 
    uint32_t y; 
    
    // Maximum value of image. 
    uint16_t maxval; 

    // Pointer to the array containing the image information
    uint16_t *image_arr;
}image_data_t;

typedef enum{
    PIXEL_SET_SUCCESS, 
    PIXEL_SET_FAILIURE_OUT_OF_BOUNDS, 
    PIXEL_SET_FAILIURE_INVALID_ARR_PTR, 
    PIXEL_SET_FAILIURE_INVALID_IMAGE_PTR, 
    PIXEL_SET_FAILIURE_MAXRES_OUT_OF_BOUNDS
}pixel_set_get_return_t;

typedef struct{
    // Pointer to the primary image data array and text information
    image_data_t *image_data; 

    // RGB value that we want to progrma
    uint16_t r; 
    uint16_t g; 
    uint16_t b; 
    
    // X and Y position of the pixel we want to set. 
    uint16_t x; 
    uint16_t y; 

}set_get_pixel_t; 

/*
*   @brief Allows us to set a pixel in the set_get_pixel_ptr
*   @params set_get_pixel_t. 
*/
pixel_set_get_return_t set_pixel(set_get_pixel_t sp); 

/*
*   @brief Allows us to get a pixel value. 
*   @notes set_get_pixel_t pointer
*/
pixel_set_get_return_t get_pixel(set_get_pixel_t *sp);

//#define SPECIFIC_HIEGHT
#define WIDTH   600
#define HIEGHT  400

typedef enum{
    IMAGE_NOT_UNPACKED, 
    IMAGE_UNPACK_SUCCESS, 
    IMAGE_UNPACK_FAIL_FILE_DNE, 
    IMAGE_UNPACK_FAIL_TYPE_FAIL, 
    IMAGE_UNPACK_FILE_SIZE_WRONG, 
    IMAGE_UNPACK_FAIL_RES_UNPACK_FAIL
}image_unpack_status_t; 

typedef struct{
    // Image unpack status
    image_unpack_status_t unpack_status; 
    
    // PPM file type 
    uint8_t p_val; 

    // Pointer to file object. 
    FILE *file_ptr;

    // Size of the file
    size_t file_size; 

    // Struct that will hold all of our image information
    image_data_t image_dat; 
}image_info_t;

image_info_t unpack_image(char *file_path, size_t file_path_len);

typedef enum{
    FILE_WRITE_SUCESS, 
    FILE_WRITE_NAME_INVALID, 
    FILE_WRITE_INVALID_IMAGE_PTR, 
    FILE_WRITE_INVALID_IMAGE_ARRAY_PTR
}file_write_status_t; 

/*
*   @brief Allows us to take in a file new and our image information, and write that as a file. 
*   @params char file[] string array that holds our file information
*   @params size_t file_size, size of string array(not used atm but maybe in the future lmao);
*   @params image_info_t image_info struct that has everything needed to burn the image(size, x and y size, other stuff ig);
*/
extern file_write_status_t write_file(char file[], size_t file_size, image_info_t image_info);

/*
*   @brief Generates a black and white image copy of image_info_t placed in
*   @notes It's a new image, allowing us to have an old and new copy of the original image
*   @params image_info_t image_info(source information)
*   @returns image_info_t new file with greyscale information
*/
image_info_t greyscale_image_data(image_info_t image_info);

#endif 