#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

// Substitua com suas credenciais de rede
const char* ssid = "Teste";
const char* password = "teste123";

// Definição dos pinos de saída
const int output26 = 26;
const int output27 = 27;

// Estado dos pinos de saída
String output26State = "off";
String output27State = "off";

// Instancia o servidor na porta 80
AsyncWebServer server(80);

// Página HTML em PROGMEM
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>ESP32 Web Server</title>
    <style>
      html { font-family: Helvetica; display: inline-block; text-align: center; }
      .button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px; text-decoration: none; font-size: 30px; cursor: pointer; }
      .button2 { background-color: #555555; }
    </style>
    <script>
      function toggleGPIO(gpio, state) {
        var xhr = new XMLHttpRequest();
        xhr.open("GET", "/" + gpio + "/" + state, true);
        xhr.send();
      }

      function updateButtons() {
        fetch("/states")
          .then(response => response.json())
          .then(data => {
            document.getElementById("gpio26").innerHTML = data.output26State === "on" ? 
              '<button class="button button2" onclick="toggleGPIO(26, \'off\')">OFF</button>' : 
              '<button class="button" onclick="toggleGPIO(26, \'on\')">ON</button>';
            
            document.getElementById("gpio27").innerHTML = data.output27State === "on" ? 
              '<button class="button button2" onclick="toggleGPIO(27, \'off\')">OFF</button>' : 
              '<button class="button" onclick="toggleGPIO(27, \'on\')">ON</button>';
          });
      }

      setInterval(updateButtons, 1000); // Atualiza os botões a cada 1 segundo
    </script>
  </head>
  <body>
    <h1>ESP32 Web Server</h1>
    <p>GPIO 26 - State: <span id="gpio26"></span></p>
    <p>GPIO 27 - State: <span id="gpio27"></span></p>
  </body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);

  // Configura os pinos de saída
  pinMode(output26, OUTPUT);
  pinMode(output27, OUTPUT);
  digitalWrite(output26, LOW);
  digitalWrite(output27, LOW);

  // Conecta ao Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado.");
  Serial.println("Endereço IP: ");
  Serial.println(WiFi.localIP());

  // Define rota para controlar o GPIO 26
  server.on("/26/on", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(output26, HIGH);
    output26State = "on";
    request->send(200, "text/plain", "GPIO 26 ON");
  });

  server.on("/26/off", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(output26, LOW);
    output26State = "off";
    request->send(200, "text/plain", "GPIO 26 OFF");
  });

  // Define rota para controlar o GPIO 27
  server.on("/27/on", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(output27, HIGH);
    output27State = "on";
    request->send(200, "text/plain", "GPIO 27 ON");
  });

  server.on("/27/off", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(output27, LOW);
    output27State = "off";
    request->send(200, "text/plain", "GPIO 27 OFF");
  });

  // Rota para enviar o estado dos GPIOs como JSON
  server.on("/states", HTTP_GET, [](AsyncWebServerRequest *request){
    String json = "{\"output26State\":\"" + output26State + "\",\"output27State\":\"" + output27State + "\"}";
    request->send(200, "application/json", json);
  });

  // Página principal com o estado dos GPIOs
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  // Inicia o servidor
  server.begin();
}

void loop() {
  // Nada a fazer no loop
}
