#include "driver_aht30_interface.h"

static aht30_handle_t aht30;

bool init_climate_sensor()
{
    DRIVER_AHT30_LINK_INIT(&aht30, aht30_handle_t);
    DRIVER_AHT30_LINK_IIC_INIT(&aht30, aht30_interface_iic_init);
    DRIVER_AHT30_LINK_IIC_DEINIT(&aht30, aht30_interface_iic_deinit);
    DRIVER_AHT30_LINK_IIC_READ_CMD(&aht30, aht30_interface_iic_read_cmd);
    DRIVER_AHT30_LINK_IIC_WRITE_CMD(&aht30, aht30_interface_iic_write_cmd);
    DRIVER_AHT30_LINK_DELAY_MS(&aht30, aht30_interface_delay_ms);
    DRIVER_AHT30_LINK_DEBUG_PRINT(&aht30, aht30_interface_debug_print);
    return aht30_init(&aht30) == 0;
}

bool read_climate(float *temperature, uint8_t *humidity)
{
    uint32_t temperature_raw;
    uint32_t humidity_raw;
    return aht30_read_temperature_humidity(&aht30, &temperature_raw,
        temperature, &humidity_raw, humidity) == 0;
}
