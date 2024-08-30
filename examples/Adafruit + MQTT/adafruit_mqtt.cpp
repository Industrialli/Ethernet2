#include "industrialli_hub.hpp"

/* Biblioteca de comunicação com Adafruit
 * via MQTT.
 */
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>

/* Informações para conexão com servidor
 * Adafruit.
 */
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "***********"
#define AIO_KEY         "***********"

uint8_t mac_address[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
EthernetClient client;

/* Cria um objeto cliente para se comunicar com 
 * servidor do Adafruit.
 */
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/* Cria um objeto para envio de dados da temperatura.
 */
Adafruit_MQTT_Publish temperature = Adafruit_MQTT_Publish(&mqtt,  AIO_USERNAME "/feeds/temperature");

industrialli_hub hub;

void setup(){
	hub.begin();
    
    /* Inicializa a biblioteca Ethernet.
     */
    Ethernet.init();

    /* Configura um MAC para o shield w5500, e obtem outras
     * informações através de um servidor DHCP.
     */
    Ethernet.begin(mac_address);

    analog_input.begin();
    analog_input.set_read_mode(A01, READ_20mA);
}

void loop() {
    /* Se estiver conectado ao adafruit, então envia os dados
     * para o servidor.
     */
    if(mqtt.connected()){
        temperature.publish(analog_input.map_pin(A01, 4, 20, 0, 200));

        /* Limite de 30 publicações por minuto.
         */
        delay(2000);

    }else {

        /* Se não estiver conectado, tenta se conectar ao servidor
         * a cada 5 segundos.
         */
        while(mqtt.connect()) {
            mqtt.disconnect();
            delay(5000);
        }
    }

	leds.update();
}