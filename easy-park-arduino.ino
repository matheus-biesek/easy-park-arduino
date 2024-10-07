#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define RXD2 16
#define TXD2 17

class Serial2Data {
  private:
  String messageAdm;
  bool gateOneStatus;

  public:
  Serial2Data() {
    messageAdm = "";
    gateOneStatus = false;
  }

  void setMessageAdm(const String& msg) {
    messageAdm = msg;
  }

  String getMessageAdm() const {
    return messageAdm;
  }

  void setGateOneStatus(bool status) {
    gateOneStatus = status;
  }

  bool getGateOneStatus() const {
    return gateOneStatus;
  }

  void processReceivedData(const String& receivedData) {
    if (receivedData.startsWith("messageAdm:")) {
      String msg = receivedData.substring(receivedData.indexOf(':') + 1);
      msg.trim();
      setMessageAdm(msg);
      Serial.println("Mensagem do Adm recebida: " + getMessageAdm());
    } 
    else if (receivedData.startsWith("gateOneStatus:")) {
      String status = receivedData.substring(receivedData.indexOf(':') + 1);
      status.trim();
      setGateOneStatus(status.equalsIgnoreCase("true"));
      Serial.println("Status da cancela recebido: " + String(getGateOneStatus()));
    } 
    else {
      Serial.println("Mensagem desconhecida recebida: " + receivedData);
    }
  }
};

class SensorUltrassonic {
  private:
  int trigPin;
  int echoPin;
  int ledBusy;
  int ledAvailable;
  long duration;
  float distance;
  bool isOccupied;
  const float detectionThreshold = 10.0;
  int position;

  public:
  SensorUltrassonic(int trig, int echo, int ledRed, int ledGreen, int thePosition) {
    trigPin = trig;
    echoPin = echo;
    ledBusy = ledRed;
    ledAvailable = ledGreen;
    isOccupied = false;
    position = thePosition;
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    pinMode(ledBusy, OUTPUT);
    pinMode(ledAvailable, OUTPUT);
  }

  bool getStatus() {
    return isOccupied;
  }

  float measureDistance() {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = (duration * 0.034) / 2;
    return distance;
  }

  void updateStatus() {
    float dist = measureDistance();
    if (dist <= detectionThreshold) {
      digitalWrite(ledBusy, HIGH);
      digitalWrite(ledAvailable, LOW);
      isOccupied = true;
    } else {
      digitalWrite(ledBusy, LOW);
      digitalWrite(ledAvailable, HIGH);
      isOccupied = false;
    }
  }

  void sendIsOccupiedSerial2() {
    String message;
    switch (position) {
      case 1: {
        message = "vacancyOne:";
        message += (isOccupied ? "true" : "false");
        break;
      }
      case 2: {
        message = "vacancyTwo:";
        message += (isOccupied ? "true" : "false");
        break;
      }
      default:{
        message = "";
        break;
      }
    }
    if (message != "") {
        Serial2.println(message);
    }
  }
};

class ServoGate {
  private:
  Servo servo;
  int pin;
  int openPosition;
  int closedPosition;
  bool isOpen;
  int position;
  bool status;

  public:
  ServoGate(int pin, int openPos, int closedPos, int posit) 
    : pin(pin), openPosition(openPos), closedPosition(closedPos), isOpen(false), position(posit) {}

  void initialize() {
    servo.attach(pin);
    close();
  }

  void setStatus(bool statusSend){
    status = statusSend;
  }

  void open() {
    if (!isOpen && status) {
      servo.write(openPosition);
      isOpen = true;
    }
  }

  void close() {
    if (isOpen && !status) {
      servo.write(closedPosition);
      isOpen = false;
    }
  }
};

class LCDPanel {
  private:
  LiquidCrystal_I2C lcd;
  String lastMessage;
  String message;

  public:
  LCDPanel(uint8_t lcd_Addr, int lcd_cols, int lcd_rows)
    : lcd(lcd_Addr, lcd_cols, lcd_rows), lastMessage(""), message("") {}

  void initialize() {
    lcd.init();
    lcd.begin(16, 2);
    lcd.backlight();
  }

  void setMessage(const String& newMessage) {
    message = newMessage;
  }

  void displayMessage() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(message);
  }

  void updateMessage() {
    if (message != lastMessage) {
      lastMessage = message;
      displayMessage();
    }
  }

  void clear() {
    lcd.clear();
  }

  void turnOn() {
    lcd.backlight();
  }

  void turnOff() {
    lcd.noBacklight();
  }
};

class Display7Segment {
  private:
    int pins[7];
    int lastNumber = -1;

  byte numbers[10][7] = {
    {1, 1, 1, 1, 1, 1, 0}, // 0
    {0, 1, 1, 0, 0, 0, 0}, // 1
    {1, 1, 0, 1, 1, 0, 1}, // 2
    {1, 1, 1, 1, 0, 0, 1}, // 3
    {0, 1, 1, 0, 0, 1, 1}, // 4
    {1, 0, 1, 1, 0, 1, 1}, // 5
    {1, 0, 1, 1, 1, 1, 1}, // 6
    {1, 1, 1, 0, 0, 0, 0}, // 7
    {1, 1, 1, 1, 1, 1, 1}, // 8
    {1, 1, 1, 1, 0, 1, 1}  // 9
  };

  public:
    Display7Segment(int pinA, int pinB, int pinC, int pinD, int pinE, int pinF, int pinG) {
      pins[0] = pinA;
      pins[1] = pinB;
      pins[2] = pinC;
      pins[3] = pinD;
      pins[4] = pinE;
      pins[5] = pinF;
      pins[6] = pinG;
    }

    void initialize() {
      for (int i = 0; i < 7; i++) {
        pinMode(pins[i], OUTPUT);
      }
    }

    void showNumber(int number) {
        if (number != lastNumber){
          lastNumber = number;
          if (number < 0 || number > 9) {
          Serial.println("Número inválido! Deve ser entre 0 e 9.");
          return;
        }
        for (int i = 0; i < 7; i++) {
          digitalWrite(pins[i], numbers[number][i]);
        }
      }
    }
};

Serial2Data serial2Data;

SensorUltrassonic sensorOne(13, 12, 2, 3, 1);
SensorUltrassonic sensorTwo(9, 8, 5, 6, 2);

Display7Segment displaySegmentsVacancy(22, 24, 26, 28, 30, 32, 34);

ServoGate servoGateOne(10, 90, 0, 1);

LCDPanel lcdPanelOne(0x27, 16, 2);

void setup() {
  Serial.begin(19200);  // Depuração - PC
  Serial2.begin(9600);  // Comunicação - ESP32
  servoGateOne.initialize();
  displaySegmentsVacancy.initialize();
  lcdPanelOne.initialize();
}

void loop() {
  if (Serial2.available() > 0) {
    String receivedDataSerial2 = Serial2.readStringUntil('\n');
    serial2Data.processReceivedData(receivedDataSerial2);
  }

  int totalVacancyOccupied = 0;
  SensorUltrassonic sensors[] = {sensorOne, sensorTwo};
  for (int i = 0; i < 2; i++) {
    sensors[i].updateStatus();
    sensors[i].sendIsOccupiedSerial2();
    if (!sensors[i].getStatus()){
      totalVacancyOccupied++;
    }
  }
  displaySegmentsVacancy.showNumber(totalVacancyOccupied);

  servoGateOne.setStatus(serial2Data.getGateOneStatus());
  if (serial2Data.getGateOneStatus()) {
    servoGateOne.open();
  } else {
    servoGateOne.close();
  }

  lcdPanelOne.setMessage(serial2Data.getMessageAdm());
  lcdPanelOne.updateMessage();

  delay(300);
}
