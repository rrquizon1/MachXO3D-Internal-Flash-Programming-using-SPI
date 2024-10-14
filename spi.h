// data.h
#ifndef SPI_H
#define SPI_H

#define PAGE_SIZE 256 // Define the page size as 256 bytes

int configure_output(struct gpiod_line *line, const char *consumer, int value);
int spi_init(int spi_speed,struct gpiod_line **cs,struct gpiod_line **prgrmn);
static int rbpi_ioctl(unsigned char *tx_buf, unsigned char *rx_buf, int len);
int rbpi_tx(unsigned char *buf, int bytes);
int rbpi_rx(unsigned char *buf, int bytes);
int rbpi_exit();
int sr_check(int done);
void chip_erase();
void read_data(unsigned char *read_data, size_t data_size, unsigned int start_address);
void write_enable();
void write_data(const unsigned char *data, size_t length, unsigned int start_address);

void device_id();
void activation_key();
void isc_enable_sram();
void isc_enable_flash();
void lsc_init_address();
void isc_disable();
void isc_erase_sram();
void isc_erase_flash(int CFG);
void isc_erase_feature();
void fast_program();
void program_done();
void verify_data(int CFG);
void program_data(int CFG);
void program_feature_row();

void program_internal_flash(int CFG);





#endif
