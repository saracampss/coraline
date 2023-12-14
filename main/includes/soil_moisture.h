#ifndef _SOIL_MOISTURE_
#define _SOIL_MOISTURE_

#include "esp_adc/adc_oneshot.h"

void adc_init();
void adc_config_pin(adc_channel_t channel);
void adc_deinit();
int analogRead(adc_channel_t channel);
void soil_task(void *params);

#endif // _SOIL_MOISTURE_
