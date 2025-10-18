#include "driver_aht30_interface.h"

#include <pico/binary_info.h>
#include <hardware/i2c.h>
#include <hardware/gpio.h>

#include <stdarg.h>

static bool reserved_addr(uint8_t addr) {
    return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
}


/**
 * @brief  interface iic bus init
 * @return status code
 *         - 0 success
 *         - 1 iic init failed
 * @note   none
 */
uint8_t aht30_interface_iic_init(void)
{
    i2c_init(i2c_default, 50'000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
    bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));
    return 0;
}

/**
 * @brief  interface iic bus deinit
 * @return status code
 *         - 0 success
 *         - 1 iic deinit failed
 * @note   none
 */
uint8_t aht30_interface_iic_deinit(void)
{
    i2c_deinit(i2c_default);
    gpio_deinit(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_deinit(PICO_DEFAULT_I2C_SCL_PIN);
    gpio_disable_pulls(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_disable_pulls(PICO_DEFAULT_I2C_SCL_PIN);
    return 0;
}

/**
 * @brief      interface iic bus read
 * @param[in]  addr iic device write address
 * @param[out] *buf pointer to a data buffer
 * @param[in]  len length of the data buffer
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 * @note       none
 */
uint8_t aht30_interface_iic_read_cmd(uint8_t addr, uint8_t *buf, uint16_t len)
{
    return i2c_read_blocking(i2c_default, addr, buf, len, false) < 0;
}

/**
 * @brief     interface iic bus write
 * @param[in] addr iic device write address
 * @param[in] *buf pointer to a data buffer
 * @param[in] len length of the data buffer
 * @return    status code
 *            - 0 success
 *            - 1 write failed
 * @note      none
 */
uint8_t aht30_interface_iic_write_cmd(uint8_t addr, uint8_t *buf, uint16_t len)
{
    return i2c_write_blocking(i2c_default, addr, buf, len, false) < 0;
}

/**
 * @brief     interface delay ms
 * @param[in] ms time
 * @note      none
 */
void aht30_interface_delay_ms(uint32_t ms)
{
    sleep_ms(ms);
}

/**
 * @brief     interface print format data
 * @param[in] fmt format data
 * @note      none
 */
void aht30_interface_debug_print(const char *const fmt, ...)
{
    va_list myargs;
    va_start(myargs, fmt);
    vprintf(fmt, myargs);
    va_end(myargs);
}
