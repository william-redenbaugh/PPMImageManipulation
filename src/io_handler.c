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
*   @brief Prints out the main header: 
*/
void print_menu(void){
    printf("----------------------------------------------\n");
    printf("1: Load a PPM image\n");
    printf("2: Save the image in PPM and JPEG format\n");
    printf("3: Change the color image to black and white\n");
    printf("4: Sharpen the image\n");
    printf("5: Change the hue of image\n");
    printf("6: Test all functions\n");
    printf("7: Exit\n");
    printf("----------------------------------------------\n");
}

/*
*   @brief Which options are available?
*   @returns choice a month. 
*/
uint8_t prompt_choice(void){
    // Array pointer to all our file stuff
    uint8_t choice = 0; 

    // Read in that information
    printf("Please make your choice: ");
    scanf("%d", &choice);
    printf("\n");

    // Energy out of bounds. 
    if(choice > 7)
        choice = 7; 
    return choice; 
}