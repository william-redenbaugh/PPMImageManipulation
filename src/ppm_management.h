#ifndef __PPM_MANAGMENT_H
#define __PPM_MANAGMENT_H

#include <stdio.h> 
#include <string.h> 
#include <assert.h> 
#include <stdint.h> 
#include <stdlib.h> 
#include <math.h> 

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

typedef enum{
    IMAGE_NOT_UNPACKED, 
    IMAGE_FILE_READ_NO_UNPACK, 
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

/*
*   @brief Takes in a file path, along with the length of the path, and copies the photo information in
*   @params char *file_path, pointer to the begining char string 
*   @params size_t file_path_len size of the string .
*/
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
*   @brief we take in image data and we "age" it so that it looks older.
*   @params image_info_t previous image data
*   @returns new image struct.  
*/
image_info_t age_image_data(image_info_t image_info); 

/*
*   @brief Generates a black and white image copy of image_info_t placed in
*   @notes It's a new image, allowing us to have an old and new copy of the original image
*   @params image_info_t image_info(source information)
*   @returns image_info_t new file with greyscale information
*/
image_info_t greyscale_image_data(image_info_t image_info);

/*
*   @brief Takes in an image information struct, and returns a sharpened copy of that image
*   @notes Previous image info is retained so we have a copy, feel free to delete that copy later on if you see fit. 
*   @params image_info_t image_info(the image that we will process)
*   @returns copy of image with sharpening applied.  
*/
image_info_t sharpen_image_data(image_info_t image_info);

/*
*   @brief Takes in an image information struct, and returns a hue modified copy of that image
*   @notes Previous image info is retained so we have a copy, feel free to delete that copy later on if you see fit. 
*   @params image_info_t image_info(the image that we will process)
*   @returns copy of image with sharpening applied.  
*/
image_info_t change_hue(image_info_t image_info, double hue); 

/*
*   @brief So that we don't end up with tons of memory issues, we clear out our image array buffer. s
*   @notes If you enable debug mode, then we also clear our the struct since sometime's I'm trying to read that data   
*   for debugging purposes, and it comes in handy. 
*/
void free_image_data_mem(image_info_t *image_info);

#endif 