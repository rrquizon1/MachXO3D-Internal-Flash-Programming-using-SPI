This is an SSPI Internal Flash programming using Raspberry pi 5.

This example uses lgpiod library so make sure to install it: sudo apt-get install gpiod libgpiod-dev.

MachXO3D have two sectors for internal flash programming namely CFG0 and CFG1. These sectors can be used for dual-boot instead of using an external flash for golden image. FPGA-TN-02069-1.7 ([MachXO3D Programming and Configuration User Guide](https://www.latticesemi.com/view_document?document_id=52591)) shows more information regarding MachXO3D dualboot and other features. This document also describes the instructions used by this programming example.

Source Files 

main.c- contains the programming procedure

spi.c- contains function for SPI transaction and some programming commands for MachXO3D devices.

spi_data.c- contains the primary image to be programmed at CFG0 

spi_data_golden.c-contains the golden image to be programmed at CFG1

feature_row.c-contains the feature row and feature bits information taken from .fea file.


Steps to run the project:

1. Compile by running the makefile

2. Run the executable generated ./rhodz_spi_XO3D

Some keypoints and other pointers for this example. 

1. This example, uses SSPI make sure that you SLAVE_SPI_PORT ENABLED in the global preferences in your project and also have dual boot enabled by setting the PRIMARY_BOOT and SECONDARY_BOOT settings below:
   ![image](https://github.com/user-attachments/assets/cba0de3a-5ae8-41d4-a729-4a8d83487774)


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
   
   
6. The usercode in the jed file can be found at the bottom:
![image](https://github.com/user-attachments/assets/ee66e61a-cee3-467d-bc73-d1772d335f15)



7. Take note that this example assumes that the device is not secured. If you are programming a secured device, you need to check FPGA-TN-02069 1.7 for the complete flow.


You can modify the primary and golden image by modifying the g_pucDataArray and g_pucDataArray_golden from spi_data.c and spi_data_golden.c respectively. Usercode of CFG0 and CFG1 can also be programmed using this example. Just modify the usercode0 for the intended usercode of CFG0 image and usercode1 for CFG1 image.
![image](https://github.com/user-attachments/assets/fa0e23b9-d9ff-41bf-a72e-758cd7ec684b)
![image](https://github.com/user-attachments/assets/6dbd8ad9-7dfc-4e59-9084-484407aac278)



Feature Row and Feature bits data can be modified at feature_row.c:
![image](https://github.com/user-attachments/assets/2fff93ea-87e9-4c8b-9560-d5e91919e1dd)

Take note that to program the feature row there should be 8 zeros padding at the beginning while feature bits are as is:
![image](https://github.com/user-attachments/assets/4a4c8873-04ef-40a5-9dcd-d7fcc62c2ea3)

The main.c file is modular, focusing solely on key operations such as SPI initialization, programming the internal flash, and configuring the feature row:

![image](https://github.com/user-attachments/assets/431a1f14-cb54-46e2-8571-7e2f0abf465e)




Sample terminal run:

![image](https://github.com/user-attachments/assets/dd01dc21-8db9-4496-98e3-4e694658642e)






