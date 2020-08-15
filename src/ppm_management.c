#include "ppm_management.h"

/*
*   @brief Allows us to enable a couple of debugging features. 
*   @params Disable this if you want better preformance at the cost of more error checking
*/
#define DEBUG_PPM

// Function declarations at the top. 
static inline void convert_image_array(image_info_t *img_info);
void unpack_p_resolution(image_info_t *img_info);
static inline void convert_image_array(image_info_t *img_info);
static inline image_info_t get_file_path(char *file_path); 
extern pixel_set_get_return_t set_pixel(set_get_pixel_t sp);
extern pixel_set_get_return_t get_pixel(set_get_pixel_t *sp);
extern file_write_status_t write_file(char file[], size_t file_size, image_info_t image_info);
inline void write_image_metadata(image_info_t *image_info, FILE *fp); 
inline void write_image_data(image_info_t *image_info, FILE *fp);

/*
    How is the image array stored in memory? That's an interested question that you have professor. I'm glad you asked. 
    Rather than generating a 2D array that will contain all of my information, I opted to create a 1D array that contains all the same information. 
    This was a design choice made by my desire for a "data oriented design". You could allocate a 2D array on the heap, but without understanding
    Linux's heap allocation system, we cannot garantee where that memory will actually be placed in memoryspace. For that reason, if we at least allocate
    one contiguous space where we can put all of our data, then we can at least retain some level of organization in memory. 

    One might say "why not just allocate a pre-allocated size?". My response would be "preposturous, what a lame program that only accepts a single resolution"

    For this reason I have this comment here. 

    The array is allocated on the heap, but in one single contiguous block. At a high level:
    -The first part of the array contains the red values. 
    -The second part of the array contains the green values. 
    -The third and final part of the array contains the blue values. 

    -Each part of the array goes like this (x0, y0), (x1, y0), (x2, y0)...(xN, y0), (x0, y1)

    example: 

    Array serialization information: 
    [rgb][rgb]      [r][r]  [g][g]  [b][b]      [r][r][r][r][g][g][g][g][b][b][b][b]
    [rgb][rgb]      [r][r]  [g][g]  [b][b]      
                =                           = Widthhe file path string, and will return the string. 
*   @params char *file_path, pointer to the begining char string 
*   @params size_t file_path_len size of the string .
*/
image_info_t unpack_image(char *file_path, size_t file_path_len){
    image_info_t img_info = get_file_path(file_path); 

    // If there was some kind of error with the program 
    if(img_info.unpack_status == IMAGE_UNPACK_FAIL_FILE_DNE)
        return img_info; 
        
    // Unpack the resultion
    unpack_p_resolution(&img_info);
    
    // Save the image as an array.
    convert_image_array(&img_info);

    // If the image was able to be unpacked properly. 
    printf("Image file read sucessfully :)\n");
    img_info.unpack_status = IMAGE_UNPACK_SUCCESS; 
    fclose(img_info.file_ptr);

    return img_info; 
}

/*
*   @brief Allows us to read a couple of strings from the array, and gets the pvalue, resolution and maxiumum color resolution 
*   @params image_info_t *img_info pointer to the img_info, has everything we need to unpack this stuff
*/
void unpack_p_resolution(image_info_t *img_info){
    printf("Attempting to unpack image!\n");
    // Buffer that we will use to unpack our first bits of information
    char buff[32];

    // Sometimes there are spaces that don't contain important information at the start 
    memset(buff, 0, sizeof(buff)); 
    while(!strlen(buff))
        fscanf(img_info->file_ptr, "%s", buff);

    // Parse out the p file type value
    img_info->p_val = (buff[1] - 48);

    // What type of image are we parsing out?
    printf("Image type: P%d \n", img_info->p_val);

    //Parse out the x size
    fscanf(img_info->file_ptr, "%s", buff);
    img_info->image_dat.x = atoi(buff);
    printf("Width: %s. ", buff); 

    // Parse out the y size. 
    fscanf(img_info->file_ptr, "%s", buff);    
    img_info->image_dat.y = atoi(buff);
    printf("Height: %s.\n", buff); 

    fscanf(img_info->file_ptr, "%s", buff);
    img_info->image_dat.maxval = atoi(buff);
    printf("Max color value: %s\n", buff);
    
    if(fgetc(img_info->file_ptr) != '\n'){
        printf("Carriage return failed!\n");
        assert(NULL);
    }
}

/*
*   @brief Convers our image into an array for use later. 
*   @params image_info_t *img_info array to all the image information
*/
static inline void convert_image_array(image_info_t *img_info){
    // Creates our array
    img_info->image_dat.image_arr = (uint16_t*)malloc(sizeof(uint16_t) * img_info->image_dat.x * img_info->image_dat.y * 3 + 100);
    
    // Setup set_get_pixel configuration struct.
    set_get_pixel_t sp; 
    sp.image_data = &img_info->image_dat; 
    
    for(uint32_t y = 0; y < img_info->image_dat.y; y++){
        for(uint32_t x = 0; x < img_info->image_dat.x; x++){
            sp.x = x; 
            sp.y = y; 
            // Get the rgb values. 
            sp.r = fgetc(img_info->file_ptr); 
            sp.g = fgetc(img_info->file_ptr); 
            sp.b = fgetc(img_info->file_ptr);
            set_pixel(sp);
        }
    }
}

/*
*   @brief given a file path, we generate an image_info_t struct that will start our image unpack process
*   @params char *file_path(file path string array )
*/
static inline image_info_t get_file_path(char *file_path){
    // Create our image info message. 
    image_info_t img_info; 
    // Open the file if possible. 
    FILE *fp = fopen(file_path, "r");
    
    // If we couldn't find the deseired file, we return out of the subroutine. 
    if(fp == NULL){
        printf("Couldn't find file\n");
        img_info.unpack_status = IMAGE_UNPACK_FAIL_FILE_DNE;
        // Error the hell outa here. 
        assert(fp);
        return img_info;    
    }

    // Set the file pointer. 
    img_info.file_ptr = fp; 
    printf("Found file successfully!\n");
    return img_info; 
}

/*
*   @brief allows us to get a set_get_pixel_t struct and set a pixel 
*   @params set_get_pixel_t sp struct that has all the information needed to set the pixel 
*   @returns pixel_set_get_return_t enumerated value that let's us know whether or not we were able to set the pixel successfully and where it went wrong. 
*/
extern pixel_set_get_return_t set_pixel(set_get_pixel_t sp){
#ifdef DEBUG_PPM 
    // If the pixel wasn't able to return properly
    if(sp.x >= sp.image_data->x || sp.y >= sp.image_data->y)
        return PIXEL_SET_FAILIURE_OUT_OF_BOUNDS; 

    // If we are trying to program pixel and we are tsp.y * sp.image_data->x + sp.xDS;
    
    if(sp.image_data == NULL)
        return PIXEL_SET_FAILIURE_INVALID_IMAGE_PTR; 
    
    if(sp.image_data->image_arr == NULL)
        return PIXEL_SET_FAILIURE_INVALID_ARR_PTR;
    
#endif 
    // Setting up our spot and spot offset values. 
    // This is the location of our data in the array. 
    uint32_t spot = sp.y * sp.image_data->x + sp.x; 
    uint32_t spot_offset = sp.image_data->y * sp.image_data->x;

    // Set the red value. 
    sp.image_data->image_arr[sp.y * sp.image_data->x + sp.x] = sp.r;
    // Set the green value
    sp.image_data->image_arr[sp.y * sp.image_data->x + sp.x +  spot_offset] = sp.g;
    // Set the blue value. 
    sp.image_data->image_arr[sp.y * sp.image_data->x + sp.x + spot_offset + spot_offset] = sp.b;
}

/*
*   @brief Given a set_get_pixel_t pointer, we get get a the pixel value to the arrya. 
*   @params set_get_pixel_t *sp pointer to the get_set_pixel array. 
*   @returns pixel_set_get_return_t(status of our retreivale of getting the pixel array)
*/
extern pixel_set_get_return_t get_pixel(set_get_pixel_t *sp){
#ifdef DEBUG_PPM 
    
    // If the pixel wasn't able to return properly
    if(sp->x >= sp->image_data->x || sp->y >= sp->image_data->y)
        return PIXEL_SET_FAILIURE_OUT_OF_BOUNDS; 

    // If the pointer to the image data is null
    if(sp->image_data == NULL)
        return PIXEL_SET_FAILIURE_INVALID_IMAGE_PTR; 
    
    // If the pointer to the image data array is null
    if(sp->image_data->image_arr == NULL)
        return PIXEL_SET_FAILIURE_INVALID_ARR_PTR;

#endif
    // Setting up our spot and spot offset values. 
    // This is the location of our data in the array. 
    uint32_t spot = sp->y * sp->image_data->x + sp->x; 
    uint32_t spot_offset = sp->image_data->y * sp->image_data->x;

    // Setting our rgb values. 
    sp->r = sp->image_data->image_arr[sp->y * sp->image_data->x + sp->x];
    sp->g = sp->image_data->image_arr[sp->y * sp->image_data->x + sp->x +  spot_offset];
    sp->b = sp->image_data->image_arr[sp->y * sp->image_data->x + sp->x + spot_offset + spot_offset];
}

/*
*   @brief Allows us to take in a file new and our image information, and write that as a file. 
*   @params char file[] string array that holds our file information
*   @params size_t file_size, size of string array(not used atm but maybe in the future lmao);
*   @params image_info_t image_info struct that has everything needed to burn the image(size, x and y size, other stuff ig);
*/
extern file_write_status_t write_file(char file[], size_t file_size, image_info_t image_info){
    // If our file name is invalid 
    if(file == NULL)
        return FILE_WRITE_NAME_INVALID;

    // If our array is invalid. 
    if(image_info.image_dat.image_arr == NULL)
        return FILE_WRITE_INVALID_IMAGE_ARRAY_PTR; 

    FILE *fp = fopen(file, "w");
    if(fp == NULL){
        printf("File couldn't read properly"); 
        assert(fp);
    }    

    // Writing our image metadata information
    write_image_metadata(&image_info, fp);

    // Writing our image data.
    write_image_data(&image_info, fp);

    // Close out our file to prevent data corruption
    fclose(fp);    
    printf("File written properly\n");
    
    return FILE_WRITE_SUCESS; 
}

/*
*   @brief Writes our image metadata to the file in questions
*   @notes Grabs image metadata from the image_info and sends it to the file pointer. 
*   @params image_info_t *image_info(image info metadata)
*   @params File *fp (file pointer) file manipulation object pointer. 
*/
inline void write_image_metadata(image_info_t *image_info, FILE *fp){
    // What type of ppm file are we writing?
    fprintf(fp, "P%d\n", image_info->p_val); 
    // Print out the width
    fprintf(fp, "%d ", image_info->image_dat.x);
    // Print out the hieght
    fprintf(fp, "%d\n", image_info->image_dat.y);
    // Print out the maximum bit value of the resolution
    fprintf(fp, "%d\n", image_info->image_dat.maxval);
}

/*
*   @brief Writes the actual image data from our image_info array into the file in question
*   @notes Might take a little while to run due to the shear size of the array
*   @params image_info_t *image_info pointer.
*   @params FILE *fp file pointer the we use to manipulate our file object
*/
inline void write_image_data(image_info_t *image_info, FILE *fp){
    
    set_get_pixel_t sp; 
    sp.image_data = &image_info->image_dat; 

    for(uint32_t y = 0; y < image_info->image_dat.y; y++){
        for(uint32_t x = 0; x < image_info->image_dat.x; x++){
            sp.x = x; 
            sp.y = y; 
            get_pixel(&sp);
            fputc(sp.r, fp); 
            fputc(sp.g, fp);
            fputc(sp.b, fp);
        }
    }
}

/*
*   @brief So that we don't end up with tons of memory issues, we clear out our image array buffer. s
*   @notes If you enable debug mode, then we also clear our the struct since sometime's I'm trying to read that data   
*   for debugging purposes, and it comes in handy. 
*/
void free_image_data_mem(image_info_t *image_info){
    // Free out our image array
    free(image_info->image_dat.image_arr); 
    
#ifdef DEBUG_PPM
    // Clear our struct information since none of it will be relevant anymore anyway
    image_info->file_ptr = NULL; 
    image_info->file_size = 0; 
    image_info->p_val = 0; 
    image_info->unpack_status = IMAGE_NOT_UNPACKED; 
    image_info->image_dat.maxval = 0; 
    image_info->image_dat.image_arr = NULL; 
    image_info->image_dat.x = 0; 
    image_info->image_dat.y = 0; 
#endif 
}

/*
*   @brief we take in image data and we "age" it so that it looks older. 
*/
image_info_t age_image_data(image_info_t image_info){

}

/*
*   @brief Generates a black and white image copy of image_info_t placed in
*   @notes It's a new image, allowing us to have an old and new copy of the original image
*   @params image_info_t image_info(source information)
*   @returns image_info_t new file with greyscale information
*/
image_info_t greyscale_image_data(image_info_t image_info){
    image_info_t new_image_info = image_info; 
    // Create a new array and copy the contents over. 
    uint16_t *newimage = (uint16_t*)malloc(sizeof(uint16_t) * image_info.image_dat.x * image_info.image_dat.y * 3 + 100);
    memcpy(newimage, image_info.image_dat.image_arr, image_info.image_dat.x * image_info.image_dat.y * 3 + 100);
    // Change pointer for new image. 
    new_image_info.image_dat.image_arr = newimage; 
    
    // Struct that deals with pixel informatiojn
    set_get_pixel_t sp; 
    sp.image_data = &new_image_info.image_dat; 

    for(uint32_t y = 0; y < image_info.image_dat.y; y++){
        for(uint32_t x = 0; x < image_info.image_dat.x; x++){
            sp.x = x; 
            sp.y = y; 

            // Gets the original pixel data. 
            get_pixel(&sp);

            // Generate pixel average. 
            uint32_t ave = ((uint32_t)sp.r + (uint32_t)sp.g + (uint32_t)sp.b)/3; 
            sp.r = ave; 
            sp.g = ave; 
            sp.b = ave; 

            // Sets the pixel to the desired value
            set_pixel(sp);
        }
    }

    // Since we are just manipulating the passed in information we should be chilling. 
    return new_image_info; 
}

static inline image_info_t image_info_copy(image_info_t image_info){

}

#undef DEFINE_PPM