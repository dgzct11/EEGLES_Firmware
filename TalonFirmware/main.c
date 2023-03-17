#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "pico/multicore.h"
#include "hardware/i2c.h"
#include "hardware/spi.h"
#include "hardware/pio.h"
#include "hardware/uart.h"
#include <stdint.h>

//SD Card
//battery sensor
//ads1299
//wifi
/* ------------------------------------------------------------------------------------------------------
*/
//Hardware Controller definitions
#define BS_I2C i2c1



//Pin Defitions
#define BS_DIO 22
#define BS_SCL 23
#define BS_ALRT 24



/*___________________________________________________________________________________________________________________
BMAX17048
*/
#define MAX17048_ADDRESS 0x36
//function to handle alerts for the battery sensor





//ALERT Codes
#define LOW_CHARGE_ALRT 4
#define CHARGE_CHANGE_ALRT 3
#define RESET_ALRT 2
#define OVERVOLTAGE_ALRT 0
#define UNDERVOLTAGE_ALRT 1

void max17048_alert_handler(){
    
    uint8_t status_register[] = { 0x1A };
    uint8_t status_data[2];
    i2c_write_blocking(BS_I2C, MAX17048_ADDRESS, status_register, 1, true);
    i2c_read_blocking(BS_I2C, MAX17048_ADDRESS, status_data, 2, false);
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
void max17048_reset() {
    uint8_t reset_cmd[] = {0xFE, 0x54,0x00};
    i2c_write_blocking(BS_I2C, MAX17048_ADDRESS, reset_cmd, 3, false);
}
void max17048_init() {
    // Initialize I2C
    i2c_init(BS_I2C, 100 * 1000);
    
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
    irq_set_exclusive_handler(GPIO_IRQ_EDGE_FALL, max17048_alert_handler);

    // Enable interrupts for the GPIO pin
    irq_set_enabled(GPIO_IRQ_EDGE_FALL, true);
}

float max17048_read_voltage() {
    uint8_t voltage_register[] = { 0x02 };
    uint8_t voltage_data[2];
    i2c_write_blocking(BS_I2C, MAX17048_ADDRESS, voltage_register, 1, true);
    i2c_read_blocking(BS_I2C, MAX17048_ADDRESS, voltage_data, 2, false);

    uint16_t voltage_raw = (voltage_data[0] << 8) | voltage_data[1];
    return ((float)voltage_raw * 1.25) / 1000.0;
}

float max17048_read_charge() {
    uint8_t soc_register[] = { 0x04 };
    uint8_t soc_data[2];
    i2c_write_blocking(BS_I2C, MAX17048_ADDRESS, soc_register, 1, true);
    i2c_read_blocking(BS_I2C, MAX17048_ADDRESS, soc_data, 2, false);

    uint16_t soc_raw = (soc_data[0] << 8) | soc_data[1];
    return (float)soc_raw / 256.0;
}

/*___________________________________________________________________________________________________________________
BADS1299
*/

//define ADS1299 pins
#define ADS1299_GPIO_RESET 6
#define ADS1299_GPIO_CS 4
#define ADS1299_GPIO_START 5
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

// Define other register addresses...

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
static inline void cs_select() {
    
    gpio_put(4, 0);  // Active low
    
}

static inline void cs_deselect() {
   
    gpio_put(4, 1);
    
}

// Initialize the SPI interface
void init_spi() {
    spi_init(spi0, 4000000);  // Set SPI clock to 1 MHz
    gpio_set_function(0, GPIO_FUNC_SPI);
    gpio_set_function(2, GPIO_FUNC_SPI);
    gpio_set_function(7, GPIO_FUNC_SPI);
    bi_decl(bi_3pins_with_func(0, 7, 2, GPIO_FUNC_SPI));

    gpio_init(4);
    gpio_set_dir(4,GPIO_OUT);
    gpio_put(4, 1);

    bi_decl(bi_1pin_with_name(1, "SPI CS"));
    spi_set_format( spi0, 8, 0, 1, SPI_MSB_FIRST);
}



// Send a command to the ADS1299
void send_command(uint8_t cmd) {
    uint8_t tx_data[1] = { cmd };
    uint8_t rx_data[1];
    spi_write_read_blocking(spi0, tx_data, rx_data, 1);
}

//TODO finish read and write functions based on WREG and RREG command.
// Read a register value from the ADS1299
uint8_t read_register(uint8_t reg_addr) {
    reg_addr |= READ_BIT;
    uint8_t tx_data[3] = { reg_addr, 0x00, 0x00 };
    uint8_t rx_data[3];
    
    spi_write_read_blocking(spi0, tx_data, rx_data, 3);
   
    return rx_data[2];
}

void read_consecutive_registers( uint8_t reg_addr, uint8_t len, uint8_t *read_buf ){
    reg_addr |= READ_BIT;
    uint8_t tx_data[len+2];
    tx_data[0] = reg_addr;
    tx_data[1] = len-1;

    spi_write_read_blocking(spi0, tx_data, read_buf, len+2);
}

void write_consecutive_registers( uint8_t reg_addr, uint8_t *data){
    reg_addr |= WRITE_BIT;
    uint8_t len = sizeof(data);
    uint8_t tx_data[len+2];
    tx_data[0]= reg_addr;
    tx_data[1] = len;
    uint8_t rx_data[len+2];
    spi_write_read_blocking(spi0, tx_data, rx_data, len+2); 
}
// Write a register value to the ADS1299
void write_register(uint8_t reg_addr, uint8_t reg_val) {
    reg_addr |= WRITE_BIT;
    uint8_t tx_data[3] = { reg_addr,0x00,reg_val };
    uint8_t rx_data[3];
    
    spi_write_read_blocking(spi0, tx_data, rx_data, 3);
    
}



//when start goes high, drdy will go high, then drop after Tsettle, then will start indicating data ready. 
bool first_drdy_fall_detected = false;
void start_continuous_data(){
    send_command(ADS1299_CMD_RDATAC);
    gpio_put(ADS1299_GPIO_START, 1);
    first_drdy_fall_detected = false;
}

//TODO add interrupt handler when DRDY pin goes low


void read_data(uint8_t *data){
    //STATUS BYTE + 8-channel 
    spi_read_blocking(spi0, 0x00, data, ADS1299_CHANNELS + 1);
}


void ads1299_init() {
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

    init_spi();
    //send SDATAC command to write the registers
    cs_select();
    send_command(ADS1299_CMD_SDATAC);
    write_register(ADS1299_REG_CONFIG3, 0b11101000);
    sleep_ms(400);

    //write configuration registers
        //enable CLK output and Set output data rate to 4kSPS
        write_register(ADS1299_REG_CONFIG1,  0b10110010);
        //test signals generated internally
        write_register(ADS1299_REG_CONFIG2,  0b11010000);
        //close SRB1 swithces
        write_register(ADS1299_REG_MISC1, 0b00100000);
    //TODO
    //START data transmission
    send_command(ADS1299_CMD_RDATAC);
    

   

    
    //initialize start pin
    gpio_init(5);
    gpio_set_dir(5,GPIO_OUT);
    
   
}     

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
    bi_decl(bi_3pins_with_func(ESP12F_PIN_CLK, ESP12F_PIN_MOSI, ESP12F_PIN_MISO, GPIO_FUNC_SPI));

    gpio_init(ESP12F_PIN_CS);
    gpio_set_dir(ESP12F_PIN_CS,GPIO_OUT);
    gpio_put(ESP12F_PIN_CS, 1);

    bi_decl(bi_1pin_with_name(ESP12F_PIN_CS, "SPI CS"));
    spi_set_format( spi0, 8, 0, 1, SPI_MSB_FIRST);
}


void ESP12F_write(uint8_t *data){
    uart_write_blocking(uart0, data, sizeof(data));
}
void ESP12F_read(uint8_t *data, uint8_t len){
    uart_read_blocking(uart0, data, len);
}
//function on other core to send data to ESP12F
void ESP12F_send_data(){

}
//_____________________________________________________________________________
//BSDCARD


void core1_interrupt_handler(){

}
// multi core code
void core1_main() {
    while (1) {

    }
}
void read_ADS1299_data(){
    if(first_drdy_fall_detected = false)
        return;
    first_drdy_fall_detected = true;
    uint8_t data[ADS1299_CHANNELS + 2];
    read_data(data);
    //set final byte to state of charge
    data[ADS1299_CHANNELS + 1] = max17048_read_charge();
    multicore_fifo_push_blocking(0);
    
    
}


int main() {
    stdio_init_all();
    ads1299_init();
    max17048_init();
    
    multicore_reset_core1();
    multicore_launch_core1(core1_main());

    while (true) {
        float voltage = max17048_read_voltage();
        float soc = max17048_read_charge();

        //printf("Battery voltage: %.2fV, State of Charge: %.2f%%\n", voltage, soc);
        sleep_ms(1000);
    }
    return 0;
}


/*
TODO List
1. Continuous Reading from ADS1299, activated by DRDY pin ????
2. Writing to SD Card   All Good
3. Transfering to WIFI Card
4. ESP8266 Firmware
*/