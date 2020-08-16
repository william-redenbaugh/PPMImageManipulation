#include "ppm_management.h"

/*
*   @brief Allows us to enable a couple of debugging features. 
*   @params Disable this if you want better preformance at the cost of more error checking
*/
#define DEBUG_PPM

// Internal Struct declarations at the top!

/* Red green and blue struct that deals with surrounding pixels.    
    The data is stored as shown: 
        [0][1][2]
        [3][4][5]
        [6][7][8]
    This is done for semantic purposes. 
    As you can see below, it's just a struct containing three color values 
    This is so we can contain all that information

*/
typedef struct{
    int r; 
    int g; 
    int b; 
}surrounding_pixels_t;

typedef struct{
    double alpha; 
    double beta; 
    double gamma; 
}hue_calc_t; 

/*
*   @brief Struct that allows us to deal with pixel setting stuff
*   @notes can be coppied or passed around depending on the use case scenario
*/
typedef struct{
    // Pointer to the primary image data array and text information
    image_data_t *image_data; 

    // RGB value that we want to progrma
    uint16_t r; 
    uint16_t g; 
    uint16_t b; 
    
    // X and Y position of the pixel we want to set. 
    uint16_t x; 
    uint16_t y; 

}set_get_pixel_t; 

// Function declarations here: 
image_info_t unpack_image(char *file_path, size_t file_path_len); 
void unpack_p_resolution(image_info_t *img_info); 
static inline void convert_image_array(image_info_t *img_info); 
static inline image_info_t get_file_path(char *file_path); 
extern pixel_set_get_return_t set_pixel(set_get_pixel_t sp); 
extern pixel_set_get_return_t get_pixel(set_get_pixel_t *sp); 
extern file_write_status_t write_file(char file[], size_t file_size, image_info_t image_info); 
inline void write_image_metadata(image_info_t *image_info, FILE *fp); 
inline void write_image_data(image_info_t *image_info, FILE *fp); 
image_info_t age_image_data(image_info_t image_info); 
image_info_t greyscale_image_data(image_info_t image_info); 
inline set_get_pixel_t greyscale_average(set_get_pixel_t sp); 
image_info_t sharpen_image_data(image_info_t image_info); 
inline surrounding_pixels_t get_pixel_offset_data(set_get_pixel_t sp, int x, int y); 
inline void gather_surrounding_pixels(set_get_pixel_t sp, surrounding_pixels_t surrounding_pixel[9], uint32_t x, uint32_t y); 
inline void cal_average_sharpen (set_get_pixel_t *sp, surrounding_pixels_t surrounding_pixel[9]); 
void free_image_data_mem(image_info_t *image_info); 
image_info_t change_hue(image_info_t image_info, double hue); 
inline hue_calc_t calculate_hue(double hue);
inline set_get_pixel_t calc_pixel_hue(set_get_pixel_t sp, hue_calc_t hue_calc); 

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

    return PIXEL_SET_SUCCESS; 
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

    return PIXEL_SET_SUCCESS; 
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
*   @brief we take in image data and we "age" it so that it looks older.
*   @params image_info_t previous image data
*   @returns new image struct.  
*/
image_info_t age_image_data(image_info_t image_info){
    
    image_info_t new_image_info = image_info; 
    // Create a new array and copy the contents over. 
    uint16_t *newimage = (uint16_t*)malloc(sizeof(uint16_t) * image_info.image_dat.x * image_info.image_dat.y * 3 + 100);
    // Change pointer for new image. 
    new_image_info.image_dat.image_arr = newimage; 
    
    // Struct that deals with pixel informatiojn
    set_get_pixel_t sp; 

    for(uint32_t y = 0; y < image_info.image_dat.y; y++){
        for(uint32_t x = 0; x < image_info.image_dat.x; x++){
            // Check current image with color data. 
            sp.image_data = &image_info.image_dat; 
            sp.x = x; 
            sp.y = y; 
            get_pixel(&sp); 

            sp.b = (sp.r + sp.g + sp.b)/ 5; 
            sp.r = (uint16_t)((double)sp.b * 1.6);
            sp.g = (uint16_t)((double)sp.b * 1.6); 

            // Set our source destination as our last image data. 
            sp.image_data = &new_image_info.image_dat; 
            // Sets the pixel to the desired value for our source destination
            set_pixel(sp);
        }
    }

    // Send em the new image.  
    return new_image_info; 
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
    // Change pointer for new image. 
    new_image_info.image_dat.image_arr = newimage; 
    
    // Struct that deals with pixel informatiojn
    set_get_pixel_t sp; 

    for(uint32_t y = 0; y < image_info.image_dat.y; y++){
        for(uint32_t x = 0; x < image_info.image_dat.x; x++){
            // Check current image with color data. 
            sp.image_data = &image_info.image_dat; 
            sp.x = x; 
            sp.y = y; 

            // Changes pixel to greyscale. 
            sp = greyscale_average(sp);
            // Set our source destination as our last image data. 
            sp.image_data = &new_image_info.image_dat; 
            // Sets the pixel to the desired value for our source destination
            set_pixel(sp);
        }
    }

    // Send em the new image.  
    return new_image_info; 
}

/*
*   @brief returns a set get pixel greyscale average on a per pixel basis. 
*   @params set_get_pixel_t sp (pixel in question)
*/
inline set_get_pixel_t greyscale_average(set_get_pixel_t sp){
    // Gets the original pixel data. 
    get_pixel(&sp);

    // Generate pixel average. 
    uint32_t ave = ((uint32_t)sp.r + (uint32_t)sp.g + (uint32_t)sp.b)/3; 
    sp.r = ave; 
    sp.g = ave; 
    sp.b = ave; 
    return sp;
}

/*
*   @brief Takes in an image information struct, and returns a sharpened copy of that image
*   @notes Previous image info is retained so we have a copy, feel free to delete that copy later on if you see fit. 
*   @params image_info_t image_info(the image that we will process)
*   @returns copy of image with sharpening applied.  
*/
image_info_t sharpen_image_data(image_info_t image_info){
    image_info_t new_image_info = image_info; 
    // Create a new array and copy the contents over. 
    uint16_t *newimage = (uint16_t*)malloc(sizeof(uint16_t) * image_info.image_dat.x * image_info.image_dat.y * 3 + 100);
    // Change pointer for new image. 
    new_image_info.image_dat.image_arr = newimage; 
    
    // Struct that deals with pixel informatiojn
    set_get_pixel_t sp; 

    // Surrounding pixel struct that has everything
    surrounding_pixels_t surrounding_pixel[9]; 

    for(uint32_t y = 1; y < image_info.image_dat.y-1; y++){
        for(uint32_t x = 1; x < image_info.image_dat.x-1; x++){
            // Check current image with color data. 
            sp.image_data = &image_info.image_dat; 
            sp.x = x;
            sp.y = y;

            // We get all the pixel data from the surrounding pixels
            gather_surrounding_pixels(sp, surrounding_pixel, x, y);
            // When we get all the surrounding pixels, we pass em in along with the 
            // pixel modifier.         
            cal_average_sharpen(&sp, surrounding_pixel);

            // Set our source destination as our last image data. 
            sp.image_data = &new_image_info.image_dat; 
            // Sets the pixel to the desired value for our source destination
            set_pixel(sp);
        }
    }

    // Send em the new image.  
    return new_image_info; 
}

/*
*   @brief Gets pixel data with an offset. Basically let's say we have a set_get_pixel, with preloaded pixel values inside
*   This way we can get the surrounding pixel data. 
*/
inline surrounding_pixels_t get_pixel_offset_data(set_get_pixel_t sp, int x, int y){
    surrounding_pixels_t surround_pixel; 
    sp.x = sp.x + x; 
    sp.y = sp.y + y;

    // Get pixel data. 
    get_pixel(&sp);

    // Get pixel information
    surround_pixel.r = sp.r;
    surround_pixel.g = sp.g; 
    surround_pixel.b = sp.b; 
    return surround_pixel; 
}

/*
*   @brief Gets the surrounding pixel information from a particular pixel
*   @params set_get_pixel sp the pixel in question(must be setup properly beforehand)
*   @params surrounding_pixels_t surrounding_pixels (array of 9 rgb pixel structs that will hold the surrouding pixel information)
*/
inline void gather_surrounding_pixels(set_get_pixel_t sp, surrounding_pixels_t surrounding_pixel[9], uint32_t x, uint32_t y){
    // Getting top left pixel 
    surrounding_pixel[0] = get_pixel_offset_data(sp, -1, -1);
    // Getting top middle pixel 
    surrounding_pixel[1] = get_pixel_offset_data(sp, 0, -1);
    // Getting top right pixel 
    surrounding_pixel[2] = get_pixel_offset_data(sp, 1, -1);
    // Getting the left pixel. 
    surrounding_pixel[3] = get_pixel_offset_data(sp, -1, 0);
    // Getting the center pixel(aka the primary pixel in question)
    surrounding_pixel[4] = get_pixel_offset_data(sp, 0, 0);
    // Get the pixel to the right
    surrounding_pixel[5] = get_pixel_offset_data(sp, 1, 0);
    // Get the bottom left pixel
    surrounding_pixel[6] = get_pixel_offset_data(sp, -1, 1);
    // Get the bottom pixel. 
    surrounding_pixel[7] = get_pixel_offset_data(sp, 0, 1); 
    // Get the bottom right pixel
    surrounding_pixel[8] = get_pixel_offset_data(sp, 1, 1);
}

/*
*   @brief Pass in a specific pixel, along with an array of the surrounding pixels, and calclates the average sharpening for that image
*   @params set_get_pixel_t (pointer to the pixel in question)
*   @params surrounding_pixels_t the 9 surrounding pixels of the primary pixel, so we have all the required pixel information
*/
inline void cal_average_sharpen (set_get_pixel_t *sp, surrounding_pixels_t surrounding_pixel[9]){
    // Formular requires that we multiply pixel's original value by 9.
    int32_t red = 9 * surrounding_pixel[4].r; 
    int32_t green = 9* surrounding_pixel[4].g; 
    int32_t blue = 9 * surrounding_pixel[4].b; 

    // Complete the calculation via subtracting the pixels. 
    for(uint8_t i = 0; i < 9; i++){
        if(i != 4){
            red = red - surrounding_pixel[i].r; 
            green = green - surrounding_pixel[i].g; 
            blue = blue - surrounding_pixel[i].b; 
        }
    } 

    // Deals with out of bounds numbers. 
    if(red > 255)
        red = 255; 
    if(red < 0)
        red = 0; 
    if(green > 255)
        green = 255; 
    if(green < 0)
        green = 0; 
    if(blue > 255)
        blue = 255; 
    if(blue < 0)
        blue = 0; 

    // Newly calculated information get's saved in the pixel pointer. 
    sp->r = red; 
    sp->g = green; 
    sp->b = blue; 
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
*   @brief Takes in an image information struct, and returns a hue modified copy of that image
*   @notes Previous image info is retained so we have a copy, feel free to delete that copy later on if you see fit. 
*   @params image_info_t image_info(the image that we will process)
*   @returns copy of image with sharpening applied.  
*/
// Used for functional purposes. 
image_info_t change_hue(image_info_t image_info, double hue){
    image_info_t new_image_info = image_info; 
    // Create a new array and copy the contents over. 
    uint16_t *newimage = (uint16_t*)malloc(sizeof(uint16_t) * image_info.image_dat.x * image_info.image_dat.y * 3 + 100);
    // Change pointer for new image. 
    new_image_info.image_dat.image_arr = newimage; 
    
    // Struct that helps with hue calculations
    hue_calc_t hue_calc = calculate_hue(hue);

    // Struct that deals with pixel information
    set_get_pixel_t sp;

    for(uint32_t y = 0; y < image_info.image_dat.y; y++){
        for(uint32_t x = 0; x < image_info.image_dat.x; x++){
            // Check current image with color data. 
            sp.image_data = &image_info.image_dat; 

            // Set x and y corridinates. 
            sp.x = x; 
            sp.y = y; 

            // Gets the original pixel data. 
            get_pixel(&sp);

            // Calculate new pixel using the hue calculations. 
            sp = calc_pixel_hue(sp, hue_calc);

            // Set our source destination as our last image data. 
            sp.image_data = &new_image_info.image_dat; 

            // Sets the pixel to the desired value for our source destination
            set_pixel(sp);
        }
    }

    // Send em the new image.  
    return new_image_info;
}

/*
*   @brief Pass in a double floating point hue value. 
*   @notes Calculation is return in components via a struct
*   @params double hue value 
*   @returns hue_calc_t struct with the math resultants 
*/
inline hue_calc_t calculate_hue(double hue){
    // If hue is greater than 360, then let's just allow ourselves to wrap around. 
    if(hue > 360){
        hue = (double)((uint16_t)(hue) % 360); 
    }
    // If it's a negative value, then we do the inverted version of the value above us 
    if(hue < 0){
        hue = (double)((uint16_t)(hue) % 360) * -1; 
    }
    hue_calc_t hue_calc; 
    #define PI 3.14159265 
    hue_calc.alpha = (2 * cos(hue * PI / 180) + 1)/3;
    hue_calc.beta = ((1 - cos(hue * PI / 180))/3) - (sin(hue * PI / 180))/sqrt(3.0); 
    hue_calc.gamma = ((1 - cos(hue * PI / 180))/3) + (sin(hue * PI / 180))/sqrt(3.0); 
    #undef PI

    return hue_calc; 
}

/*
*   @brief Takes in a pixel and some hue calculations and returns a new pixel with the hue calculations
*   @params set_get_pixel_t sp (pixel in question)
*   @params hue_calc_t hue_calc the hue calculations that hold everything
*/
inline set_get_pixel_t calc_pixel_hue(set_get_pixel_t sp, hue_calc_t hue_calc){
    // Buffers for use later. 
    double red = (double)sp.r; 
    double green = (double)sp.g; 
    double blue = (double)sp.b; 

    // Offset color hue
    int r = (double)(red * hue_calc.alpha + green * hue_calc.beta + blue * hue_calc.gamma); 
    int g = (double)(red * hue_calc.gamma + green * hue_calc.alpha + blue * hue_calc.beta); 
    int b = (double)(red * hue_calc.beta + green * hue_calc.gamma + blue * hue_calc.alpha); 

    // Clip out values
    if(r > 255)
        r = 255;
    if(r < 0)
        r = 0;  
    if(g > 255)
        g = 255;
    if (g < 0)
        g = 0; 
    if(b > 255)
        g = 255; 
    if(b < 0)
        b = 0; 

    // Set the pixel. 
    sp.r = r; 
    sp.g = g; 
    sp.b = b; 
    
    return sp; 
}
#undef DEFINE_PPM