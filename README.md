This is an SSPI Internal Flash programming using Raspberry pi 5.

This example uses lgpiod library so make sure to install it: sudo apt-get install gpiod libgpiod-dev.

MachXO3D have two sectors for internal flash programming namely CFG0 and CFG1. These sectors can be used for dual-boot instead of using an external flash for golden image. FPGA-TN-02069-1.7 ([MachXO3D Programming and Configuration User Guide](https://www.latticesemi.com/view_document?document_id=52591)) shows more information regarding MachXO3D dualboot and other features. This document also describes the instructions used by this programming example.

Source Files 

main.c- contains the programming procedure

spi.c- contains function for SPI transaction and some programming commands for MachXO3D devices.

spi_data.c- contains the primary image to be programmed at CFG0 

spi_data_golden.c-contains the golden image to be programmed at CFG1

feature_row.c-contains the feature row information taken from .fea file.


Steps to run the project:

1. Compile by running the makefile

2. Run the executable generated ./rhodz_spi_XO3D

Some keypoints and other pointers for this example. 

1. This example, uses SSPI make sure that you SLAVE_SPI_PORT ENABLED in the global preferences in your project:
   ![image](https://github.com/user-attachments/assets/d19f59ff-a773-4745-ac3b-863802c9beb1)

   This setting is stored in the fea files, so once you programmed the fea file and have SLAVE_SPI_PORT disabled, communication with SPI will not work any more.

2.  Jedec file contains binary bitstream data. It is helpful to convert it to hex to be able to use hexadecimal array for programming. Lattice has a Deployment Tool which can be used to convert jedec into hex file:
   ![image](https://github.com/user-attachments/assets/d6d42e34-0e62-4db6-812e-4f32163a7bed)

Sample hex output is below:

![image](https://github.com/user-attachments/assets/b8b0a46a-594c-480d-b277-9d0650d59acb)

You could use a simple script to turn this into a hexadecimal array:
![image](https://github.com/user-attachments/assets/dd8faefe-c209-4f83-ac28-e14e0e8a349f)

3. Feature row and Feature bits information is in a .fea file. It is also in binary format, you can easily convert it as it as significantly fewer bits.  See below for example fea file:
   ![image](https://github.com/user-attachments/assets/3533e4f0-21ad-4cb1-8162-35d592a77862)

   Take note that the first row is Feature Row and the second row is Feature bits. In this example, separate arrays are used to store them as they will be using different commands to be programmed.

4. This example uses a fixed wait command after programming of a page. For optimal programming, you could used a status register read monitoring busy bit. For simplicity of implementation a fixed delay is used. The same could be done for erase command implementations.

5. This example assumes CFG0 will be the primary image while CFG1 is the golden image. Consult FPGA-TN-02069-1.7 for more modifications that could be done.


You can modify the primary and golden image by modifying the g_pucDataArray and g_pucDataArray_golden from spi_data.c and spi_data_golden.c respectively. 
![image](https://github.com/user-attachments/assets/0b97fc76-90ae-481d-8d7f-209618321ae2)
![image](https://github.com/user-attachments/assets/5db25c2b-3906-4d1a-92e6-9a6e1f9a9a37)











