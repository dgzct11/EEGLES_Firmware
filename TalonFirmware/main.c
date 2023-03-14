#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include "hardware/spi.h"
#include <stdint.h>

//SD Card
//battery sensor
//ads1299
//wifi
/* ------------------------------------------------------------------------------------------------------
*/
//Hardware Controller definitions
#define BS_I2C i2c1
#define SD__SPI
#define ADS_SPI 


//Pin Defitions
#define BS_DIO 22
#define BS_SCL 23
#define BS_ALRT 24



/*___________________________________________________________________________________________________________________
MAX17048
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
ADS1299
*/
// Define ADS1299 register addresses
#define ADS1299_REG_ID         0x00
#define ADS1299_REG_CONFIG1    0x01
#define ADS1299_REG_CONFIG2    0x02
#define ADS1299_REG_CONFIG3    0x03
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
    asm volatile("nop \n nop \n nop");
    gpio_put(4, 0);  // Active low
    asm volatile("nop \n nop \n nop");
}

static inline void cs_deselect() {
    asm volatile("nop \n nop \n nop");
    gpio_put(4, 1);
    asm volatile("nop \n nop \n nop");
}

// Initialize the SPI interface
void init_spi() {
    spi_init(spi0, 2000000);  // Set SPI clock to 1 MHz
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

void ads1299_init() {
    init_spi();


    cs_select();
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




int main() {
    stdio_init_all();
    max17048_init();

    while (true) {
        float voltage = max17048_read_voltage();
        float soc = max17048_read_charge();

        //printf("Battery voltage: %.2fV, State of Charge: %.2f%%\n", voltage, soc);
        sleep_ms(1000);
    }
    return 0;
}