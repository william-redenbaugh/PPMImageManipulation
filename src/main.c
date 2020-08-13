// Primary libraries.
#include <stdio.h> 
#include <string.h> 
#include <math.h> 

// Our ppmmanagement libraries 
#include "ppm_management.h"

/*
    Todo list to complete project: 
    "-" means task to complete
    "*" means task has been completed 
    
    - Prompt input for desired file *
    - Parse out important data from file
*/
int main(void){
    
    char arr[] = "file.ppm";
    unpack_image_info_t image_unpack = unpack_image(arr, sizeof(arr));
    return 0; 
}