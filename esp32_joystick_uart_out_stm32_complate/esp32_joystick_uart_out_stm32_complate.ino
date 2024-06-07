TaskHandle_t Task1;
TaskHandle_t Task2;

#include <HardwareSerial.h>
HardwareSerial SerialPort(2);

#include <EEPROM.h>
#define EEPROM_SIZE 32

char string[32] = "saya";
char macAddress[32];
byte flag = 0;
char strclear[1] = "";
int buffer = 0;

int rx;
volatile byte tx[12] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 11, 12 };
char macadd[] = "00:00:00:00:00:00";



#include <Ps3Controller.h>
int cro, squ, tri, cir;
int xl, yl, xr, yr;
byte event1, event2;
int str, slc, l1, l2, l3, r1, r2, r3, up, down, left, right, bat;

unsigned long interval = 4000;  // the time we need to wait
unsigned long previousMillis = 0, currentMillis;

void notify() {
  if (Ps3.data.button.cross) cro = 1;
  else cro = 0;

  if (Ps3.data.button.square) squ = 1;
  else squ = 0;

  if (Ps3.data.button.triangle) tri = 1;
  else tri = 0;

  if (Ps3.data.button.circle) cir = 1;
  else cir = 0;

  xl = Ps3.data.analog.stick.lx;
  yl = Ps3.data.analog.stick.ly;
  xr = Ps3.data.analog.stick.rx;
  yr = Ps3.data.analog.stick.ry;

  up = Ps3.data.button.up;
  down = Ps3.data.button.down;
  left = Ps3.data.button.left;
  right = Ps3.data.button.right;


  // event1 = Ps3.event.analog_changed;
  // event2 = Ps3.event.button_down;
  bat = Ps3.data.status.battery;

  l1 = Ps3.data.button.l1;
  l2 = Ps3.data.button.l2;
  l3 = Ps3.data.button.l3;
  r1 = Ps3.data.button.r1;
  r2 = Ps3.data.button.r2;
  r3 = Ps3.data.button.r3;

  str = Ps3.data.button.start;
  slc = Ps3.data.button.select;
}




void setup() {
  pinMode(34, INPUT_PULLUP);

  //create a task that will be executed in the Task1code() function, with priority 1 and executed on core 0
  xTaskCreatePinnedToCore(
    Task1code,        /* Task function. */
    "Task1",          /* name of task. */
    10000,            /* Stack size of task */
    NULL,             /* parameter of the task */
    tskIDLE_PRIORITY, /* priority of the task */
    &Task1,           /* Task handle to keep track of created task */
    0);               /* pin task to core 0 */


  //create a task that will be executed in the Task2code() function, with priority 1 and executed on core 1
  xTaskCreatePinnedToCore(
    Task2code, /* Task function. */
    "Task2",   /* name of task. */
    10000,     /* Stack size of task */
    NULL,      /* parameter of the task */
    1,         /* priority of the task */
    &Task2,    /* Task handle to keep track of created task */
    1);        /* pin task to core 1 */
}

//Task1code: blinks an LED every 1000 ms
void Task1code(void* pvParameters) {
  SerialPort.begin(2000000, SERIAL_8N1, 16, 17);
  while (1) {
    //tx += 1;
    // put your main code here, to run repeatedly:
    // SerialPort.write(tx[0]);
    if (SerialPort.available()) {
      rx = SerialPort.read();
      if (rx == 6) {
        SerialPort.write(55);
        SerialPort.write(66);
        SerialPort.write(88);
        SerialPort.write(tri);
        SerialPort.write(squ);
        SerialPort.write(cir);
        SerialPort.write(cro);
        SerialPort.write(r1);
        SerialPort.write(r2);
        SerialPort.write(r3);
        SerialPort.write(l1);
        SerialPort.write(l2);
        SerialPort.write(l3);
        SerialPort.write(str);
        SerialPort.write(slc);
        SerialPort.write(right);
        SerialPort.write(up);
        SerialPort.write(down);
        SerialPort.write(left);
        SerialPort.write(xl);
        SerialPort.write(yl);
        SerialPort.write(xr);
        SerialPort.write(yr);
        SerialPort.write(bat);
        rx=0;
      }
      //tx[11]+=1;
    }
  }
}

//Task2code: blinks an LED every 700 ms
void Task2code(void* pvParameters) {
  Serial.begin(115200);
  EEPROM.begin(EEPROM_SIZE);
  Ps3.attach(notify);
          for (int i = 0; i < 17; i++) {
            macAddress[i] = char(EEPROM.read(i));
          }
  Ps3.begin(macAddress);  //tombol up rusak
                      // Ps3.begin("00:1a:7d:da:71:10");
  while (1) {

    if (Serial.available()) buffer = Serial.read();

    if (buffer == 57) {  ///////9
      Serial.println("====================================================");
      Serial.println("PS3 Controller Host Setting :");
      Serial.println("1 to set mac address");
      Serial.println("2 to set serial output all data from controller");
      Serial.println("3 setting baud rate output serial");
      Serial.println("====================================================");
      for (int i = 0; i < 32; i++) {
        string[i] = '\0';
        flag = 1;
      }
      Serial.println(string);
      // Serial.println(macAddress);
      buffer = 0;
    }
    if (buffer == 49 && flag == 1) {  /////////1
      Serial.println("Input MAC address :");
      buffer = 0;
      while (1) {
        if (Serial.available()) {
          int availableBytes = Serial.available();
          if (availableBytes == 1 && Serial.read() == 57) {
            buffer = 57;
            break;
          }
          for (int i = 0; i < availableBytes; i++) {
            string[i] = Serial.read();
          }
          // Serial.println(string);
          if (strlen(string) < 17) Serial.println("wrong input !");
        }

        if (strlen(string) >= 17) {
          for (int i = 0; i < 17; i++) {
            EEPROM.write(i, string[i]);
          }
          EEPROM.commit();

          for (int i = 0; i < 17; i++) {
            macAddress[i] = char(EEPROM.read(i));
          }
          Serial.println(macAddress);
          Serial.println("MAC set !");
          break;
        }
      }
      flag = 0;
    }
    if (buffer == 50 && flag == 1) {  ///////////2
      Serial.println("Serial set");
      buffer = 0;
      flag = 0;
    }
    if (buffer == 51 && flag == 1) {  //////////3
      Serial.println("Serial set");
      buffer = 0;
      flag = 0;
    }
  }


  //
  while (1) {

    // Serial.print(tri);
    // Serial.print(" ");
    // Serial.print(squ);
    // Serial.print(" ");
    // Serial.print(cir);
    // Serial.print(" ");
    // Serial.print(cro);
    // Serial.print(" ");
    // Serial.print(r1);
    // Serial.print(" ");
    // Serial.print(r2);
    // Serial.print(" ");
    // Serial.print(r3);
    // Serial.print(" ");
    // Serial.print(l1);
    // Serial.print(" ");
    // Serial.print(l2);
    // Serial.print(" ");
    // Serial.print(l3);
    // Serial.print(" ");
    // Serial.print(str);
    // Serial.print(" ");
    // Serial.print(slc);
    // Serial.print(" ");
    // Serial.print(bat);
    // Serial.print(" ");
    // Serial.print(right);
    // Serial.print(" ");
    // Serial.print(up);
    // Serial.print(" ");
    // Serial.print(down);
    // Serial.print(" ");
    // Serial.print(left);
    // Serial.print(" ");
    // Serial.print(xl);
    // Serial.print(" ");
    // Serial.print(yl);
    // Serial.print(" ");
    // Serial.print(xr);
    // Serial.print(" ");
    // Serial.println(yr);
  }
}

void loop() {
  delay(1);
}