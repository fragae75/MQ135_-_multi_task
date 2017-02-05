/*
 * mesure dehors : 23 <=> Aube = 10
 *                39 <=> 75
 */
int sensorValue;
int digitSensorValue;
int PIN_D0 = 7;
int PIN_LED_VERT = 8;
int PIN_LED_ORANGE = 9;
int PIN_LED_ROUGE = 10;
int PIN_LED_ROUGE_ROUGE = 11;
int SEUIL_VERT = 50;
int SEUIL_ORANGE = 100;
int SEUIL_ROUGE = 150;
int SEUIL_ROUGE_ROUGE = 250;


class Flasher
{
  // Class Member Variables
  // These are initialized at startup
  public:
    int ledPin;      // the number of the LED pin
    long OnTime;     // milliseconds of on-time
    long OffTime;    // milliseconds of off-time
   
    // These maintain the current state
    int ledState;                 // ledState used to set the LED
    unsigned long previousMillis;   // will store last time LED was updated
 
  public:
    Flasher(int pin, int lState, long on, long off);
    void Update(long on, long off);
    void Update();
};


 
// Constructor - creates a Flasher 
// and initializes the member variables and state
Flasher::Flasher(int pin, int lState, long on, long off)
{
  ledPin = pin;
  pinMode(ledPin, OUTPUT);     
    
  OnTime = on;
  OffTime = off;
  
  ledState = lState; 
  previousMillis = millis();
}
 
void Flasher::Update()
{
    // check to see if it's time to change the state of the LED
    unsigned long currentMillis = millis();
     
    if((ledState == HIGH) && (currentMillis - previousMillis >= OffTime))
    {
      ledState = LOW;  // Turn it on
      previousMillis = currentMillis;  // Remember the time
      digitalWrite(ledPin, ledState);  // Update the actual LED
    }
    else if ((ledState == LOW) && (currentMillis - previousMillis >= OnTime))
    {
      ledState = HIGH;  // turn it off
      previousMillis = currentMillis;   // Remember the time
      digitalWrite(ledPin, ledState);   // Update the actual LED
    }
}

void Flasher::Update(long on, long off)
{
  OnTime = on;
  OffTime = off;
  Update();
}


class SensorReadMQ135
{
  // Class Member Variables
  // These are initialized at startup
  public:
    int readPin;      // the number of the LED pin
    int readPeriod;   
    int currentValue;

    // These maintain the current state
    unsigned long previousMillis;   // will store last time LED was updated
 
  public:
    SensorReadMQ135(int pin, long period);
    int Read();
};

SensorReadMQ135::SensorReadMQ135(int pin, long period)
{
  readPin = pin;
  readPeriod = period;
  currentValue = 0;
  previousMillis = millis();
}

int SensorReadMQ135::Read()
{
  // check to see if it's time to change the state of the LED
  unsigned long currentMillis = millis();

//  Serial.print(" - ");  
//  Serial.println(currentMillis);  
   
  if(currentMillis - previousMillis >= readPeriod)
  {
    previousMillis = currentMillis;   // Remember the time
    currentValue = analogRead(readPin);       // read analog input pin 0

    Serial.print(currentMillis);  
    Serial.print(" - Analog : ");  
    Serial.println(currentValue , DEC);  // prints the value read
    if (currentValue >SEUIL_ROUGE_ROUGE)
    {
      digitalWrite(PIN_LED_ROUGE_ROUGE, HIGH);   
      digitalWrite(PIN_LED_ROUGE, HIGH);   
      digitalWrite(PIN_LED_ORANGE, HIGH);   
      digitalWrite(PIN_LED_VERT, HIGH);   
    } 
    else if (currentValue >SEUIL_ROUGE)
    {
      digitalWrite(PIN_LED_ROUGE_ROUGE, LOW);   
      digitalWrite(PIN_LED_ROUGE, HIGH);   
      digitalWrite(PIN_LED_ORANGE, HIGH);   
      digitalWrite(PIN_LED_VERT, HIGH);   
    } 
    else if (currentValue >SEUIL_ORANGE)
    {
      digitalWrite(PIN_LED_ROUGE_ROUGE, LOW);   
      digitalWrite(PIN_LED_ROUGE, LOW);   
      digitalWrite(PIN_LED_ORANGE, HIGH);   
      digitalWrite(PIN_LED_VERT, HIGH);   
    } 
    else if (currentValue >SEUIL_VERT)
    {
      digitalWrite(PIN_LED_ROUGE_ROUGE, LOW);   
      digitalWrite(PIN_LED_ROUGE, LOW);   
      digitalWrite(PIN_LED_ORANGE, LOW);   
      digitalWrite(PIN_LED_VERT, HIGH);   
    } 
    else //(currentValue <= 50)
    {
      digitalWrite(PIN_LED_ROUGE_ROUGE, LOW);   
      digitalWrite(PIN_LED_ROUGE, LOW);   
      digitalWrite(PIN_LED_ORANGE, LOW);   
      digitalWrite(PIN_LED_VERT, LOW);   
    } 

  }
  return(currentValue);
}
  
Flasher ledMQ135(PIN_D0, LOW, 200, 200);
SensorReadMQ135 sensorMQ135(0, 500);

void setup()
{
  // Timer0 is already used for millis() - we'll just interrupt somewhere
  // in the middle and call the "Compare A" function below
  OCR0A = 0xAF; // Output Compare Register A = 1010 1111, 175
  TIMSK0 |= _BV(OCIE0A); // TIMSK0 – Timer/Counter Interrupt Mask Register - enable timer compare interrupt - TIMSK0: 3 _BV(OCIE0A): 2
  
  Serial.begin(9600);      // sets the serial port to 9600
  Serial.print("** "); Serial.print(OCR0A, HEX); Serial.print(" TIMSK0: ");Serial.print(TIMSK0, HEX);Serial.print(" _BV(OCIE0A): ");Serial.println(_BV(OCIE0A), HEX);
//  pinMode(PIN_D0,OUTPUT);
  pinMode(PIN_LED_VERT,OUTPUT);
  pinMode(PIN_LED_ORANGE,OUTPUT);
  pinMode(PIN_LED_ROUGE,OUTPUT);
  pinMode(PIN_LED_ROUGE_ROUGE,OUTPUT);

}

// Interrupt is called once a millisecond, 
SIGNAL(TIMER0_COMPA_vect) 
{
  sensorValue = sensorMQ135.Read();
  
  if (sensorValue>SEUIL_ROUGE_ROUGE)
    ledMQ135.Update(100, 100);
  else if (sensorValue>SEUIL_ROUGE)
    ledMQ135.Update(300, 300);
  else if (sensorValue>SEUIL_ORANGE)
    ledMQ135.Update(500, 500);
  else if (sensorValue>SEUIL_VERT)
    ledMQ135.Update(1000, 1000);
  else //(sensorValue<= 50)
    ledMQ135.Update(1000, 10000000);
} 

void loop()
{
}
