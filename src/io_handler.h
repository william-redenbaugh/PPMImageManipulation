#ifndef _IO_HANDLER_H
#define _IO_HANDLER_H

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

/*
*   @brief Struct that carries all of our file information
*   @notes, used for io purposes so that 
*/
typedef struct{
    char* str_ptr;  
    uint32_t str_size; 
}file_information_t; 

file_information_t prompt_file_information(void);


#endif 