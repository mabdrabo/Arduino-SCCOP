
#include <SoftwareSerial.h>   //Software Serial Port
#include <Canbus.h>

#define BTRxD 11
#define BTTxD 9
#define CBRxD 5
#define CBTxD 4
 
#define DEBUG_ENABLED  1

char buffer[512];  //Data will be temporarily stored to this buffer before being written to the file
char tempbuf[15];
char lat_str[14];
char lon_str[14];

int LED2 = 7;
int LED3 = 8;

boolean scan = true;

SoftwareSerial canbusSerial(CBRxD, CBTxD);
SoftwareSerial blueToothSerial(BTRxD,BTTxD);
 
void setup() {
  Serial.begin(9600);
//  canbusSerial.begin(4800);
  pinMode(0, INPUT);
  pinMode(BTTxD, OUTPUT);
  pinMode(LED2, OUTPUT); 
  
  pinMode(LED3, OUTPUT); 
 
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
  
  setupCanbus();
  setupBlueToothConnection();
} 
 
void loop() { 
  char recvChar;
  if (canbusSerial.available()) {  // check if there's any data sent from the remote CANBUS shield
    recvChar = canbusSerial.read();
    Serial.print(recvChar);
  }
  if (blueToothSerial.available()) {  // check if there's any data sent from the remote bluetooth shield
    recvChar = blueToothSerial.read();
    Serial.print(recvChar);
  }
  if (scan) {
    if (Serial.available()) {  // check if there's any data sent from the local serial terminal, you can add the other applications here
      recvChar  = Serial.read();
      blueToothSerial.print(recvChar);
      blueToothSerial.print("\n");
    }
    
    digitalWrite(LED3, HIGH);
    
    if(Canbus.ecu_req(ENGINE_RPM,buffer) == 1) {         /* Request for engine RPM */
      Serial.println(buffer);                         /* Display data on Serial Monitor */
    }
    Serial.println(buffer);
   
    if(Canbus.ecu_req(VEHICLE_SPEED,buffer) == 1) {
      Serial.println(buffer);
    }Serial.println(buffer);
    
    if(Canbus.ecu_req(ENGINE_COOLANT_TEMP,buffer) == 1) {
      Serial.print(buffer);
    }Serial.println(buffer);
    
    if(Canbus.ecu_req(THROTTLE,buffer) == 1) {
      Serial.println(buffer);
    }
    Serial.println(buffer);
    //  Canbus.ecu_req(O2_VOLTAGE,buffer);
   
   digitalWrite(LED3, LOW); 
   delay(100);
    
  }
}

void setupCanbus() {
  while (!Canbus.init(CANSPEED_250)) {
    Serial.println("CAN Init");
  }
  Serial.println("CAN Init OK");
  delay(1000);
}
 
void setupBlueToothConnection() {
  blueToothSerial.begin(38400); //Set BluetoothBee BaudRate to default baud rate 38400
  blueToothSerial.print("\r\n+STWMOD=0\r\n"); //set the bluetooth work in slave mode
  blueToothSerial.print("\r\n+STNA=AEMUV-BT\r\n"); //set the bluetooth name as "SeeedBTSlave"
  blueToothSerial.print("\r\n+STOAUT=1\r\n"); // Permit Paired device to connect me
  blueToothSerial.print("\r\n+STAUTO=0\r\n"); // Auto-connection should be forbidden here
  delay(2000); // This delay is required.
  blueToothSerial.print("\r\n+INQ=1\r\n"); //make the slave bluetooth inquirable 
  Serial.println("The slave bluetooth is inquirable!");
  delay(2000); // This delay is required.
  blueToothSerial.flush();
}




