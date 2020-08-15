#include "io_handler.h"

file_information_t prompt_file_information(void){
    // Array pointer to all our file stuff
    char *in_arr = (char*)malloc(sizeof(char) * 512);
    
    // Read in that information
    printf("Please input the file name to be loaded?: ");
    scanf("%s", in_arr);

    file_information_t file_info; 
    file_info.str_ptr = in_arr; 
    file_info.str_size = strlen(in_arr);

    // Return our file information struct with relevant infomration
    return file_info; 
}

