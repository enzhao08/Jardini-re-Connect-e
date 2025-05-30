int Capthum = 32;
int humidite = 0;
long somme = 0 ;
int HumiditeActuel;
long mesure = long (analogRead(32)); 

void setup() {
  Serial.begin(9600);
  pinMode(32, INPUT);
  pinMode(humidite, OUTPUT);
  analogWrite(32, LOW);
  
}

void loop() {

    somme = 0;
    for (int i = 0; i < 100; i++) {
    long mesure = long (analogRead(32)); 
    somme += mesure;
    }
    long moyenne = somme / 100; 
    
    Serial.print("Moyenne des 100 mesures: ");
    Serial.println(moyenne); 
    delay(1000);


   if (moyenne <= 1000)
   {
     HumiditeActuel = 100;
   }  
   else
   {
     HumiditeActuel = map(moyenne, 1000, 4095, 0, 100);  
   }
     Serial.print("humidité = ");
     Serial.print(HumiditeActuel);
     Serial.println("%");
}
