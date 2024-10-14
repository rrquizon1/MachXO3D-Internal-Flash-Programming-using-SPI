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
#include "spi_data_golden.h"
#include "spi.h"
#include "feature_row.h"
#define num_chunks (g_iDataSize+16-1)/16
#define num_chunks_golden (g_iDataSize_golden+16-1)/16


static uint32_t speed;
static int spi_fd = -1;
extern struct gpiod_line *cs;
extern struct gpiod_line *prgmn;
char a='A';
int configure_output(struct gpiod_line *line, const char *consumer, int value)
{
    if (gpiod_line_request_output(line, consumer, value) < 0)
    {
        perror("Request line as output failed");
        return -1;
    }

    return 0;
}


int spi_init(int spi_speed,struct gpiod_line **cs,struct gpiod_line **prgrmn){
	
	int ret = 0;
	uint8_t mode = SPI_MODE_0;
	uint8_t bits = 8;
	uint32_t reg;
	uint32_t shift;

	// setup spi via ioctl

	speed = spi_speed;

	spi_fd = open("/dev/spidev0.0", O_RDWR);
	if (spi_fd < 0) {
		fprintf(stderr, "Failed to open /dev/spidev0.0: %s\n", strerror(errno));
		return 0;
	}

	ret |= ioctl(spi_fd, SPI_IOC_WR_MODE, &mode);
	ret |= ioctl(spi_fd, SPI_IOC_RD_MODE, &mode);
	ret |= ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	ret |= ioctl(spi_fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
	ret |= ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	ret |= ioctl(spi_fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);

	if(ret) {
		fprintf(stderr, "Failed to setup SPI.\n");
	if (spi_fd > -1)
		close(spi_fd);
	spi_fd = -1;
		return 0;
	}	
	
	
	
	
	
	
    //GPIO CS and CRST initialize
    struct gpiod_chip *chip;

    
    
     // Replace with your GPIO pin number for CRST
    int offset_cs = 21; // Replace with your GPIO pin number for CS
    int offset_prgmn = 20; // Replace with your GPIO pin number for CS
    
    chip = gpiod_chip_open("/dev/gpiochip4"); // Replace 4 with the appropriate chip number

    if (!chip) {
        perror("Open chip failed");
        return 1;
    }


    *cs= gpiod_chip_get_line(chip, offset_cs);
    *prgrmn= gpiod_chip_get_line(chip, offset_prgmn);


    
    if (!*cs) {
        perror("Get line failed");
        gpiod_chip_close(chip);
        return 1;
    }
    
        
    if (!*prgrmn) {
        perror("Get line failed");
        gpiod_chip_close(chip);
        return 1;
    }



     configure_output(*cs, &a, 1);
     configure_output(*prgrmn, &a, 1);
 
    usleep(100);
 
    return 0;
    
}

// Function for ioctl call
static int rbpi_ioctl(unsigned char *tx_buf, unsigned char *rx_buf, int len)
{
	struct spi_ioc_transfer req;

	memset(&req, 0, sizeof(req));

	req.tx_buf = (uintptr_t) tx_buf;
	req.rx_buf = (uintptr_t) rx_buf;
	req.len = len;

	return ioctl(spi_fd, SPI_IOC_MESSAGE(1), &req) == -1;
}


// Function for data transmit
int rbpi_tx(unsigned char *buf, int bytes)
{




	if(rbpi_ioctl(buf, NULL, bytes)) {
		fprintf(stderr, "SPI ioctl write failed: %s\n", strerror(errno));
		return 0;
	} else
		return 1;
}


int rbpi_rx(unsigned char *buf, int bytes)
{




	if(rbpi_ioctl(NULL,buf, bytes)) {
		fprintf(stderr, "SPI ioctl write failed: %s\n", strerror(errno));
		return 0;
	} else
		return 1;
}





//Closes the SPI bus

int rbpi_exit(){
	
	close(spi_fd);
}












void device_id(){
    
	unsigned char write_buf[6] = { 0xE0 ,0x00,0x00,0x00,0x00,0x00};
	unsigned char read_buf[4];
    
	gpiod_line_set_value(cs, 1);
	usleep(1);
	gpiod_line_set_value(cs, 0);
	rbpi_tx(write_buf,4);
	rbpi_rx(read_buf,4);
	gpiod_line_set_value(cs, 1);
	
	printf("Device ID Read:");
	for (size_t i = 0; i < 4; i++) {
	
	    printf("0x%02X ", read_buf[i]); // Print each element in hexadecimal format
	}
	printf("\n");
}


int sr_check(int done){
    
	unsigned char write_buf[6] = { 0x3C ,0x00,0x00,0x00,0x00,0x00};
	unsigned char read_buf[8];
    
	gpiod_line_set_value(cs, 1);
	usleep(1);
	gpiod_line_set_value(cs, 0);
	rbpi_tx(write_buf,4);
	rbpi_rx(read_buf,4);
	gpiod_line_set_value(cs, 1);
	
	printf("Status Register Read:");
	for (size_t i = 0; i < 4; i++) {
	
	    printf("0x%02X ", read_buf[i]); // Print each element in hexadecimal format
	}
	printf("\n");
	
    //bytes[1] = 0b00101000;  // Set bits for illustration (bit 8 and bit 13)

    // Check bit 8 (second byte, 0th bit in index 1)
    int bit8 = (read_buf[2] & (1 << 0)) ? 1 : 0; //done bit

    // Check bit 13 (second byte, 5th bit in index 1) //fail flag
    int bit13 = (read_buf[2] & (1 << 5)) ? 1 : 0;
    
        // Check bit 13 (second byte, 5th bit in index 1) //fail flag
    int bit12 = (read_buf[2] & (1 << 4)) ? 1 : 0;


    if(bit13==0){
	printf("Fail flag not triggered\n");    
     }
    else{
	 printf("Fail flag triggered. Exitin\n");
	 exit(0);   
	}
     //if (done==0 || done==1){
     if(bit8==done){
	 printf("Done bit is %d. No errors\n",bit8);    
	     
	}
     else{
	printf("Done bit is %d. Error detected. Exiting....\n",bit8); 
	exit(0);    
	     
	}
	//}
     if(bit12==0){
	 printf("Busy flag not triggered\n");   
	     return 0;
	}
     else{
	printf("Busy flag triggered.\n");
	//exit(0);    
	 return 1;    
	}
	
	
}





void activation_key(){
    
	unsigned char write_buf[6] = { 0xA4 ,0xC6,0xF4,0x8A};
	
    
	gpiod_line_set_value(cs, 1);
	usleep(1);
	gpiod_line_set_value(cs, 0);
	rbpi_tx(write_buf,4);
	gpiod_line_set_value(cs, 1);
	
	printf("Activation Key sent Successfully");

		printf("\n");
}

void isc_enable_sram(){
    
	unsigned char write_buf[6] = { 0xC6 ,0x00,0x00,0x00};
	
    
	gpiod_line_set_value(cs, 1);
	usleep(1);
	gpiod_line_set_value(cs, 0);
	rbpi_tx(write_buf,4);
	gpiod_line_set_value(cs, 1);
	printf("Device Enters Programming Mode: SRAM");
	printf("\n");

}

void isc_enable_flash(){
    
	unsigned char write_buf[6] = { 0xC6 ,0x08,0x00,0x00};
	
    
	gpiod_line_set_value(cs, 1);
	usleep(1);
	gpiod_line_set_value(cs, 0);
	rbpi_tx(write_buf,4);
	gpiod_line_set_value(cs, 1);
	printf("Device Enters Programming Mode: Flash");
	printf("\n");

}



void lsc_init_address(int CFG){ // CFG0 CFG1 Feature Row
	unsigned char write_buf[4] = { 0x46,0x00,0x01,0x00};	
	if (CFG==0){
	 write_buf[2]= 0x01;
	}
	else if (CFG==1) {
	write_buf[2] =0x02;	
	}

	else if (CFG==3) {
	write_buf[2] =0x00;	
	write_buf[1] =0x04;	
	}
	
	else{
		printf("Not a valid sector");
		exit(0);
	}
    
	gpiod_line_set_value(cs, 1);
	usleep(1);
	gpiod_line_set_value(cs, 0);
	rbpi_tx(write_buf,4);
	gpiod_line_set_value(cs, 1);
	if (CFG==0){
	printf("Init Adress CFG0");}
	
	else if (CFG==1) {
		printf("Init Adress CFG1");
		
	}
	
	else if (CFG==2){
			printf("Init Address Feature Row");
	}
	
printf("\n");
}

void isc_disable(){
    
	unsigned char write_buf[6] = { 0x26 ,0x00,0x00,0x00};
	
    
	gpiod_line_set_value(cs, 1);
	usleep(1);
	gpiod_line_set_value(cs, 0);
	rbpi_tx(write_buf,4);
	gpiod_line_set_value(cs, 1);
	printf("Device exits Programming Mode");
	printf("\n");

}



void isc_erase_sram(){
    
	unsigned char write_buf[6] = { 0x0E ,0x00,0x00,0x00};
	
    
	gpiod_line_set_value(cs, 1);
	usleep(1);
	gpiod_line_set_value(cs, 0);
	rbpi_tx(write_buf,4);
	gpiod_line_set_value(cs, 1);
	printf("SRAM Erased");
	printf("\n");

}

void isc_erase_feature(){
    
	unsigned char write_buf[6] = { 0x0E ,0x04,0x00,0x00};
	
    
	gpiod_line_set_value(cs, 1);
	usleep(1);
	gpiod_line_set_value(cs, 0);
	rbpi_tx(write_buf,4);
	gpiod_line_set_value(cs, 1);
	printf("Feature Row Erased");
	printf("\n");

}

void isc_erase_flash(int CFG){
	
	
	unsigned char write_buf[6] = { 0x0E ,0x00,0x01,0x00};
	if (CFG==0){
	write_buf[2]=0x01;
	}
	else{
		
	write_buf[2]=0x02;	
	}
	
	gpiod_line_set_value(cs, 1);
	usleep(1);
	gpiod_line_set_value(cs, 0);
	rbpi_tx(write_buf,4);
	gpiod_line_set_value(cs, 1);
	printf("Erasing Flash CFG %d.....",CFG);
	printf("\n");

}

void program_done(){
    
	unsigned char write_buf[6] = { 0x5E ,0x00,0x00,0x00};
	
    
	gpiod_line_set_value(cs, 1);
	usleep(1);
	gpiod_line_set_value(cs, 0);
	rbpi_tx(write_buf,4);
	gpiod_line_set_value(cs, 1);
	printf("Done bit programmed");
	printf("\n");

}

void fast_program(){
	unsigned char lsc_bitstream_burst[4]={0x7A,0x00,0x00,0x00};
	printf("Sending bitstream using LSC_BITSTREAM_BURST\n");
	gpiod_line_set_value(cs, 1);
	usleep(1000);
	gpiod_line_set_value(cs, 0);

	rbpi_tx(lsc_bitstream_burst,4);  //Send 0x7A command
	rbpi_tx(g_pucDataArray,g_iDataSize);//Send bitstream
		
	
	
}

void verify_data(int CFG){
	unsigned char read_bitstream[g_iDataSize];	
	unsigned char lsc_read_incr[4]= {0x73,0x00,0x00,0x00};	
	printf("Verifying internal flash..\n");
	if (CFG==0){
	for(int k=0;k<num_chunks;k++){
		gpiod_line_set_value(cs, 1);
		usleep(1);
		gpiod_line_set_value(cs, 0);
		rbpi_tx(lsc_read_incr,4);
		rbpi_rx(&read_bitstream[k*16],16);
		gpiod_line_set_value(cs, 1);
		usleep(1000);
	}
	

	
	if (memcmp(g_pucDataArray, read_bitstream, g_iDataSize) == 0) {
        printf("Erase, Program, Verify Succesful!\n");
	} else {
        printf("Wrong data read out Erase, Program, Verify fail!\n");
	
    }}
    
    
    	else{
	for(int k=0;k<num_chunks_golden;k++){
		gpiod_line_set_value(cs, 1);
		usleep(1);
		gpiod_line_set_value(cs, 0);
		rbpi_tx(lsc_read_incr,4);
		rbpi_rx(&read_bitstream[k*16],16);
		gpiod_line_set_value(cs, 1);
		usleep(1000);
	}		
		if (memcmp(g_pucDataArray_golden, read_bitstream, g_iDataSize_golden) == 0) {
			printf("Erase, Program, Verify Succesful!\n");
			} else {
		printf("Wrong data read out Erase, Program, Verify fail!\n");
	
	
	
	}
	
	
	}

}


void program_data(int CFG){
   unsigned char lsc_prog_incr[4]= {0x70,0x00,0x00,0x00};
   printf("Programming internal flash..\n");
   if (CFG==0){
   for(int k=0;k<num_chunks;k++){
  	gpiod_line_set_value(cs, 1);
	usleep(1);
	gpiod_line_set_value(cs, 0);
	rbpi_tx(lsc_prog_incr,4);
	
	rbpi_tx(&g_pucDataArray[k*16],16);
	gpiod_line_set_value(cs, 1);
	usleep(1000);
  }}
  
  else{
   for(int k=0;k<num_chunks_golden;k++){
  	gpiod_line_set_value(cs, 1);
	usleep(1);
	gpiod_line_set_value(cs, 0);
	rbpi_tx(lsc_prog_incr,4);
	
	rbpi_tx(&g_pucDataArray_golden[k*16],16);
	gpiod_line_set_value(cs, 1);
	usleep(1000);
  }	  
	  
	  
	  
  }
	

	
	
}


void program_feature_row(){
   unsigned char lsc_prog_feature[4]= {0xE4,0x00,0x00,0x00};
   unsigned char lsc_read_feature[4]= {0xE7,0x00,0x00,0x00};
   unsigned char lsc_prog_feabits[4]= {0xF8,0x00,0x00,0x00};
   unsigned char lsc_read_feabits[4]= {0xFB,0x00,0x00,0x00};
   unsigned char feature_read[16];
   unsigned char feabits_read[4];
 printf("============================================\n");
 printf("Programming Feature Row \n");	
 printf("============================================\n");

     isc_enable_flash();
     usleep(1000);
    lsc_init_address(3);
    usleep(1000);
    isc_erase_feature();
    sleep(5);

   
  	gpiod_line_set_value(cs, 1);
	usleep(1);
	gpiod_line_set_value(cs, 0);
	rbpi_tx(lsc_prog_feature,4);
	rbpi_tx(feature_row,16);
	gpiod_line_set_value(cs, 1);
	sleep(5); //to optimize wait time use polling of busy bit
	
	gpiod_line_set_value(cs, 1);
	usleep(1);
	gpiod_line_set_value(cs, 0);
	rbpi_tx(lsc_read_feature,4);
	rbpi_rx(feature_read,16);
	gpiod_line_set_value(cs, 1);
	
	if (memcmp(feature_row, feature_read, 16) == 0) {
		printf("Feature Row Programming Successful!\n");
		} else {
		printf("Wrong feature row written!\n");
	
	exit(0);}
	
	gpiod_line_set_value(cs, 1);
	usleep(1);
	gpiod_line_set_value(cs, 0);
	rbpi_tx(lsc_prog_feabits,4);
	rbpi_tx(feature_bits,4);
	gpiod_line_set_value(cs, 1);
	sleep(5);
	
	gpiod_line_set_value(cs, 1);
	usleep(1);
	gpiod_line_set_value(cs, 0);
	rbpi_tx(lsc_read_feabits,4);
	rbpi_rx(feabits_read,4);
	gpiod_line_set_value(cs, 1);
    
	if (memcmp(feature_bits, feabits_read, 4) == 0) {
		printf("Feature Bits Programming Successful!\n");
		} else {
		printf("Wrong feature bits written!\n");
	
	exit(0);}	
	

	
	
}





void program_internal_flash(int CFG){
	
if(CFG==0 || CFG==1){
 printf("============================================\n");
 printf("Erase, Program, Verify of CFG %d \n", CFG);	
 printf("============================================\n");
 printf("\n");

}

else{
printf("Not Valid CFG sector \n");	
exit(0);
	
}

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
    
    //ISC_ENABLE FLASH
    isc_enable_flash();
    
    usleep(100);
    
      //Init CFG Address
    lsc_init_address(CFG);

    
    //Erase Flash
    int busy_flag=1;
    isc_erase_flash(CFG);
    
  
    
    while (busy_flag==1){//Polling Status Register for busy flag
    sleep(20);// long wait time to give time to erase
    busy_flag=sr_check(0);// Check if device is still busy
 
    }
    printf("Finished Erasing flash CFG %d.....\n",CFG);
    
    //Init CFG Address
    lsc_init_address(CFG);
    usleep(1000);
    
    
    //Program Internal Flash
    program_data(CFG);

    usleep(1000);

    //Init CFG Address
    lsc_init_address(CFG);
    
    //Verifying internal flash
    verify_data(CFG);


    //Init CFG0 Address
    
    lsc_init_address(CFG);
    
    program_done();

    //Status Register Check, DONE=1 and Fail=0
    sr_check(1);//Argument of 1 means DONE bit is expected to be 1
	
  
   printf("\n");
   sleep(1);
	
	
}
