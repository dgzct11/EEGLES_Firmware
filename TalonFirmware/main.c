#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "pico/multicore.h"
#include "hardware/i2c.h"
#include "hardware/spi.h"
#include "hardware/pio.h"
#include "hardware/uart.h"
#include "sd_card.h"
#include "ff.h"
#include <stdint.h>








/*___________________________________________________________________________________________________________________
MAX17048
*/
#define MAX17048_ADDRESS 0x36

//Pin Defitions
#define BS_DIO 22
#define BS_SCL 23
#define BS_ALRT 24


//ALERT Codes
#define LOW_CHARGE_ALRT 4
#define CHARGE_CHANGE_ALRT 3
#define RESET_ALRT 2
#define OVERVOLTAGE_ALRT 0
#define UNDERVOLTAGE_ALRT 1

void MAX17048_alert_handler(){
    
    uint8_t status_register[] = { 0x1A };
    uint8_t status_data[2];
    i2c_write_blocking(i2c1, MAX17048_ADDRESS, status_register, 1, true);
    i2c_read_blocking(i2c1, MAX17048_ADDRESS, status_data, 2, false);
    uint8_t data = status_data[0];
    bool result[] = {
        (data >> 1)%2,
        (data >> 1)%2,
        (data >> 1)%2,
        (data >> 1)%2,
        (data >> 1)%2
    };
    //TODO make this show something 
    
}
void MAX17048_reset() {
    uint8_t reset_cmd[] = {0xFE, 0x54,0x00};
    i2c_write_blocking(i2c1, MAX17048_ADDRESS, reset_cmd, 3, false);
}
void MAX17048_init() {
    // Initialize I2C
    i2c_init(i2c1, 100 * 1000);
    
   //set the function of the T2c pins
    gpio_set_function(BS_DIO, GPIO_FUNC_I2C);
    gpio_set_function(BS_SCL, GPIO_FUNC_I2C);
    
    //pull them up initially
    gpio_pull_up(BS_DIO);
    gpio_pull_up(BS_SCL);

    //initialize the Alert Pin
    gpio_init(BS_ALRT);
    gpio_set_dir(BS_ALRT, GPIO_IN);

    // Enable falling edge interrupts on GPIO pin 14
    gpio_set_irq_enabled(BS_ALRT, GPIO_IRQ_EDGE_FALL, true);

    // Register the interrupt handler function
    irq_set_exclusive_handler(GPIO_IRQ_EDGE_FALL, MAX17048_alert_handler);

    // Enable interrupts for the GPIO pin
    irq_set_enabled(GPIO_IRQ_EDGE_FALL, true);
}

float MAX17048_read_voltage() {
    uint8_t voltage_register[] = { 0x02 };
    uint8_t voltage_data[2];
    i2c_write_blocking(i2c1, MAX17048_ADDRESS, voltage_register, 1, true);
    i2c_read_blocking(i2c1, MAX17048_ADDRESS, voltage_data, 2, false);

    uint16_t voltage_raw = (voltage_data[0] << 8) | voltage_data[1];
    return ((float)voltage_raw * 1.25) / 1000.0;
}

float MAX17048_read_charge() {
    uint8_t soc_register[] = { 0x04 };
    uint8_t soc_data[2];
    i2c_write_blocking(i2c1, MAX17048_ADDRESS, soc_register, 1, true);
    i2c_read_blocking(i2c1, MAX17048_ADDRESS, soc_data, 2, false);

    uint16_t soc_raw = (soc_data[0] << 8) | soc_data[1];
    return (float)soc_raw / 256.0;
}

/*___________________________________________________________________________________________________________________
ADS1299
*/

//define ADS1299 pins
#define ADS1299_GPIO_RESET 6
#define ADS1299_GPIO_CS 4
#define ADS1299_GPIO_START 5
#define ADS1299_GPIO_DRDY 1

#define ADS1299_GPIO_MISO 0
#define ADS1299_GPIO_SCLK 2
#define ADS1299_GPIO_MOSI 7

// Define ADS1299 register addresses
#define ADS1299_CHANNELS 8

#define ADS1299_REG_ID         0x00
#define ADS1299_REG_CONFIG1    0x01
#define ADS1299_REG_CONFIG2    0x02
#define ADS1299_REG_CONFIG3    0x03
#define ADS1299_REG_LOFF       0x04
#define ADS1299_REG_CH1SET    0x05
#define ADS1299_REG_CH2SET    0x06
#define ADS1299_REG_CH3SET    0x07
#define ADS1299_REG_CH4SET    0x08
#define ADS1299_REG_CH5SET    0x09
#define ADS1299_REG_CH6SET    0x0a
#define ADS1299_REG_CH7SET    0x0b
#define ADS1299_REG_CH8SET    0x0c
#define ADS1299_REG_BIAS_SENSP    0x09
#define ADS1299_REG_BIAS_SENSN    0x0a
#define ADS1299_REG_LOFF_SENSP    0x0b
#define ADS1299_REG_LOFF_SENSN    0x0c
#define ADS1299_REG_LOFF_FLIP    0x0c
#define ADS1299_REG_LOFF_STATP    0x0c
#define ADS1299_REG_LOFF_STATN    0x0c

#define ADS1299_REG_MISC1         0x15



// Define ADS1299 command codes
#define ADS1299_CMD_WAKEUP     0x02
#define ADS1299_CMD_STANDBY    0x04
#define ADS1299_CMD_RESET      0x06
#define ADS1299_CMD_START      0x08
#define ADS1299_CMD_STOP       0x0A

#define ADS1299_CMD_RDATAC     0x10
#define ADS1299_CMD_SDATAC     0x11
#define ADS1299_CMD_RDATA      0x12

#define READ_BIT 0x20
#define WRITE_BIT 0x40

// Define other command codes...
static inline void ADS1299_cs_select() {
    gpio_put(4, 0);  // Active low
}

static inline void ADS1299_cs_deselect() {
    gpio_put(4, 1);
}

// Initialize the SPI interface
void ADS1299_init_spi() {
    spi_init(spi0, 4000000);  // Set SPI clock to 4 MHz
    gpio_set_function(ADS1299_GPIO_MISO, GPIO_FUNC_SPI);
    gpio_set_function(ADS1299_GPIO_SCLK, GPIO_FUNC_SPI);
    gpio_set_function(ADS1299_GPIO_MOSI, GPIO_FUNC_SPI);
    

    gpio_init(ADS1299_GPIO_CS);
    gpio_set_dir(ADS1299_GPIO_CS,GPIO_OUT);
    gpio_put(ADS1299_GPIO_CS, 1);

   
    spi_set_format( spi0, 8, 0, 1, SPI_MSB_FIRST);
}




void ADS1299_send_command(uint8_t cmd) {
    uint8_t tx_data[1] = { cmd };
    uint8_t rx_data[1];
    spi_write_read_blocking(spi0, tx_data, rx_data, 1);
}



uint8_t ADS1299_read_register(uint8_t reg_addr) {
    reg_addr |= READ_BIT;
    uint8_t tx_data[3] = { reg_addr, 0x00, 0x00 };
    uint8_t rx_data[3];
    
    spi_write_read_blocking(spi0, tx_data, rx_data, 3);
   
    return rx_data[2];
}

void ADS1299_read_consecutive_registers( uint8_t reg_addr, uint8_t len, uint8_t *read_buf ){
    reg_addr |= READ_BIT;
    uint8_t tx_data[len+2];
    tx_data[0] = reg_addr;
    tx_data[1] = len-1;

    spi_write_read_blocking(spi0, tx_data, read_buf, len+2);
}

void ADS1299_write_consecutive_registers( uint8_t reg_addr, uint8_t *data){
    reg_addr |= WRITE_BIT;
    uint8_t len = sizeof(data);
    uint8_t tx_data[len+2];
    tx_data[0]= reg_addr;
    tx_data[1] = len;
    uint8_t rx_data[len+2];
    spi_write_read_blocking(spi0, tx_data, rx_data, len+2); 
}
// Write a register value to the ADS1299
void ADS1299_write_register(uint8_t reg_addr, uint8_t reg_val) {
    reg_addr |= WRITE_BIT;
    uint8_t tx_data[3] = { reg_addr,0x00,reg_val };
    uint8_t rx_data[3];
    
    spi_write_read_blocking(spi0, tx_data, rx_data, 3);
    
}



//when start goes high, drdy will go high, then drop after Tsettle, then will start indicating data ready. 
bool first_drdy_fall_detected = false;
void ADS1299_start_continuous_data(){
    ADS1299_send_command(ADS1299_CMD_RDATAC);
    gpio_put(ADS1299_GPIO_START, 1);
    first_drdy_fall_detected = false;
}

//TODO add interrupt handler when DRDY pin goes low


void ADS1299_read_data(uint8_t *data){
    //STATUS BYTE + channels
    spi_read_blocking(spi0, 0x00, data, 3*ADS1299_CHANNELS + 3);
}


void ADS1299_init() {
    //initialize RESET pin and set RESET pin to 1
    gpio_init(ADS1299_GPIO_RESET);
    gpio_set_dir(ADS1299_GPIO_RESET, GPIO_OUT);
    gpio_put(ADS1299_GPIO_RESET, 1);

    //wait for Tpor
    sleep_ms(500);

    //send Reset Pulse
    gpio_put(ADS1299_GPIO_RESET, 0);
    sleep_us(1);
    gpio_put(ADS1299_GPIO_RESET, 1);
    sleep_us(9);

    ADS1299_init_spi();
    //send SDATAC command to write the registers
    ADS1299_cs_select();
    ADS1299_send_command(ADS1299_CMD_SDATAC);
    ADS1299_write_register(ADS1299_REG_CONFIG3, 0b11101000);
    sleep_ms(400);

    //write configuration registers
        //enable CLK output and Set output data rate to 4kSPS
        ADS1299_write_register(ADS1299_REG_CONFIG1,  0b10110010);
        //test signals generated internally
        ADS1299_write_register(ADS1299_REG_CONFIG2,  0b11010000);
        //close SRB1 swithces
        ADS1299_write_register(ADS1299_REG_MISC1, 0b00100000);
    //TODO
    //START data transmission
    ADS1299_send_command(ADS1299_CMD_RDATAC);
    

   

    
    //initialize start pin
    gpio_init(5);
    gpio_set_dir(5,GPIO_OUT);
    
   
}  

uint8_t data[3*ADS1299_CHANNELS + 4];
  

//__________________________________________________________________________
//BESP12E
#define ESP12F_PIN_TX 16
#define ESP12F_PIN_RX 17
#define ESP12F_PIN_EN 18
#define ESP12F_PIN_CS 19
#define ESP12F_PIN_CLK 26
#define ESP12F_PIN_MOSI 27
#define ESP12F_PIN_MISO 28
void ESP12F_init_uart(){
    uart_init(uart0, 115200);

    gpio_set_function(ESP12F_PIN_TX, GPIO_FUNC_UART);
    gpio_set_function(ESP12F_PIN_RX, GPIO_FUNC_UART);

}

void ESP12F_init_spi() {
    spi_init(spi1, 4000000);  // Set SPI clock to 1 MHz
    gpio_set_function(ESP12F_PIN_CLK, GPIO_FUNC_SPI);
    gpio_set_function(ESP12F_PIN_MOSI, GPIO_FUNC_SPI);
    gpio_set_function(ESP12F_PIN_MISO, GPIO_FUNC_SPI);
    

    gpio_init(ESP12F_PIN_CS);
    gpio_set_dir(ESP12F_PIN_CS,GPIO_OUT);
    gpio_put(ESP12F_PIN_CS, 1);

 
    spi_set_format( spi0, 8, 0, 1, SPI_MSB_FIRST);
}
inline void ESP12F_send_ADS_data(uint8_t* data){
    spi_write_blocking(spi1, data, 3*ADS1299_CHANNELS + 4);
}
void ESP12F_init(){
    ESP12F_init_uart();
    ESP12F_init_spi();
}
void ESP12F_write_uart(uint8_t *data){
    uart_write_blocking(uart0, data, sizeof(data));
}
void ESP12F_read_uart(uint8_t *data, uint8_t len){
    uart_read_blocking(uart0, data, len);
}

//_____________________________________________________________________________
//BSDCARD
FRESULT fr;
FATFS fs;
FIL fil;
char filename[] = "data.txt";
void SDCARD_init(){
    
    int ret;
    char buf[100];
    

    if(!sd_init_driver()){
        printf("ERROR: Could not initialize SD Card \r\n");
        sleep_ms(10000);
    }

    fr = f_mount(&fs, "0:", 1);
    if(fr != FR_OK){
        printf("ERROR: COULD not mount filesystem");
    }

    f_open(&fil, filename, FA_WRITE | FA_CREATE_ALWAYS);
   
}

void SDCARD_write_data(){
    
    for(uint8_t i = 0; i < sizeof(data); i++){
        f_putc(data[i], &fil);
    }
    
}
void SDCARD_close_file(){
    f_close(&fil);
}
//________________________________________________________________________________





//commands
#define CMD_CORE1_SEND_DATA 0

//states
bool write_to_sd_card = false;
bool send_to_esp12f = true;

void core1_interrupt_handler(){
    uint8_t command = multicore_fifo_pop_blocking();
    
    //command to send data
    if (command == CMD_CORE1_SEND_DATA){
        ESP12F_send_ADS_data(data);
    }
    multicore_fifo_clear_irq(); //clear interrupt
}
// multi core code
void core1_main() {

    //configure core 1 interrupt
    multicore_fifo_clear_irq();
    irq_set_exclusive_handler(SIO_IRQ_PROC1, core1_interrupt_handler);
    irq_set_enabled(SIO_IRQ_PROC1, true);



    while (1) {
        // a function that keeps this infinite while loop running so that it doesn't take up much energy
        tight_loop_contents();
    }
}



//function that runs when DRDY goes low
void ADS1299_drdy_interrupt(uint gpio, uint32_t event_mask){
    
    //first DRDY drop indicates that adc is settling. The second drop indicates that data is ready.
    if(!first_drdy_fall_detected)
        return;
    first_drdy_fall_detected = true;

    ADS1299_read_data(data);
    //set final byte to state of charge
    data[3*ADS1299_CHANNELS + 3] = MAX17048_read_charge();

    if(send_to_esp12f){
        multicore_fifo_push_blocking(CMD_CORE1_SEND_DATA);
    }
    if(write_to_sd_card){
        SDCARD_write_data();
    }
}


int main() {
    stdio_init_all();

    multicore_launch_core1(core1_main);

    ADS1299_init();
    MAX17048_init();
    ESP12F_init();
    
    gpio_set_irq_enabled_with_callback(ADS1299_GPIO_DRDY,  GPIO_IRQ_EDGE_FALL, true, &ADS1299_drdy_interrupt);
  
    ADS1299_start_continuous_data();

    while (true) {
        float voltage = MAX17048_read_voltage();
        float soc = MAX17048_read_charge();

        //printf("Battery voltage: %.2fV, State of Charge: %.2f%%\n", voltage, soc);
        sleep_ms(1000);
    }
    return 0;
}


/*
TODO List

2. ESP8266 Firmware
3. ANDROID APP
4. Communication between ESP8266 and two cores
    1. UART interrupts and commands
    2. FIFO Commands
*/