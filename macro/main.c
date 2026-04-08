#include <stdint.h>
#include <stdio.h>

// Mocking Modbus Master status
typedef struct {
    uint8_t buffer[256];
    uint16_t index;
} ModbusMaster;

// 1. Internal implementation for common structure (01, 02, 03, 04)
static inline void modbusBuildRequest01020304(ModbusMaster *status, uint8_t function, uint16_t reg_addr, uint16_t count) {
    status->buffer[status->index++] = function;
    status->buffer[status->index++] = (uint8_t)(reg_addr >> 8);
    status->buffer[status->index++] = (uint8_t)(reg_addr & 0xFF);
    status->buffer[status->index++] = (uint8_t)(count >> 8);
    status->buffer[status->index++] = (uint8_t)(count & 0xFF);
}

// 2. Macro to define the PDU layer
#define DEFINE_PDU_BUILDER(id) \
    void modbusBuildRequest##id##PDU(ModbusMaster *status, uint16_t reg_addr, uint16_t count) { \
        modbusBuildRequest01020304(status, 0x##id, reg_addr, count); \
    }

// 3. Macro to define the RTU layer (Wrapper)
#define DEFINE_RTU_BUILDER(id) \
    void modbusBuildRequest##id##RTU(ModbusMaster *status, uint8_t slave_addr, uint16_t reg_addr, uint16_t count) { \
        status->index = 0; \
        status->buffer[status->index++] = slave_addr; \
        modbusBuildRequest##id##PDU(status, reg_addr, count); \
        /* CRC would be calculated and added here */ \
        printf("Built RTU frame for Function %02X\n", 0x##id); \
    }

// 4. Using macros to generate functions for Code 03
DEFINE_PDU_BUILDER(03)
DEFINE_RTU_BUILDER(03)

int main() {
    ModbusMaster master;
    
    // Calling the generated RTU function
    // The compiler knows this exists because the Macro expanded it above
    modbusBuildRequest03RTU(&master, 1, 100, 5);
    
    printf("Buffer: ");
    for(int i=0; i < master.index; i++) printf("%02X ", master.buffer[i]);
    
    return 0;
}
