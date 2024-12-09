
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"
#include <Wire.h>
#include <MPU6050.h>
#include <SoftPWM.h>

#define TO_DEG 57.2957f
#define TIME_OUT 10
#define GRAVITY_SCALE 4096.0

MPU6050 accgyro;
int accuracy =45; // Точность
float anglex;
float angley;
long int t1;

RF24 radio(9, 10); //Arduino UNO
const uint8_t channel = 0x60; // Канал, на котором работает передатчик
const int num_reps = 100; // Число измерений для усреднения активности
const uint32_t address = 77777;
void setup() {
  Serial.begin(9600);

  printf_begin();
  radio.begin();
  radio.setChannel(channel); // Устанавливаем канал
  radio.setAutoAck(false); // Отключаем авто-подтверждение
  radio.setDataRate(RF24_250KBPS); // скорость обмена данными RF24_1MBPS или RF24_2MBPS
  radio.setCRCLength(RF24_CRC_8); // размер контрольной суммы 8 bit или 16 bit
  radio.setPALevel(RF24_PA_MIN);
  radio.openWritingPipe(address);

  radio.powerUp();
  radio.stopListening();
  radio.printDetails();  
  delay(1000);   

  accgyro.initialize();
  // Отправка данных

}

int direct[2] = {0,0};
int count = 0;
bool test = false;
void loop() {
  if (test) {
  direct[0] = count;
  if (count == 10) {
    count = 0;
  }
  radio.write(&direct, sizeof(direct));
  Serial.println("Sended");
  count++;
  delay(5000);
  return;
  }
  long int t = millis();
  if (t1 < t) {
    int16_t ax, ay, az, gx, gy, gz;
    float accx, accy, gyrox;
    t1 = t + TIME_OUT;
    accgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    // Преобразование в единицы g
    accx = constrain(ax / GRAVITY_SCALE, -1.0, 1.0);
    accy = constrain(ay / GRAVITY_SCALE, -1.0, 1.0);

    // Получение углов
    anglex = (accy >= 0) ? (90 - TO_DEG * acos(accy))
                         : (TO_DEG * acos(-accy) - 90);
    angley = (accx >= 0) ? (90 - TO_DEG * acos(accx))
                         : (TO_DEG * acos(-accx) - 90);

    int x = angley;
    int y = anglex;

    if (abs(x) < accuracy && abs(y) < accuracy) {
        direct[0] = 0; // STOP
    }
    else if (y > accuracy) { //вперед 
      if(x > accuracy)
        direct[0] = 4;//вправо
      else if(x < - accuracy)
        direct[0] = 5; //влево
      else
        direct[0] = 3;
    }
    else if (y < -accuracy) { // назад
      if(x > accuracy)
        direct[0] = 7;//вправо
      else if(x < - accuracy)
        direct[0] = 8; //влево
      else
      direct[0] = 6;
    }
    else {
      if(x > accuracy)
        direct[0] = 1;//вправо
      else 
        direct[0] = 2; //влево
    }
    int speed = max(abs(x), abs(y));

    if (speed > accuracy && direct[0] != 0) {
        direct[1] = map(speed, accuracy, 90, 155, 255);
    } else {
        direct[1] = 0;
    }
    
    radio.write(&direct, sizeof(direct));
  }
}

