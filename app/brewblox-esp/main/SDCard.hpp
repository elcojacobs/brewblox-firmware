#pragma once

#include "driver/gpio.h"
#include "driver/sdspi_host.h"
#include "driver/spi_common.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "sdkconfig.h"
#include "sdmmc_cmd.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/unistd.h>

#include "driver/sdspi_host.h"
#include "driver/spi_common.h"
#define SPI_DMA_CHAN 1
// Pin mapping when using SPI mode.
// With this mapping, SD card can be used both in SPI and 1-line SD mode.
// Note that a pull-up on CS line is required in SD mode.
constexpr auto PIN_NUM_MISO = GPIO_NUM_12;
constexpr auto PIN_NUM_MOSI = GPIO_NUM_13;
constexpr auto PIN_NUM_CLK = GPIO_NUM_14;
constexpr auto PIN_NUM_CS = GPIO_NUM_5;
constexpr auto PIN_NUM_TFT_CS = GPIO_NUM_4;

static const char* TAG = "SD test";

#define MOUNT_POINT "/sdcard"

class SDCard {
public:
    static void test()
    {
        esp_err_t ret;

        gpio_set_direction(PIN_NUM_CS, GPIO_MODE_OUTPUT);
        gpio_set_direction(PIN_NUM_TFT_CS, GPIO_MODE_OUTPUT);

        gpio_set_level(PIN_NUM_CS, 1);
        gpio_set_level(PIN_NUM_TFT_CS, 1);

        gpio_set_pull_mode(PIN_NUM_MISO, GPIO_PULLUP_ONLY);
        gpio_pullup_en(PIN_NUM_MISO);

        // Options for mounting the filesystem.
        // If format_if_mount_failed is set to true, SD card will be partitioned and
        // formatted in case when mounting fails.
        esp_vfs_fat_sdmmc_mount_config_t mount_config = {
#if 1
            .format_if_mount_failed = true,
#else
            .format_if_mount_failed = false,
#endif // EXAMPLE_FORMAT_IF_MOUNT_FAILED
            .max_files = 5,
            .allocation_unit_size = 16 * 1024
        };
        sdmmc_card_t* card;
        const char mount_point[] = MOUNT_POINT;
        ESP_LOGI(TAG, "Initializing SD card");

        // Use settings defined above to initialize SD card and mount FAT filesystem.
        // Note: esp_vfs_fat_sdmmc/sdspi_mount is all-in-one convenience functions.
        // Please check its source code and implement error recovery when developing
        // production applications.

        sdmmc_host_t host = SDSPI_HOST_DEFAULT();

        spi_bus_config_t bus_cfg
            = {
                .mosi_io_num = PIN_NUM_MOSI,
                .miso_io_num = PIN_NUM_MISO,
                .sclk_io_num = PIN_NUM_CLK,
                .quadwp_io_num = -1,
                .quadhd_io_num = -1,
                .max_transfer_sz = 4000,
            };

        ret = spi_bus_initialize(spi_host_device_t(host.slot), &bus_cfg, SPI_DMA_CHAN);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to initialize bus.");
            return;
        }

        // This initializes the slot without card detect (CD) and write protect (WP) signals.
        // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
        sdspi_device_config_t slot_config = {
            .host_id = HSPI_HOST,
            .gpio_cs = PIN_NUM_CS,
            .gpio_cd = SDSPI_SLOT_NO_CD,
            .gpio_wp = SDSPI_SLOT_NO_WP,
            .gpio_int = GPIO_NUM_NC,
        };

        ret = esp_vfs_fat_sdspi_mount(mount_point, &host, &slot_config, &mount_config, &card);

        if (ret != ESP_OK) {
            if (ret == ESP_FAIL) {
                ESP_LOGE(TAG,
                         "Failed to mount filesystem. "
                         "If you want the card to be formatted, set the EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
            } else {
                ESP_LOGE(TAG,
                         "Failed to initialize the card (%s). "
                         "Make sure SD card lines have pull-up resistors in place.",
                         esp_err_to_name(ret));
            }
            return;
        }

        // Card has been initialized, print its properties
        sdmmc_card_print_info(stdout, card);

        // Use POSIX and C standard library functions to work with files.
        // First create a file.
        ESP_LOGI(TAG, "Opening file");
        FILE* f = fopen(MOUNT_POINT "/hello.txt", "w");
        if (f == NULL) {
            ESP_LOGE(TAG, "Failed to open file for writing");
            return;
        }
        fprintf(f, "Hello %s!\n", card->cid.name);
        fclose(f);
        ESP_LOGI(TAG, "File written");

        // Check if destination file exists before renaming
        struct stat st;
        if (stat(MOUNT_POINT "/foo.txt", &st) == 0) {
            // Delete it if it exists
            unlink(MOUNT_POINT "/foo.txt");
        }

        // Rename original file
        ESP_LOGI(TAG, "Renaming file");
        if (rename(MOUNT_POINT "/hello.txt", MOUNT_POINT "/foo.txt") != 0) {
            ESP_LOGE(TAG, "Rename failed");
            return;
        }

        // Open renamed file for reading
        ESP_LOGI(TAG, "Reading file");
        f = fopen(MOUNT_POINT "/foo.txt", "r");
        if (f == NULL) {
            ESP_LOGE(TAG, "Failed to open file for reading");
            return;
        }
        char line[64];
        fgets(line, sizeof(line), f);
        fclose(f);
        // strip newline
        char* pos = strchr(line, '\n');
        if (pos) {
            *pos = '\0';
        }
        ESP_LOGI(TAG, "Read from file: '%s'", line);

        // All done, unmount partition and disable SDMMC or SPI peripheral
        esp_vfs_fat_sdcard_unmount(mount_point, card);
        ESP_LOGI(TAG, "Card unmounted");

        //deinitialize the bus after all devices are removed
        spi_bus_free(spi_host_device_t(host.slot));
        return;
    }
};
