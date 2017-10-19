#include <iostream>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include "lcd.h"
#include "logger.h"

Lcd::Lcd(): Connector("Unknown", "/dev/null")
{
}

Lcd::Lcd(std::string connector, std::string device): Connector(connector, device)
{
}

Lcd::~Lcd()
{
}

bool Lcd::Test()
{
    Logger::GetLogger()->Log("Running LCD Test");
    
    int fbfd 				= 0;
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    long int screensize 	= 0;
    char *fbp 				= 0;
    uint32_t x              = 0;
    uint32_t y			    = 0;
    uint32_t j			    = 0;
    long int location 		= 0;
    std::size_t ret		    = 0;

    FILE *image;
    BITMAPFILEHEADER bfh;
    BITMAPINFOHEADER bih;

    memset(&bfh,0,sizeof(BITMAPFILEHEADER));
    memset(&bih,0,sizeof(BITMAPINFOHEADER));
    
    /* Open the file for reading and writing */
    fbfd = open("/dev/fb0", O_RDWR);
    if (fbfd == -1) {
        Logger::GetLogger()->Log("LCD failed to open /dev/fb0");
        return true;
    }

    /* Get fixed screen information */
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo) == -1) {
        Logger::GetLogger()->Log("LCD failed to get fixed screen info");
        return true;
    }

    /* Get variable screen information */
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
        Logger::GetLogger()->Log("LCD failed to get variable screen info");
        return true;
    }

    /* Figure out the size of the screen in bytes */
    screensize = ((vinfo.xres * vinfo.yres * vinfo.bits_per_pixel) / 8);
    
    Logger::GetLogger()->Log("LCD /dev/fb0 size " + std::to_string(screensize));

    /* Map the device to memory */
    fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    if ((int)fbp == -1) {
        Logger::GetLogger()->Log("LCD mmap failed");
        return true;
    }
    
    if(!(image = fopen("/usr/share/mfg-test/fruit_girl.bmp", "rb+")))
    {
        Logger::GetLogger()->Log("LCD error opening image file");
        munmap(fbp, screensize);
        close(fbfd);
        return true;
    }
    
    fread(&bfh.magic,2,1,image);
    
    fseek(image,2,SEEK_SET);
    fread(&bfh.filesz,4,1,image);

    fseek(image,10,SEEK_SET);
    fread(&bfh.offset,4,1,image);

    fseek(image,14,SEEK_SET);
    fread(&bih.header_sz,4,1,image);

    fseek(image,18,SEEK_SET);
    fread(&bih.width,4,1,image);

    fseek(image,22,SEEK_SET);
    fread(&bih.height,4,1,image);

    fseek(image,26,SEEK_SET);
    fread(&bih.nplanes,2,1,image);

    fseek(image,28,SEEK_SET);
    fread(&bih.depth,2,1,image);

    fseek(image,30,SEEK_SET);
    fread(&bih.compress_type,4,1,image);

    //TODO: get this in the log
    //std::cout << "***********************************\n" << std::endl;
    //std::cout << "Type:\t\t\t" << bfh.magic[0] << bfh.magic[1] << std::endl;
    //std::cout << "File Size:\t\t" << bfh.filesz << std::endl;
    //std::cout << "Data Offset:\t\t" << bfh.offset << std::endl;
    //std::cout << "Header Size:\t\t" << bih.header_sz << std::endl;
    //std::cout << "Height:\t\t\t" << bih.height << std::endl;
    //std::cout << "Width:\t\t\t" << bih.width << std::endl;
    //std::cout << "Number of planes:\t" << bih.nplanes << std::endl;
    //std::cout << "Bit Count:\t\t" << bih.depth << std::endl;
    //std::cout << "Compression:\t\t" << bih.compress_type << std::endl;
    //std::cout << "***********************************\n" << std::endl;
    
    int padding = 0;
    int scanlinebytes = bih.width * 3;
    while ((scanlinebytes + padding ) % 4 != 0) {
        padding++;
    }
    int psw = scanlinebytes + padding;    
    
    fseek(image,bfh.offset,SEEK_SET);
    uint8_t buffer[bfh.filesz - bfh.offset];
    ret = fread(&buffer, 1, sizeof(buffer), image);
    if (ret != sizeof(buffer)) {
        Logger::GetLogger()->Log("LCD image read mismatch");
	}
        
    uint8_t newbuf[bih.width * bih.height * 3];
    long int bufpos = 0;
    long int newpos = 0;
    for (y = 0; y < bih.height; y++) {
        for (x = 0; x < 3 * bih.width; x += 3) {
            newpos = y * 3 * bih.width + x;     
			bufpos = ( bih.height - y - 1 ) * psw + x;

			newbuf[newpos] = buffer[bufpos + 2];       
			newbuf[newpos + 1] = buffer[bufpos+1]; 
			newbuf[newpos + 2] = buffer[bufpos];  
        }
    }

    location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +
        (y+vinfo.yoffset) * finfo.line_length;
            
    j=0;
    // Figure out where in memory to put the pixel
    for (y = 0; y < bih.height; y++) {
        for (x = 0; x < bih.width; x++) {

            location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +
                       (y+vinfo.yoffset) * finfo.line_length;
            
            *(fbp + location) = newbuf[j + 2]; 
            *(fbp + location + 1) = newbuf[j + 1];
            *(fbp + location + 2) = newbuf[j];
            *(fbp + location + 3) = 0;      // No transparency
            j += 3; //increment pixel pointer.
        }
    }
    
    fclose(image);
    munmap(fbp, screensize);
    close(fbfd);

    return false;
}

