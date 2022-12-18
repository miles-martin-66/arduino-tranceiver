/*
Miles Martin
Oct 2022
Transmitter.ino
Transmitter to send data to a paired Arduino 33 BLE from an attached sensor.
*/

#include <ArduinoBLE.h>

// Use uuidgen to generate these 128 bit UUIDs
const char* sUuid = "05419122-8F7F-4CED-B015-7E3223C6452B";
const char* sCharaUuid = "5ABD848B-EB71-43AB-B0C6-85DAD033E20F";

// Controls 
float delaytime = 50;     // Number of miliseconds between data pushes
float sensormin = 0.0;    // Minimum output of the sensor
float sensormax = 1023.0; // Maximum output of the sensor
float servomin = 0.0;     // Minimum receivable servo value
float servomax = 255.00;  // Maximum recievable servo value

// Set variables
float outputval = 0.0;
int sensorpin = A0;
int sensordata = 0;

// DEBUG: Set this to 1 to print to serial window, 0 to be quiet
int debug = 0;

void setup() {
  Serial.begin(9600);

//  if ( debug ) { while (!Serial); } // If this option is chosen, Arduino will not load code unless it has a serial output (Arduino IDE)

  pinMode(LEDR, OUTPUT); pinMode(LEDG, OUTPUT); pinMode(LEDB, OUTPUT); pinMode(LED_BUILTIN, OUTPUT);
  
  setLedColour('r');
 
  if (!BLE.begin()) {
    if ( debug ) { Serial.println("* Starting Bluetooth® Low Energy module failed!"); }
    while (1);
  }
  
  BLE.setLocalName("Transmitter"); 
  BLE.advertise();
  if ( debug ) { Serial.println("Transmitter"); }
}

void loop() {
  connectToPeripheral();
}

void connectToPeripheral(){
  BLEDevice peripheral;
  
  if ( debug ) { Serial.println("- Discovering peripheral device..."); }

  do
  {
    BLE.scanForUuid(sUuid);
    peripheral = BLE.available();
    if ( debug ) { Serial.println("- Trying to connect..."); }
    setLedColour('r');
  } while (!peripheral);
  
  if (peripheral) {
    if ( debug ) {
      Serial.println("* Peripheral device found!");
      Serial.print("* Device MAC address: ");
      Serial.println(peripheral.address());
      Serial.print("* Device name: ");
      Serial.println(peripheral.localName());
      Serial.print("* Advertised service UUID: ");
      Serial.println(peripheral.advertisedServiceUuid());
    }
    BLE.stopScan();
    controlPeripheral(peripheral);
  }
}

void controlPeripheral(BLEDevice peripheral) {
  if ( debug ) { Serial.println("- Connecting to peripheral device..."); }
  if (peripheral.connect()) {
    if ( debug ) { Serial.println("* Connected to peripheral device!"); }
    setLedColour('b');
  } else {
    if ( debug ) { Serial.println("* Connection to peripheral device failed!"); }
    return;
  }
  if ( debug ) { Serial.println("- Discovering peripheral device attributes..."); }
  if (peripheral.discoverAttributes()) {
    if ( debug ) { Serial.println("* Peripheral device attributes discovered!"); }
  } else {
    if ( debug ) { Serial.println("* Peripheral device attributes discovery failed!"); }
    peripheral.disconnect();
    return;
  }

  BLECharacteristic serCharUID = peripheral.characteristic(sCharaUuid);
  
  while (peripheral.connected()) {
    sensordata = getSensorData();
    delay( delaytime );
    if ( debug ) { 
      Serial.print("* Writing sensordata: ");
      Serial.println(sensordata);
    }
    serCharUID.writeValue((byte)sensordata);
    if ( debug ) { Serial.println("* Writing value to Reciver done!"); }
  }
  if ( debug ) { Serial.println("- Peripheral device disconnected!"); }
}


// Format and send the sensor values to the Receiver
int getSensorData() {
  
  outputval = map(sensormax*log10(mymapf(analogRead(sensorpin),sensormin,sensormax,1.0,10.0)),sensormin,sensormax,servomin,servomax); 
  
  return outputval;
}

// Map function that works with floating point numbers
float mymapf(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

//  Function for setting the LED colour
void setLedColour(char colour) {
  switch (colour) {
    case 'r': // Red
	    digitalWrite(LEDR, LOW); digitalWrite(LEDG, HIGH); digitalWrite(LEDB, HIGH); digitalWrite(LED_BUILTIN, LOW);
      break;
    case 'g': // Green
      digitalWrite(LEDR, HIGH); digitalWrite(LEDG, LOW); digitalWrite(LEDB, HIGH); digitalWrite(LED_BUILTIN, LOW);
      break;
    case 'b': //Blue
    	digitalWrite(LEDR, HIGH); digitalWrite(LEDG, HIGH); digitalWrite(LEDB, LOW); digitalWrite(LED_BUILTIN, LOW);
      break;
    case 'w': // White
	    digitalWrite(LEDR, LOW); digitalWrite(LEDG, LOW); digitalWrite(LEDB, LOW); digitalWrite(LED_BUILTIN, LOW);
      break;
    default:  // Yellow
	    digitalWrite(LEDR, LOW); digitalWrite(LEDG, LOW); digitalWrite(LEDB, HIGH); digitalWrite(LED_BUILTIN, LOW);
      break;
  }
}
