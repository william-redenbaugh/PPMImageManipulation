#include "ppm_management.h"

image_data_t process_image(FILE *fp, image_params_t image_data){
    printf("Taking in image input\n");
}

unpack_image_info_t unpack_image(char *file_path, size_t file_path_len){
    unpack_image_info_t img_info; 

    FILE *fp = fopen(file_path, "r");
    
    // If we couldn't find the deseired file, we
    if(fp == NULL){
        printf("Couldn't find file\n");
        img_info.unpack_status = IMAGE_UNPACK_FAIL_FILE_DNE;
        return img_info;    
    }

    printf("Image unpacked successfully! :)\n");
    
    {
        // Buffer that we will use to unpack our first bits of information
        char buff[32];

        // Sometimes there are spaces that don't contain important information at the start 
        memset(buff, NULL, sizeof(buff)); 
        while(!strlen(buff))
            fscanf(fp, "%s", buff);

        // Parse out the p file type value
        img_info.p_val = (buff[1] - 48);
        

    }

    img_info.unpack_status = IMAGE_UNPACK_SUCCESS; 
    img_info.file_ptr = fp; 
    return img_info; 
}