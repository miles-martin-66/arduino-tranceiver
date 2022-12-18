/*
Miles Martin
Oct 2022
Receiver.ino
Reciever to collect data over BLE from a paired Arduino Nano 33 BLE and control an attached servo 
*/

#include <ArduinoBLE.h>
#include <Servo.h>

Servo myservo;  

// Use uuidgen to generate these 128 bit UUIDs
const char* sUuid = "05419122-8F7F-4CED-B015-7E3223C6452B";
const char* sCharaUuid = "5ABD848B-EB71-43AB-B0C6-85DAD033E20F";

char sensordata;
int debug = 0;

BLEService service(sUuid); 
BLEByteCharacteristic characteristic(sCharaUuid, BLERead | BLEWrite);

void setup() {
  Serial.begin(9600);

//  if ( debug ) { while (!Serial); } // If this option is chosen, Arduino will not load code unless it has a serial output (Arduino IDE)

  pinMode(LEDR, OUTPUT); pinMode(LEDG, OUTPUT); pinMode(LEDB, OUTPUT); pinMode(LED_BUILTIN, OUTPUT);
	setLedColour('r');

  if (!BLE.begin()) {
    if ( debug ) { Serial.println("- Starting Bluetooth® Low Energy module failed!"); }
    while (1);
  }

  BLE.setLocalName("Receiver");
  BLE.setAdvertisedService(service);
  service.addCharacteristic(characteristic);
  BLE.addService(service);
  characteristic.writeValue(-1);
  BLE.advertise();
  if ( debug ) { Serial.println("Receiver"); }
  myservo.attach(10);
}

void loop() {
  BLEDevice central = BLE.central();
  if ( debug ) { Serial.println("- Discovering central device..."); }
  delay(500);
  if (central) {
    setLedColour('b');
    if ( debug ) { 
      Serial.println("* Connected to central device!");
      Serial.print("* Device MAC address: ");
      Serial.println(central.address());
      Serial.println(" ");
    }
    while (central.connected()) {
      if (characteristic.written()) {
         sensordata = characteristic.value();
         setServoData(sensordata);
       }
    }
    setLedColour('r');
    if ( debug ) { Serial.println("* Disconnected from transmitter!"); }
  }
}

void setServoData(char sensordata) {
  if ( debug ) { Serial.println("- Data has been recieved: "); }
  int servoAng = sensordata - '0';
  myservo.write(servoAng);
  Serial.println( sensordata );
}

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
