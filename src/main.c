// Primary libraries.
#include <stdio.h> 
#include <string.h> 
#include <math.h> 

// Our ppmmanagement libraries 
#include "ppm_management.h"

// Our IOHandler libraries
#include "io_handler.h"

void run(void){
    // Allows us to get the image unpacking, but checking the image struct
    image_info_t image_unpack;
    memset(&image_unpack, 0, sizeof(image_unpack)); 

    // We go back here if 
    back_prompt:
    
    // We print out the menu to the users. 
    print_menu();

    switch(prompt_choice()){
    case(1):{
        file_information_t file_info = prompt_file_information();
        image_unpack = unpack_image(file_info.str_ptr, file_info.str_size);
    }
    break; 
    
    case(2): 

    break; 

    case(3): 

    break; 
    
    case(4): 

    break; 
    
    case(5): 

    break; 
    
    case(6): 

    break; 
    
    case(7):

    break; 
    
    }
}

/*
    Todo list to complete project: 
    "-" means task to complete
    "*" means task has been completed 
    
    - Prompt input for desired file *
    - Parse out important data from file
*/
int main(void){
    
    while(1){
        run();
    }

    // Prompt our user for our file information
    file_information_t file_info = prompt_file_information();
    
    // Unpack our image. 
    char file[] = "file.ppm";
    image_info_t image_unpack = unpack_image(file, sizeof(file));
    
    //image_info_t blacknwhite_image = greyscale_image_data(image_unpack);
    //image_info_t sharpen_image = sharpen_image_data(image_unpack);
    //image_info_t change_hue_16 = change_hue(image_unpack, 50);
    image_info_t old_image = age_image_data(image_unpack);

    char newfile[] = "newfile.ppm";
    // Write the image data to a file. 
    write_file(newfile, sizeof(newfile), old_image);

    // Don't wanna leave any free data lying around, so lets clear it!
    free_image_data_mem(&old_image);
    free_image_data_mem(&image_unpack);
    return 0; 
}