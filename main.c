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
//#include "data.h"




struct gpiod_line *cs;
struct gpiod_line *prgrmn;

#define num_chunks (g_iDataSize+16-1)/16

int main() {
    
    
   
    spi_init(3000*1024,&cs,&prgrmn);
    
    usleep(1000);
    //Device ID read 	
    device_id();
    
    usleep(10);
    //ISC_ENABLE
    isc_enable_sram();
    usleep(10);
    
    //ISC_ERASE
    isc_erase_sram();
    usleep(10);
    
    isc_enable_flash();
    
    //Status Register Check, DONE=0 and Fail=0
    //sr_check(1);
    usleep(100);
    
    
    //Init CFG0 Address
    lsc_init_address(0);
    
    //Erase Flash
    int busy_flag=1;
    isc_erase_flash();
    
    
    while (busy_flag==1){//Polling Status Register for busy flag
    sleep(20);// long wait time to give time to erase
    busy_flag=sr_check(0);// Check if device is still busy
 
    }
    printf("Finished Erasing flash.....\n");
    
    //Init CFG0 Address
    lsc_init_address(0);
    usleep(1000);
    
    
    //Program Internal Flash
    program_data();

    usleep(1000);

    //Init CFG0 Address
    lsc_init_address(0);
    
    //Verifying internal flash
    verify_data();


    //Init CFG0 Address
    
    lsc_init_address(0);
    
    program_done();

    //Status Register Check, DONE=1 and Fail=0
    sr_check(1);


	
    //Exits programming mode and enters usermode
    isc_disable();

    rbpi_exit();
	
	
}
