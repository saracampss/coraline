#ifndef MQTT_H
#define MQTT_H

void mqtt_start();
void mqtt_envia_mensagem(char * topico, char * mensagem);
void comunicacao_servidor_task(void *params);

#endif // MQTT
