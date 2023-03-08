#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include <stdint.h>


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

bool * max17048_alert_handler(){
    
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
    return result;
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




int main() {
    stdio_init_all();
    max17048_init();

    while (true) {
        float voltage = max17048_read_voltage();
        float soc = max17048_read_charge();

        printf("Battery voltage: %.2fV, State of Charge: %.2f%%\n", voltage, soc);
        sleep_ms(1000);
    }
}