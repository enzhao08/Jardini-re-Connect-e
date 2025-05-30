#include <WiFi.h>
WiFiServer server(80);
WiFiClient client = server.available();


int nivEauHautPin = 22;
int nivEauBasPin = 23;
int nivEauHaut = 0;
int nivEauBas = 0;
int led = 13; 
boolean currentLineIsBlank = false;

void setup() {
  pinMode(CnivEau, INPUT);
  pinMode(CnivEau2, INPUT);
  pinMode(led, OUTPUT);
  Serial.begin(9600);
}

void loop() {

  int EtatDuCapteur = digitalRead(nivEauHautPin);
  if (EtatDuCapteur == HIGH) {
    digitalWrite(led, HIGH);
    Serial.println("Niveau d'eau : 1");
  } 
  else {
    digitalWrite(led, LOW);
    Serial.println("Niveau d'eau : 0");
  }

  
  if (client)                          // if you get a client
   
  {  Serial.println("New Client.");           // print a message out the serial port
    // an http request ends with a blank line
    

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
            client.print(nivEaubas);
            
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
  }               
}
 
 
 
