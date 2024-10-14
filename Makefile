



sspiem-rbpi: $(OBJ)
	gcc -o rhodz_spi_XO3D main.c spi_data.c spi_data_golden.c spi.c feature_row.c -lgpiod 

