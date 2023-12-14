/*

	DHT22 temperature sensor driver

*/

#ifndef DHT22_H_
#define DHT22_H_

#define DHT_OK 0
#define DHT_CHECKSUM_ERROR -1
#define DHT_TIMEOUT_ERROR -2

// == function prototypes =======================================

void set_dht_gpio(int gpio);
void error_handler(int response);
int read_dht();
float get_humidity();
float get_temperature();
int get_signal_level(int usTimeOut, bool state);
void dht_task(void *params);

#endif
