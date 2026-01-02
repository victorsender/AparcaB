// Configura els pins trigger i eco del HCR-SR04
const int trigPin = 12;
const int echoPin = 13;

// Assignem un pin al brunzidor 
const int buzzer  = 2;

// Assigna els 3 pins del LED RGB
const int vermell = 9; 
const int verd    = 10;
const int blau    = 11;

// Variables globals
int duration, distance;
char modeActual = '0'; // Variable per guardar l'estat ('s' o 'c'). Comença aturat ('0')

void setup() {
  pinMode(trigPin, OUTPUT); // El pin Trigger envia el senyal (sortida)
  pinMode(echoPin, INPUT);  // El pin Echo rep el senyal (entrada)
  pinMode(buzzer, OUTPUT); // El pin Buzzer emet un so

  pinMode(vermell, OUTPUT); // Pin per al color vermell (sortida)
  pinMode(verd, OUTPUT);  // Pin per al color verd (sortida)
  pinMode(blau, OUTPUT); // Pin per al color blau (sortida)

  Serial.begin(9600); // Obre el port sèrie a 9600 bps
  while (!Serial);  // Espera a que el monitor sèrie estigui llest 
  
  Serial.println("--- MENU ---");
  Serial.println("Escriu 's' per iniciar el SENSOR DE APARCAMENT");
  Serial.println("Escriu 'c' per iniciar el mode CALIBRACIÓ");
}

void loop() {
  
  if (Serial.available() > 0) {
    char r = Serial.read();
    
    // Si rebem 's' o 'S', canviem al mode Sensor
    if (r == 's' || r == 'S') {
      modeActual = 's';
      Serial.println("Mode SENSOR activat.");
    }
    // Si rebem 'c' o 'C', canviem al mode Calibració
    else if (r == 'c' || r == 'C') {
      modeActual = 'c';
      Serial.println("Mode CALIBRACIO activat.");
      // Apaguem LEDs i buzzer al canviar a calibració
      setColor(0,0,0);
      noTone(buzzer);
    }
  }

  // OPCIÓ S: SENSOR DE APARCAMENT COMPLET 
  if (modeActual == 's') {
    mesurardurada(); // Cridem a la funció que mesura les distàncies
    distance = (duration * 0.0343) / 2;
    // Lógica visual i sonora del sensor
     int delayTime; // Creem una variable on guardarem el temps de pausa del 'pip'
     int toneFreq;   // Variable per guardar la freqüència (to) del 'pip'

  // CAS 1: Objecte massa lluny (+ de 100 cm) o error: LED RGB en blau i cap soroll
  if (distance > 100 || distance == 0) {
    setColor(0, 0, 255); // Posa el LED de color Blau
    noTone(buzzer);// Assegura que el buzzer estigui en silenci
    delay(200); // Fem una petita pausa
     return; // Surt del 'loop' i torna a començar des de dalt
  } 
  // CAS 2: Objecte a distància intermitja (entre 50 i 100 cm): LED RGB en verd i brunzit petit
  else if (distance > 50) {
    setColor(0, 255, 0); // Posa el LED de color Verd
    delayTime = 500;  // Fem una pausa més llarga (pip lent)
    toneFreq = 1700; // Posem un to baix que indiqui poca "urgència"
  } 
  // CAS 3: Objecte a distància petita (entre 30 i 50 cm): LED RGB en groc i brunzit mitjà
  else if (distance > 30) {
    setColor(255, 100, 0); // Posa el LED de color Groc
    delayTime = 300; // Fem pausa mitjana (pip mitjà)
    toneFreq = 1900; // Posem un to mig
  } 
  // CAS 4: Objecte prop (entre 15 i 30 cm): LED RGB en taronja i brunzit ràpid
  else if (distance > 15) {
    setColor(255, 30, 0); // Posa el LED de color taronja
    delayTime = 150; // Fem pausa curta (pip ràpid)
    toneFreq = 2000; // Posem un to agut
  } 
    // CAS 5: Objecte molt a prop (entre 0 i 15 cm): LED RGB en vermell i brunzit molt ràpid
  else {
    setColor(255, 0, 0); // Posa el LED de color vermell
    delayTime = 50; // Fem pausa molt curta ('pip' gairebé continu)
    toneFreq = 2200; // Posem un to molt agut
  }
    
    // So i visualització de dades
    Serial.print("Sensor (S): ");
    Serial.println(distance); // Imprimeix la distància calculada i fa un salt de línia
    Serial.println(" cm");
    
   // Funcionament del brunzidor
  tone(buzzer, toneFreq); // Encen el buzzer amb la freqüència decidida
  delay(delayTime / 2); // Deixa que soni durant la meitat del temps
  noTone(buzzer); // Apaga el buzzer (silenci)
  delay(delayTime / 2); // Espera en silenci durant l'altra meitat
}

  // OPCIÓ C: CALIBRACIÓ (Només mostra les dades emprades per calibrar ,sense llums ni so) 
  else if (modeActual == 'c') {
    mesurardurada(); // Cridem la mateixa funció de mesura

    // En mode calibració, imprimim dades més ràpid i precís sense fer soroll
    Serial.print("Durada: ");
    Serial.println(duration);   
    setColor(255, 255, 255); // Posem el LED Blanc fix per indicar mode calibració
    delay(100); // Actualització ràpida
  }
}

// Definim una funció auxiliar per mesurar (per no repetir codi a S i C)
void mesurardurada() {
  digitalWrite(trigPin, LOW); // Assegura que el trigger estigui apagat
  delayMicroseconds(2); // Es produeix un delay de 2 microsegons
  digitalWrite(trigPin, HIGH); // Activem el trigger
  delayMicroseconds(10); // Es manté aquest pol durant 10 microsegons
  digitalWrite(trigPin, LOW); // Desactivem el trigger

  duration = pulseIn(echoPin, HIGH, 30000); // En aquest apartat pulseIn mesura el temps en que l'echo està High (encès). Els 30000 microsegons fan referència al timeout del pin echo per no quedar-se bloquejat 
}

// Funcionament del LED RGB
void setColor(int r, int g, int b) {
  analogWrite(vermell, r); // Envia un senyal PWM al pin vermell
  analogWrite(verd, g); // Envia un senyal PWM al pin verd
  analogWrite(blau, b); // Envia un senyal PWM al pin blau
}