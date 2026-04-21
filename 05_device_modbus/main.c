#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include "config.h"

#define SYS_TIMEOUT_MS		100
#define SYS_INTERVAL_MS		1000
/* fmt is the first argument you pass (usually the format string),
 * and if it is tring, it will merged into the following string.
 * ... / __VA_ARGS__: Every argument after the first one.
 * Standard: __VA_ARGS__ expands exactly what you put in the ... section.
 * GNU Extension: ##__VA_ARGS__ is the "comma swallower." It deletes the comma if the ... section is empty,
 * preventing a syntax error in functions like printf
 * If you call ERROR("Value: %d", x), then:
 *	-	fmt becomes "Value: %d"
 *	-	__VA_ARGS__ becomes x
 *	--> fprintf(strerr, "Value: %d: %s\n",x,strerror(err_copy));
 *	if you is just pass ERROR("Hello world")
 *  --> fprintf(strerr, "Hello world: %s\n",strerror(err_copy));
 */
#define ERROR(fmt,...)		do { \
							int err_copy = errno; \
							fprintf(stderr, fmt ": %s\n",##__VA_ARGS__, strerror(err_copy)); \
							}while(0)
/* [1byte] function code [4byte] value 
 * Be careful with Little or Big endian format
 * The following example is for little endian format.
 * If you are using big endian format, 4 bytes value order need to be reversed
 * {0x01,0,0,0,100}
 * */
//static const uint8_t RAW_VALUE[] = {0x01,100,0,0,0};


/**
 * @brief Packs a function code and a 32-bit value into a buffer (Little Endian).
 * * @param buffer  The destination uint8_t array (must be at least 5 bytes).
 * @param fn_code The 1-byte function code.
 * @param value   The 32-bit value to pack.
 */
static void raw_pacakge(uint8_t *buffer, uint8_t fn_code, uint32_t value) {
    // Store function code at index 0
    buffer[0] = fn_code;
    // Store 32-bit value in Little Endian format (LSB first)
    buffer[1] = (uint8_t)(value & 0xFF);         // Least Significant Byte
    buffer[2] = (uint8_t)((value >> 8) & 0xFF);
    buffer[3] = (uint8_t)((value >> 16) & 0xFF);
    buffer[4] = (uint8_t)((value >> 24) & 0xFF); // Most Significant Byte
}

/**
 * @brief Configures modbus device parameters by sending a packed control frame.
 * * This function serializes a 1-byte function code and a 32-bit value into a 
 * 5-byte Little Endian stream and writes it to the kernel device driver.
 * * @param device The target sensor (CO_SENSOR or PM_SENSOR).
 * @param func   The function code (TIMEOUT_FUNC or INTERVAL_FUNC).
 * @param value  The 32-bit unsigned integer value to be set.
 * @return true if the write operation succeeded, false otherwise.
 */
static bool write_config(mbdevice_t device, uint8_t func, uint32_t value)
{
    const char *path; // Use pointer instead of char array for assignment
    switch(device) {
        case CO_SENSOR: path = CO_DEVICE_PATH; break;
        case PM_SENSOR: path = PM_DEVICE_PATH; break;
        default: return false;
    }

    uint8_t raw_buf[CONFIG_BYTES];
    raw_pacakge(raw_buf, func, value);

    int fd = open(path, O_WRONLY);
    if (fd < 0) {
        ERROR("Error Opening %s",path);
        return false;
    }

    ssize_t bytes_written = write(fd, raw_buf, sizeof(raw_buf));
    if (bytes_written < 0) {
		ERROR("Kernel Write Error");
        return false;
    }
    close(fd);
    printf("Success: Config 0x%02x wrote %zd bytes to %s\n", func, bytes_written, path);
    return true;
}

/**
 * @brief Retrieves a 32-bit measurement value from the modbus device driver.
 * * This function performs a synchronous read from the character device file.
 * It expects a 4-byte response from the kernel representing the sensor data.
 * * @param value Pointer to a 32-bit unsigned integer where the result is stored.
 * @return true if the read operation succeeded, false otherwise.
 */
static bool read_co_value(uint16_t *co_value)
{
    const char *path = CO_DEVICE_PATH;
    int fd = open(path, O_RDONLY);
    
    if (fd < 0){
		ERROR("[DEBUG] Open failed");
        return false;
    }

    uint8_t buffer_val[2]; // One register constain 2 bytes
    ssize_t byte_read = read(fd, &buffer_val, sizeof(buffer_val));
    if (byte_read < 0) {
		ERROR("[DEBUG] Read syscall failed");
        return false;
    }
    close(fd);
	/* Data recive from Modbus will be big-endian format
	 *	Value: 0x01 0x02 ---> buff[0]: 0x02	buff[1]: 0x01
	 * */
    *co_value = buffer_val[0] << 8 | buffer_val[1];
    return true;
}
static bool read_pm_value(uint16_t *pm1_0_val, uint16_t *pm2_5_val)
{
	const char *path = PM_DEVICE_PATH;
	int fd = open(path, O_RDONLY);
	if (fd < 0)
	{
		ERROR("[DEBUG] Open failed");
	}
	uint8_t buffer_val[4]; // PM1.0[0,1], PM2.5[2,3]
    ssize_t byte_read = read(fd, &buffer_val, sizeof(buffer_val));
    if (byte_read < 0) {
		ERROR("[DEBUG] Read syscall failed");
        return false;
    }
	/* Same logic with co_value */
    *pm1_0_val = buffer_val[0] << 8 | buffer_val[1];
    *pm2_5_val = buffer_val[2] << 8 | buffer_val[3];
    close(fd);
	return true;
}
int main()
{
	/* System config
	 * These functions apply the change into parameter used in kernel
	 * @interval: This is the interval time the kernel read again to fetch new data.
	 * @timeot: This is the waiting timeout when trying read data from modbus device.
	 * If can, you should implement your own interval time function on user space, 
	 * because if you set the interval too low, multiple applications trying to read data will cause the system overload.
	 * */	
	// Timeout Config
    if (!write_config(CO_SENSOR, TIMEOUT_FUNC, 100)) {
        fprintf(stderr, "Failed to config timeout\n");
        return EXIT_FAILURE;
    }

    // Interval Config
    if (!write_config(CO_SENSOR, INTERVAL_FUNC, 1000)) {
        fprintf(stderr, "Failed to config interval\n");
        return EXIT_FAILURE;
    }

	while(1)
	{
		uint16_t co_val = 0;
		uint16_t pm1_0_val;
		uint16_t pm2_5_val;

		if(!read_co_value(&co_val))
		{
			fprintf(stderr, "Error when read CO vaule\n");
		}
		else
		{
			printf("Read CO value: %d\n",co_val);
		}
		if (!read_pm_value(&pm1_0_val, &pm2_5_val))
		{
			fprintf(stderr, "Error when reading PM value");
		}
		else
		{
			printf("PM1.0: %d, PM2.5: %d",pm1_0_val,pm2_5_val);
		}
		sleep(1);
	}
    return EXIT_SUCCESS;
}
