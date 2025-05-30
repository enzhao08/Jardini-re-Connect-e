#include <WiFi.h>

const char *ssid     = "raspi-webgui";
const char *password = "ChangeMe";  //Attention 8 caractères minimum sinon le ssid est pas celui au dessus et il n'y a pas de mot de passe

// Set your Static IP address
IPAddress local_IP(192, 168, 1, 190);
// Set your Gateway IP address
IPAddress gateway(192, 168, 1, 1);

IPAddress subnet(255, 255, 255, 0);

WiFiServer server(80);

String commandLine = String(30); //chaine pour mémoriser la ligne de commande envoyée par le client (30 premier caractères de la requête http
int humidite = 0;
int chum = 0;
int phum = 0;
int thum = 0;
int CnivEau = 22;
int led = 13;
int nivEauHaut = 0;
int nivEauBas = 0;
int nivEauHautPin = 22;
int nivEauBasPin = 23; 
//******************Constantes pour la variation de luminosité de la led
#define LEDC_CHANNEL_0     0   // use first channel of 16 channels (started from zero)
#define LEDC_TIMER_13_BIT  13    // use 13 bit precission for LEDC timer
#define LEDC_BASE_FREQ     5000  // use 5000 Hz as a LEDC base frequency
#define LED_PIN            5     //La sortie pour la Led


// Arduino like analogWrite (fonction pour la variation de luminosité)
// value has to be between 0 and valueMax
void ledcAnalogWrite(uint8_t channel, uint32_t value, uint32_t valueMax = 255) {
  // calculate duty, 8191 from 2 ^ 13 - 1
  uint32_t duty = (8191 / valueMax) * min(value, valueMax);

  // write duty to LEDC
  ledcWrite(channel, duty);
}


void setup()
{
    Serial.begin(115200);
    pinMode(5, OUTPUT);      // set the LED pin mode
    pinMode(CnivEau, INPUT);
  pinMode(led, OUTPUT);
    digitalWrite(LED_PIN,HIGH); //éteindre la led au départ (led éteintre sur niveau haut)
    ledcSetup(LEDC_CHANNEL_0, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);
    //ledcAttachPin(LED_PIN, LEDC_CHANNEL_0);  //instruction placée plus loin dans le traitement des données reçu

    delay(10);

    //WiFi.mode(WIFI_STA);   //Inutile le mode wifi par défaut est le mode station (pas point d'accès)

    // Configures static IP address
    if (!WiFi.config(local_IP, gateway, subnet)) 
    {
      Serial.println("STA Failed to configure");
    }

    // Connect to Wi-Fi network with SSID and password
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) 
    {
      delay(500);
      Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    
    server.begin();
    commandLine ="";

}


void loop(){
 WiFiClient client = server.available();   // listen for incoming clients

  if (client)                          // if you get a client
   
  {  Serial.println("New Client.");           // print a message out the serial port
    // an http request ends with a blank line
    boolean currentLineIsBlank = false;

    while (client.connected())   // loop while the client's connected
    {            
      if (client.available()) // if there's bytes to read from the client,
      {             
        char c = client.read();             // read a byte, then
        if (commandLine.length() < 30) 
        {   commandLine += c;     //mémorisation des 30 premier caractères de la requête dans commandLine 
        }  
        Serial.write(c);                    // affiche la requête du client sur le moniteur
        
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank)   //Si la requête du client est terminée
        {
            nivEauHaut = digitalRead(nivEauHautPin);
            nivEauBas = digitalRead(nivEauBasPin);
            
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/plain");
            client.println("Connection: close");  // the connection will be closed after completion of the response

            client.println();  //ligne blanche pour séparer l'entête HTTP des données envoyées au Smartphone

            // the content of th
            client.print(nivEauHaut);  //de HTTP response follows the header:onnée envoyée au client
            client.print(";"); //valeur aléatoire comprise entre 0 et 99
            client.print(nivEauBas);
            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
         }

         if (c == '\n') // you're starting a new line
         {
           currentLineIsBlank = true;
         } 
    
         else if (c != '\r') // you've gotten a character on the current line
         {
           currentLineIsBlank = false;
         }
      } // fin if (client.available()
    }  //fin while (client.connected())
    
    
    delay(1);  // give the web browser time to receive the data
    client.stop(); // close the connection:

    //***************************** Traitement des données reçues en provenance du client*****************************
   
                                             //Pour allumer la Led selon la luminosité demandée
    if (commandLine.indexOf("GET /H=")>=0)   //Si la chaine  "GET /H=" est présente dans la ligne de commande
    { int debut = commandLine.indexOf("=");   //pour localiser la position du = dans la ligne de commande
      humidite = commandLine.substring(debut+1).toInt();  //extraire la sous-chaine après le = et la convertir en entier
      Serial.print("humidite : ");
      Serial.println(humidite);
      humidite = 255 - humidite;   //pour inverser le rapport cyclique (la led intégré sur la sortie 5 est allumée sur niveau bas)
      ledcAttachPin(LED_PIN, LEDC_CHANNEL_0);
      ledcAnalogWrite(LEDC_CHANNEL_0, humidite);
    }
    
    else if (commandLine.indexOf("GET /L")>=0)   //pour éteidre la led
    {  ledcDetachPin(LED_PIN);  //On est obligé de détacher la broche de la PWM Ledc sinon la led reste un peu allumée (même avec un rapport cyclique de 100%
      digitalWrite(LED_PIN,HIGH); // Pour éteindre la led ne fonctionne pas si la sortie LED_PIN n'est pas détachée de ledc
       
    } 
   
   else if (commandLine.indexOf("chum=")>=0)
    { int Debut = commandLine.indexOf("chum=");
      int Fin = commandLine.indexOf("&");
      chum = commandLine.substring(Debut+5, Fin).toInt();
      Serial.print("humidite carottes : ");
      Serial.print(chum);
      Serial.println("%");
    }
   
   else if (commandLine.indexOf("phum=")>=0)
    { int Debut = commandLine.indexOf("phum=");
      int Fin = commandLine.indexOf("&");
      phum = commandLine.substring(Debut+5, Fin).toInt();
      Serial.print("humidite patates : ");
      Serial.print(phum);
      Serial.println("%");
    }
    
    else if (commandLine.indexOf("thum=")>=0)
    { int Debut = commandLine.indexOf("thum=");
      int Fin = commandLine.indexOf("&");
      thum = commandLine.substring(Debut+5, Fin).toInt();
      Serial.print("humidite tomates : ");
      Serial.print(thum);
      Serial.println("%");
    }
    
    commandLine = "";
    } //fin if(client)
  
}
