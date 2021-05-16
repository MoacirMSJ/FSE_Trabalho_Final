#ifndef MQTT_H
#define MQTT_H

void mqtt_start();
void trata_resposta(char *data);
void mqtt_envia_mensagem(char * topico, char * mensagem);

#endif
