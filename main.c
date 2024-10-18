#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <gpiod.h>
#include "spi_data.h"
#include "spi.h"
#include "feature_row.h"
//#include "data.h"




struct gpiod_line *cs;
struct gpiod_line *prgrmn;

#define num_chunks (g_iDataSize+16-1)/16

int main() {
    
    spi_init(15000*1024,&cs,&prgrmn);// initializes SPI bus and GPIO signals
    program_internal_flash(0);// Programs CFG0
    program_internal_flash(1);//Programs CFG1
    program_feature_row(); //Programs Feature Row
    isc_disable();// Exits Programming Mode
   
   

    
    
   

	
	
}
