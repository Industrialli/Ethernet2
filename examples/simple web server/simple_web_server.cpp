#include "industrialli_hub.hpp"

uint8_t mac_address[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

industrialli_hub hub;

/* Cria um servidor/socket na porta 80. Assim
 * para se conectar ao servidor HTTP (supondo que o
 * cliente e servidor estão na mesma rede) basta acessar
 * a página web inserindo o endereço IP na barra de pesquisa 
 * no navegador.
 */
EthernetServer server(80);

void setup(){
    hub.begin();

    /* Inicializa a biblioteca Ethernet.
     */
    Ethernet.init();

    /* Configura um MAC para o shield w5500, e obtem outras
     * informações através de um servidor DHCP.
     */
    Ethernet.begin(mac_address);

    /* Exibe no console o endereço IP obtido por um servidor DHCP.
     * Este endereço é necessário para poder acessar o servidor HTTP/site
     * gerado pela PLC no navegador.
     */
    Serial.println(Ethernet.localIP());


    /* Inicializa a biblioteca de entradas analógicas
     */
    analog_input.begin();

    /* Configura as entradas analógicas A01 e A02 para operarem
     * no modo de leitura 0 - 20mA.
     */
    analog_input.set_read_mode(A01, READ_20mA);
    analog_input.set_read_mode(A02, READ_20mA);
}

void loop(){

    /* Estabelece uma conexão entre o cliente e servidor, e envia
     * a página web usando protocolo HTTP.
     */
    EthernetClient client = server.available();
    
    if (client){
        boolean currentLineIsBlank = true;

        while (client.connected()) {
            if (client.available()) {
                char c = client.read();
                
                if (c == '\n' && currentLineIsBlank) {
                
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-Type: text/html");
                    client.println("Connection: close");
                    client.println("Refresh: 5");
                    client.println();
                    client.println("<!DOCTYPE HTML>");
                    client.println("<html>");

                    /* Nessa seção HTML é informado os dados da temperatura
                     * e pressão obtidos pelos sensores conectados nas portas
                     * analógicas A01 e A02.
                     */
                    client.print("<h3>");client.print(analog_input.map_pin(A01, 4, 20, 0, 200));client.println(" &#8451;</h3>");
                    client.print("<h3>");client.print(analog_input.map_pin(A02, 4, 20, 0, 10));client.println(" Bar</h3>");

                    client.println("</html>");
                    break;
                }

                if (c == '\n') {
                    currentLineIsBlank = true;
                }else if (c != '\r') {
                    currentLineIsBlank = false;
                }
            }
        }

        delay(1);
        client.stop();
    }
}