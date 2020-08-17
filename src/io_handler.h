#ifndef _IO_HANDLER_H
#define _IO_HANDLER_H

// Including the libraries that we wanna deal with
// Standard input output libary that will let us accces files 
#include <stdio.h> 
#include <string.h> 
#include <assert.h> 
#include <stdint.h> 
#include <stdlib.h> 

/*
*   @brief Struct that carries all of our file information
*   @notes, used for io purposes so that 
*/
typedef struct{
    char* str_ptr;  
    size_t str_size; 
}file_information_t; 

/*
*   @brief Allows us to get file from the file path
*   @returns file_information_t file struct. 
*/
file_information_t prompt_file_information(void);

/*
*   @brief Prints out the main header: 
*/
void print_menu(void);

/*
*   @brief Which options are available?
*   @returns choice a month. 
*/
uint8_t prompt_choice(void);

/*
*   @brief Ask the use what value of hue they want to set their image to
*   @returns the hue value 
*/
uint16_t prompt_hue(void);

/*
*   @brief empties out the file information struct, and frees malloced memory. 
*   @params file_information_t *file_info pointer. 
*/
void free_file_information(file_information_t *file_info);
#endif 