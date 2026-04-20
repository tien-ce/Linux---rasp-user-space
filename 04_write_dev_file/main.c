#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

// Define configuration here
#define DEVICE_PATH "/dev/co_sensor"
/* [1byte] function code [4byte] value 
 * Be careful with Little or Big endian format
 * The following example is for little endian format.
 * If you are using big endian format, 4 bytes value order need to be reversed
 * {0x01,0,0,0,100}
 * */
static const uint8_t RAW_VALUE[] = {0x01,100,0,0,0};

int main()
{
    int fd = open(DEVICE_PATH, O_WRONLY);
    if (fd < 0) {
        fprintf(stderr, "Error opening %s: %s\n", DEVICE_PATH, strerror(errno));
        return EXIT_FAILURE;
    }

    ssize_t bytes_written = write(fd, RAW_VALUE, sizeof(RAW_VALUE));
    
    if (bytes_written < 0) {
        // Captures your kernel module's return values (-EFAULT, -ENOMEM)
        fprintf(stderr, "Kernel Error: %s\n", strerror(errno));
        close(fd);
        return EXIT_FAILURE;
    }

    printf("Success: Wrote %zd bytes to %s\n", bytes_written, DEVICE_PATH);

    close(fd);
    return EXIT_SUCCESS;
}
