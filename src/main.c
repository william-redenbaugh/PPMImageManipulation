// Primary libraries.
#include <stdio.h> 
#include <string.h> 
#include <math.h> 

// Our ppmmanagement libraries 
#include "ppm_management.h"

// Our IOHandler libraries
#include "io_handler.h"

static inline uint8_t run(image_info_t *previous_image, image_info_t *current_image, image_info_t *original_image); 
static inline image_info_t prompt_image_filepath(void); 
static inline void prompt_file_write(image_info_t image_unpack);
static inline void revert_changes(image_info_t *previous_image, image_info_t *current_image, image_info_t *original_image);
static inline void revert_original(image_info_t *previous_image, image_info_t *current_image, image_info_t *original_image);
static inline void test_each_function(image_info_t *previous_image, image_info_t *current_image, image_info_t *original_image); 
static inline void test_all_functions_in_order(image_info_t *previous_image, image_info_t *current_image, image_info_t *original_image); 
static inline void process_greyscale(image_info_t *previous_image, image_info_t *current_image, image_info_t *original_image); 
static inline void process_sharpen(image_info_t *previous_image, image_info_t *current_image, image_info_t *original_image); 
static inline void process_hue(image_info_t *previous_image, image_info_t *current_image, image_info_t *original_image); 
static inline void revert_checking(image_info_t *previous_image, image_info_t *current_image, image_info_t *original_image); 

int main(void){
    // Since we need to have a file to unpack in the first place: 
    image_info_t original_image = prompt_image_filepath();
    // Our current image. 
    image_info_t current_image = original_image; 
    
    // previous image is set to current image so that can't revert to NULL. 
    // Limitation of program is that we can only go back one change, perhaps we can make an array 
    // Linked list, but I don't think that is necesary. 
    image_info_t previous_image = current_image; 

    while(1){
        // If run is returned as 0, then we know to exit the program. 
        if(!run(&previous_image, &current_image, &original_image))
            return 0;  
    }
    // Satisfy compiler warnings 
    return 0; 
}

/*
*   @brief Runs the main program, that prompts the user on what to do 
*   @return bool of whether or not to end the progrma 
*/
static inline uint8_t run(image_info_t *previous_image, image_info_t *current_image, image_info_t *original_image){
    print_menu(); 

    // Get the choice from the user. 
    switch(prompt_choice()){
    // If we want to update the file path to a new "saved" file. 
    case(1): 
        free_image_data_mem(original_image);
        free_image_data_mem(current_image); 
        *original_image = prompt_image_filepath(); 
        *current_image = *original_image; 
        printf("source file image changed\n");
    break; 

    // If we want to write our changes to a file. 
    case(2): 
        prompt_file_write(*current_image);
        printf("Image saved\n");
    break;

    // If we want to convert to black and white 
    case(3):
        process_greyscale(previous_image, current_image, original_image);
    break;

    // If we want to sharpen our image. 
    case(4): 
        process_sharpen(previous_image, current_image, original_image);
    break;

    // If we want to change the hue of our image. 
    case(5): 
        process_hue(previous_image, current_image, original_image);
    break;
    // If we want to do all of these operations in a row. 
    case(6):
        test_each_function(previous_image, current_image, original_image);     
    break;

    // If we want to test each function individually. 
    case(7):
        test_all_functions_in_order(previous_image, current_image, original_image);
    break; 

    // We revert to the previous changes.  
    case(8): 
        revert_changes(previous_image, current_image, original_image);
    break;

    // If we revert to the original image from the file. 
    case(9): 
        revert_original(previous_image, current_image, original_image);

    // If we want to "exit" out of the program. 
    case(10): 
        // Exit out of the program. 
        return 0; 
    break; 

    default:
    break; 
    }  
    // Otherwise we return 1 to tell the owner function that we shouldn't exit the program. 
    return 1; 
}   

/*
*   @brief attemps to get the file path for an image, and sends the struct over to parent function
*   @returns image_info_t struct containing all the new image information
*/
static inline image_info_t prompt_image_filepath(void){
    image_info_t image_unpack; 
    // Clear our the struct. 
    memset(&image_unpack, 0, sizeof(image_unpack));
    while(image_unpack.unpack_status != IMAGE_UNPACK_SUCCESS){
        // Prompt our user for our file information
        file_information_t file_info = prompt_file_information();
        image_unpack = unpack_image(file_info.str_ptr, file_info.str_size); 
        if(image_unpack.unpack_status != IMAGE_UNPACK_SUCCESS){
            printf("Message couldn't be found, please try again. \n");    
        }
        free_file_information(&file_info); 
    }
    return image_unpack; 
}

/*
*   @brief Prompts the user for the file name that we want to save our new file in, and then writes the file
*   @params image_info_t image_unpack The image that we want to save as a file.  
*   @notes there is some checking
*/
static inline void prompt_file_write(image_info_t image_unpack){
    
    // Checking a couple red flags as to whether or not we are getting a value image unpacking tool 
    if(image_unpack.unpack_status != IMAGE_UNPACK_SUCCESS || image_unpack.image_dat.image_arr == NULL || image_unpack.file_size == 0){
        printf("File cannot be saved since image is unvalid\n");
    }

    // Keeps on prompting until we provide a valid file name 
    while(1){
        file_information_t file_info = prompt_file_information(); 
        if(file_info.str_size > 0){
            write_file(file_info.str_ptr, file_info.str_size, image_unpack); 
            return; 
        }
    }
}

/*
*   @brief Subfunction that takes all the save image information 
*   @notes Allows us to revert one change back(at most no more), so if we make a mistake we can go back one. 
*   @params image_info_t *previous_image( the previous image that allows us to save prior work as a buffer for messups)
*   @params image_info_t *current_image (the current image that will get changed to relfect the changes that we are asking)
*   @params image_info_t *original_image(original source file image). This is mostly here so we can ensure that the image is processed properly. 
*/
static inline void revert_changes(image_info_t *previous_image, image_info_t *current_image, image_info_t *original_image){
    // If the current image data isn't the original image's image data, then we free it up
    if(current_image->image_dat.image_arr != previous_image->image_dat.image_arr)
        free_image_data_mem(current_image);
    *current_image = *previous_image; 
}

/*
*   @brief Subfunction that takes all the save image information 
*   @notes Allows us to revert all of our changes to the original file if we message up bad
*   @params image_info_t *previous_image( the previous image that allows us to save prior work as a buffer for messups)
*   @params image_info_t *current_image (the current image that will get changed to relfect the changes that we are asking)
*   @params image_info_t *original_image(original source file image). This is mostly here so we can ensure that the image is processed properly. 
*/
static inline void revert_original(image_info_t *previous_image, image_info_t *current_image, image_info_t *original_image){
    // If the current image isn't the original image's heap allocated image data, then we attempt to free it up
    if(current_image->image_dat.image_arr != original_image->image_dat.image_arr)
        free_image_data_mem(current_image); 
    *current_image = *original_image;  
}

/*
*   @brief Subfunction that takes all the save image information 
*   @notes Tests out all of the different subfunctions individually, used for grading purposes. 
*   @params image_info_t *previous_image( the previous image that allows us to save prior work as a buffer for messups)
*   @params image_info_t *current_image (the current image that will get changed to relfect the changes that we are asking)
*   @params image_info_t *original_image(original source file image). This is mostly here so we can ensure that the image is processed properly. 
*/
static inline void test_each_function(image_info_t *previous_image, image_info_t *current_image, image_info_t *original_image){
    // Complete process calculations
    process_hue(previous_image, current_image, original_image); 
    char hue_process[] = "hue.ppm"; 
    write_file(hue_process, sizeof(hue_process), *current_image); 

    // We revert changes so that each image will be a different copy of the sharpen
    revert_changes(previous_image, current_image, original_image);
    process_sharpen(previous_image, current_image, original_image);
    char sharpen_process[] = "sharpen.ppm"; 
    write_file(sharpen_process, sizeof(sharpen_process), *current_image); 
    
    // We revert changes so that each image will be a different copy of the sharpen
    revert_changes(previous_image, current_image, original_image);
    process_greyscale(previous_image, current_image, original_image);
    char greyscale_process[] = "bw.ppm"; 
    write_file(greyscale_process, sizeof(greyscale_process), *current_image);
}

/*
*   @brief Subfunction that takes all the save image information 
*   @notes Tests all the different subfunctions in order, composing an abomination of an image that uses all filters. 

*   @params image_info_t *previous_image( the previous image that allows us to save prior work as a buffer for messups)
*   @params image_info_t *current_image (the current image that will get changed to relfect the changes that we are asking)
*   @params image_info_t *original_image(original source file image). This is mostly here so we can ensure that the image is processed properly. 
*/
static inline void test_all_functions_in_order(image_info_t *previous_image, image_info_t *current_image, image_info_t *original_image){
    process_hue(previous_image, current_image, original_image);  
    process_sharpen(previous_image, current_image, original_image);
    process_greyscale(previous_image, current_image, original_image);
    // Write file to image. 
    prompt_file_write(*current_image);
    printf("Image saved\n");
}

/*
*   @brief Subfunction that takes all the save image information 
*   @notes Changes current image to reflect the "greyscale" filter
*   @notes changes the previous image to the current image. 
*   @params image_info_t *previous_image( the previous image that allows us to save prior work as a buffer for messups)
*   @params image_info_t *current_image (the current image that will get changed to relfect the changes that we are asking)
*   @params image_info_t *original_image(original source file image). This is mostly here so we can ensure that the image is processed properly. 
*/
static inline void process_greyscale(image_info_t *previous_image, image_info_t *current_image, image_info_t *original_image){
    printf("Begin greyscale processing...");
    image_info_t newimage = greyscale_image_data(*current_image);
    revert_checking(previous_image, current_image, original_image);
    // The current image is then set to the new image. 
    *current_image = newimage; 
    printf("greyscale processing done\n"); 
}

/*
*   @brief Subfunction that takes all the save image information. 
*   @notes Changes current image to reflect the "sharpen" filter
*   @notes changes the previous image to the current image. 
*   @params image_info_t *previous_image( the previous image that allows us to save prior work as a buffer for messups)
*   @params image_info_t *current_image (the current image that will get changed to relfect the changes that we are asking)
*   @params image_info_t *original_image(original source file image). This is mostly here so we can ensure that the image is processed properly. 
*/
static inline void process_sharpen(image_info_t *previous_image, image_info_t *current_image, image_info_t *original_image){
    printf("Begin sharpen processing...");
    image_info_t newimage = sharpen_image_data(*current_image);
    revert_checking(previous_image, current_image, original_image);
    *current_image = newimage; 
    printf("Sharpening has been processed\n");
}

/*
*   @brief Subfunction that takes all the save image information. 
*   @notes Changes current image to reflect the "hue change" filter
*   @notes changes the previous image to the current image. 
*   @params image_info_t *previous_image( the previous image that allows us to save prior work as a buffer for messups)
*   @params image_info_t *current_image (the current image that will get changed to relfect the changes that we are asking)
*   @params image_info_t *original_image(original source file image). This is mostly here so we can ensure that the image is processed properly. 
*/
static inline void process_hue(image_info_t *previous_image, image_info_t *current_image, image_info_t *original_image){
    printf("Begining hue processing...");
    uint16_t hue = prompt_hue(); 
    image_info_t newimage = change_hue(*current_image, (double)hue);
    revert_checking(previous_image, current_image, original_image); 
    *current_image = newimage;

    printf("Hue has been processed\n");
}

/*
*   @brief Ensures that when we save update our previous image to the current image's results, everything is 
*   done properly so that we are freeing up memory properly and such. 
*/
static inline void revert_checking(image_info_t *previous_image, image_info_t *current_image, image_info_t *original_image){
    // If the previous image isn't the original image. 
    if(previous_image->image_dat.image_arr != original_image->image_dat.image_arr){
        // If it isn't the original  image or the current image, we free up the heap memory block. 
        if(previous_image->image_dat.image_arr != current_image->image_dat.image_arr)
            free_image_data_mem(previous_image); 
        
        // After we have free the previous image data's block, save the previous 
        // "current" image. 
        previous_image = current_image; 
    }
}