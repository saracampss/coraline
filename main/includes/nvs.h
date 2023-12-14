#ifndef NVS_H
#define NVS_H

#include <stdint.h>

int le_valor_nvs(char *chave);
void grava_valor_nvs(int32_t valor, char *chave);
void grava_nvs_task();
void inicia_valores_nvs();

#endif // NVS
