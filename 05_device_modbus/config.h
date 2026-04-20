#ifndef _MOBUS_DEVICE_CONF_
#define _MODBUS_DEVICE_CONF_

typedef enum MODBUS_DEVICE
{
	CO_SENSOR,
	PM_SENSOR,
}mbdevice_t;

// Define configuration here
#define CO_DEVICE_PATH		"/dev/co_sensor"
#define PM_DEVICE_PATH		"/dev/pm_sensor"
#define CONFIG_BYTES		5
#define TIMEOUT_FUNC		0x01
#define INTERVAL_FUNC		0x02
#endif // _MODBUS_DEVICE_CONF_
