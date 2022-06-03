/* Linux boot Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp32/rom/cache.h"

static void IRAM_ATTR map_flash_and_go(void)
{
	unsigned long drom_load_addr_aligned = 0x3f400000;
	unsigned long drom_addr_aligned = 0x00000000;
	unsigned long drom_page_count = (0x00400000 - drom_addr_aligned) / 0x10000;

	unsigned long irom_load_addr_aligned = 0x400d0000;
	unsigned long irom_addr_aligned = 0x00040000;
	unsigned long irom_page_count = (0x00400000 - irom_addr_aligned) / 0x10000;

	Cache_Read_Disable(0);
	Cache_Flush(0);
	cache_flash_mmu_set(0, 0, irom_load_addr_aligned, irom_addr_aligned, 64, irom_page_count);
	cache_flash_mmu_set(0, 0, drom_load_addr_aligned, drom_addr_aligned, 64, drom_page_count);
	Cache_Read_Enable(0);
	asm volatile ("jx %0" :: "r"(irom_load_addr_aligned));
}

void app_main(void)
{
    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("This is %s chip with %d CPU core(s), WiFi%s%s, ",
            CONFIG_IDF_TARGET,
            chip_info.cores,
            (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
            (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    printf("silicon revision %d, ", chip_info.revision);

    printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    printf("Minimum free heap size: %d bytes\n", esp_get_minimum_free_heap_size());

    map_flash_and_go();
    esp_restart();
}
