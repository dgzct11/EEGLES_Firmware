#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"

#define MAX17048_ADDRESS 0x36

void max17048_init() {
    // Initialize I2C
    i2c_init(i2c_default, 100000);
    gpio_set_function(4, GPIO_FUNC_I2C);
    gpio_set_function(5, GPIO_FUNC_I2C);
    gpio_pull_up(4);
    gpio_pull_up(5);

    // Reset the MAX17048
    uint8_t reset_data[] = { 0x54, 0x00 };
    i2c_write_blocking(i2c_default, MAX17048_ADDRESS, reset_data, 2, false);

    // Configure the MAX17048 to continuously sample the battery
    uint8_t config_data[] = { 0x40, 0x00 };
    i2c_write_blocking(i2c_default, MAX17048_ADDRESS, config_data, 2, false);
}

float max17048_read_voltage() {
    uint8_t voltage_register[] = { 0x02 };
    uint8_t voltage_data[2];
    i2c_write_blocking(i2c_default, MAX17048_ADDRESS, voltage_register, 1, true);
    i2c_read_blocking(i2c_default, MAX17048_ADDRESS, voltage_data, 2, false);

    uint16_t voltage_raw = (voltage_data[0] << 8) | voltage_data[1];
    return ((float)voltage_raw * 1.25) / 1000.0;
}

float max17048_read_soc() {
    uint8_t soc_register[] = { 0x04 };
    uint8_t soc_data[2];
    i2c_write_blocking(i2c_default, MAX17048_ADDRESS, soc_register, 1, true);
    i2c_read_blocking(i2c_default, MAX17048_ADDRESS, soc_data, 2, false);

    uint16_t soc_raw = (soc_data[0] << 8) | soc_data[1];
    return (float)soc_raw / 256.0;
}

int main() {
    stdio_init_all();
    max17048_init();

    while (true) {
        float voltage = max17048_read_voltage();
        float soc = max17048_read_soc();

        printf("Battery voltage: %.2fV, State of Charge: %.2f%%\n", voltage, soc);
        sleep_ms(1000);
    }
}