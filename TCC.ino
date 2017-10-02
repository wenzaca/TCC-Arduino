#include <SPI.h>
#include <Ethernet.h>
#include <elapsedMillis.h>

elapsedMillis timeElapsed;

// for DHT22 sensor
#include "DHT.h"
#define DHTPIN 2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

int ldrPin = 0; //LDR no pino analígico 8
float ldrValor = 0; //Valor lido do LDR

const int potenciometro = 5; // pino de entrada do potenciômetro

uint8_t hwaddr[6] = {0xDE, 0xAD, 0xBE, 0xEF, 0xBA, 0xBE}; // mac-adress do arduino
uint8_t ipaddr[4] = {192, 168, 100, 1};    // Endereço de IP do arduino (vai depender do seu gateway)
//uint8_t gwaddr[4] = {192, 168, 0, 1};  // Endereço de IO do seu Gateway (Não necessario para este Exemplo)
//uint8_t subnet[4] = {255, 255, 255, 0};                   // mascara da rede   (Não necessario para este Exemplo)

IPAddress serverip(192, 168, 100, 2);                   //Endereço de IP do Servidor

int serverport = 8080;     // a porta do Tomcat/Glassfish que esta sendo usada, no meu caso uso 8084

EthernetClient client;     //Instancia do Cliente para Conexao com Web Server

unsigned int interval = 15000;
boolean LED_status = 0;

void setup()
{
  dht.begin();
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  // start the Ethernet connection and the server:
  Ethernet.begin(hwaddr, ipaddr);
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}

void loop()
{

  if (client.connect(serverip, serverport)) {
    while (timeElapsed < interval)
    {
      process(client);
    }
      ldrValor = analogRead(ldrPin);
      float potVal = analogRead(potenciometro);
      client.print("GET /MagnetoControl/values?temperatura=");            //Envia a url + o parametro para o Servidor Apache TomCat atraves da pagina jsp insereTemperaturaUmidade.jsp que aguarda os parametros temperatura e umidade
      client.print(dht.readTemperature(), 2);                                          // Envia o dado de temperatura lido do Sensor naquele momento
      client.print("&umidade=");
      client.print(dht.readHumidity(), 2);   //Envia o dado de umidade lido do Sensor naquele momento
      client.print("&luminosidade=");
      client.print(abs(1024 - ldrValor), 2); //Envia o dado de luminosidade lido do Sensor naquele momento
      client.print("&potenciometro=");
      client.print(potVal, 2);   //Envia o dado de luminosidade lido do Sensor naquele momento

      client.println(" HTTP/1.1");                  //script padrão para o protocolo HTTP
      client.println("Host: www.arduinoHuno.com");    //
      client.println("User-Agent: Arduino");        //
      client.println("Accept: text/html");          //
      client.println();                             //
      timeElapsed = 0;       // reset the counter to 0 so the counting starts over...
    
  } else {
    //Se a Conexão Falhar envia a informação de Erro para Serial
    client.println("Erro ao Estabelecer Conexão");
  }
  // give the web browser time to receive the data
  // close the connection:
  client.stop();
  Serial.println("client disonnected");
}

void process(EthernetClient client){
  char c = client.read();
  String httpReq="";
  httpReq += c;
  if (httpReq.indexOf("warning") > -1) {  // see if checkbox was clicked
        // the checkbox was clicked, toggle the LED
            LED_status = 1;
    }
    
    if (LED_status) {    // switch LED on
        digitalWrite(2, HIGH);
    }
    else {              // switch LED off
        digitalWrite(2, LOW);
    }
}


