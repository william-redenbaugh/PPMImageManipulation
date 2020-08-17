#include "io_handler.h"

/*
*   @brief Allows us to get file from the file path
*   @returns file_information_t file struct. 
*/
file_information_t prompt_file_information(void){
    // Array pointer to all our file stuff
    char *in_arr = (char*)malloc(sizeof(char) * 512);
    
    // Read in that information
    printf("Please input the file name to be load: ");
    scanf("%s", in_arr);

    file_information_t file_info; 
    file_info.str_ptr = in_arr; 
    file_info.str_size = strlen(in_arr);

    // Return our file information struct with relevant infomration
    return file_info; 
}

/*
*   @brief empties out the file information struct, and frees malloced memory. 
*   @params file_information_t *file_info pointer. 
*/
void free_file_information(file_information_t *file_info){
    // Just in case the string pointer is null(aka uninitialized, we don't want program to fail on accident)
    if(file_info->str_ptr != NULL)
        free(file_info->str_ptr); 
    
    file_info->str_size = 0; 
}

/*
*   @brief Prints out the main header: 
*/
void print_menu(void){
    printf("----------------------------------------------\n");
    printf("1: Load a PPM image\n");
    printf("2: Save the image in PPM and JPEG format\n");
    printf("3: Change the color image to black and white\n");
    printf("4: Sharpen the image\n");
    printf("5: Change the hue of image\n");
    printf("6: Test each function \n"); 
    printf("7: Test all functions in order\n");
    printf("8: Revert to previous image\n"); 
    printf("9: Regrets were made(revert to original image)\n");
    printf("10: Exit\n");
    printf("----------------------------------------------\n");
}

/*
*   @brief Which options are available?
*   @returns choice a month. 
*/
uint8_t prompt_choice(void){
    // Array pointer to all our file stuff
    int choice = 0; 

    // Read in that information
    printf("Please make your choice: ");
    scanf("%d", &choice);
    printf("\n");

    if(choice > 10)
        choice = 10; 
    if(choice < 1)
        choice = 1; 

    return (uint8_t)choice; 
}

/*
*   @brief Ask the use what value of hue they want to set their image to
*   @returns the hue value 
*/
uint16_t prompt_hue(void){
    // Array pointer to all our file stuff
    int choice = 0; 

    // Read in that information
    printf("Please enter the proppery hue value: ");
    scanf("%d", &choice);
    printf("\n");

    if(choice > 360)
        choice = 260; 
    if(choice < 0)
        choice = 0; 

    return (uint16_t)choice; 
}