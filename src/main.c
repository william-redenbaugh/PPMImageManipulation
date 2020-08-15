// Primary libraries.
#include <stdio.h> 
#include <string.h> 
#include <math.h> 

// Our ppmmanagement libraries 
#include "ppm_management.h"

// Our IOHandler libraries
#include "io_handler.h"

/*
    Todo list to complete project: 
    "-" means task to complete
    "*" means task has been completed 
    
    - Prompt input for desired file *
    - Parse out important data from file
*/
int main(void){
    // Prompt our user for our file information
    //file_information_t file_info = prompt_file_information();
    
    // Unpack our image. 
    char file[] = "file.ppm";
    //unpack_image_info_t image_unpack = unpack_image(file_info.str_ptr, file_info.str_size);
    image_info_t image_unpack = unpack_image(file, sizeof(file));
    
    image_info_t blacknwhite_image = greyscale_image_data(image_unpack);
    char newfile[] = "newfile.ppm";
    // Write the image data to a file. 
    write_file(newfile, sizeof(newfile), blacknwhite_image);
    return 0; 
}