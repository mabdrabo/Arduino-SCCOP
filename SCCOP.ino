
#include <SoftwareSerial.h>   //Software Serial Port
#include <Canbus.h>

#define BTRxD 9
#define BTTxD 6
#define CBRxD 4
#define CBTxD 5
 
#define DEBUG_ENABLED  1

char buffer[512];  //Data will be stored to this buffer
char tempbuf[15];
char lat_str[14];
char lon_str[14];

int LED2 = 7;
int LED3 = 8;

boolean scan = true;
int ecu_reqs[5] = {ENGINE_RPM,VEHICLE_SPEED, ENGINE_COOLANT_TEMP, THROTTLE};

SoftwareSerial canbusSerial(CBRxD, CBTxD);
SoftwareSerial blueToothSerial(BTRxD,BTTxD);
 
void setup() {
  Serial.begin(9600);

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
    Serial.print("CAN: "+recvChar);
  }
  if (blueToothSerial.available()) {  // check if there's any data sent from the remote bluetooth shield
    recvChar = blueToothSerial.read();
    Serial.print("BT: "+recvChar);
  }
  if (scan) {
    String s = "";
    while (Serial.available()) {  // check if there's any data sent from the local serial terminal, you can add the other applications here
    s += (char) Serial.read();
//      blueToothSerial.println("0=56");
    }
    blueToothSerial.println(s);
    s = "";
    
    digitalWrite(LED3, HIGH);
    
    for (int i=0; i<sizeof(ecu_reqs); i++) {
      if(Canbus.ecu_req(ecu_reqs[i],buffer) == 1) {         /* Request */
        String data = i + strcat("#", buffer);
        Serial.println(data);                         /* Display data on Serial Monitor */
        blueToothSerial.println(data);                         /* Send data via Bluetooth */        
      } else {
//        Serial.println(i+"#23");
//        blueToothSerial.println("m");                         /* Send data via Bluetooth */
      }
    }
    
    digitalWrite(LED3, LOW); 
    delay(100);
    
  }
}

void setupCanbus() {
  canbusSerial.begin(4800);
  while (!Canbus.init(CANSPEED_500)) {
    Serial.println("CAN Init");
  }
  Serial.println("CAN Init OK");
  delay(1000);
}
 
void setupBlueToothConnection() {
  blueToothSerial.begin(38400); //Set BluetoothBee BaudRate to default baud rate 38400
  blueToothSerial.print("\r\n+STWMOD=0\r\n"); //set the bluetooth work in slave mode
  blueToothSerial.print("\r\n+STNA=SCCOP-BT\r\n"); //set the bluetooth name as "SeeedBTSlave"
  blueToothSerial.print("\r\n+STOAUT=1\r\n"); // Permit Paired device to connect me
  blueToothSerial.print("\r\n+STAUTO=0\r\n"); // Auto-connection should be forbidden here
  delay(2000); // This delay is required.
  blueToothSerial.print("\r\n+INQ=1\r\n"); //make the slave bluetooth inquirable 
  Serial.println("The slave bluetooth is inquirable!");
  delay(2000); // This delay is required.
  blueToothSerial.flush();
}




