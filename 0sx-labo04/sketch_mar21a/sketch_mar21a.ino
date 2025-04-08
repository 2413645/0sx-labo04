#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <AccelStepper.h>
#include <HCSR04.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define MOTOR_INTERFACE_TYPE 4
#define IN_1 31
#define IN_2 33
#define IN_3 35
#define IN_4 37

AccelStepper myStepper(MOTOR_INTERFACE_TYPE, IN_1, IN_3, IN_2, IN_4);

#define TRIG_PIN 6
#define ECHO_PIN 7

HCSR04 hc(TRIG_PIN, ECHO_PIN);

unsigned long previousMillisDist = 0;
unsigned long previousMillisSerial = 0;
unsigned long previousMillisStart = 0;
const long intervalDist = 50;
const long intervalSerial = 100;
const long startDelay = 2000;
int distanceCM = 0;
int targetAngle = 90;
bool initDone = false;
int minStep = (10*2038*1.0)/360;
const int maxStep = 962.38;


void setup() {
    Serial.begin(115200);
    lcd.init();
    lcd.backlight();
    myStepper.setMaxSpeed(1000);
    myStepper.setAcceleration(200);
    
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);

    previousMillisStart = millis();
    afficherIntro();
    //lcd.clear();
}

void loop() {
    unsigned long currentMillis = millis();
    
    if (!initDone) {
        if (currentMillis - previousMillisStart >= startDelay) {
            initDone = true;
            lcd.clear();
        }
        return;
    }

    if (currentMillis - previousMillisDist >= intervalDist) {
        previousMillisDist = currentMillis;
        distanceCM = mesurerDistance();
        targetAngle = calculerAngle(distanceCM);
        afficherLCD(distanceCM, targetAngle);
        deplacerMoteur(targetAngle);
    }

    if (currentMillis - previousMillisSerial >= intervalSerial) {
        previousMillisSerial = currentMillis;
        Serial.print("etd:2413645,dist:");
        Serial.print(distanceCM);
        Serial.print(",deg:");
        Serial.println(targetAngle);
    }

    myStepper.run();
}

void afficherIntro() {
    lcd.setCursor(0, 0);
    lcd.print("2413645");
    lcd.setCursor(0, 1);
    lcd.print("Labo 4B");
}

int mesurerDistance() {
   
    
    
    int distance =hc.dist(); 
    return distance;
}



int calculerAngle(int distance) {
    if (distance < 30) return -1;
    if (distance > 60) return -2;
    return map(distance, 30, 60, 10, 170);
}


void afficherLCD(int distance, int angle) {
    lcd.setCursor(0, 0);
    lcd.print("Dist : ");
    lcd.print(distance);
    lcd.print(" cm    ");
    
    lcd.setCursor(0, 1);
    if (angle == -1) {
        lcd.print("Obj : Trop pret");
    } else if (angle == -2) {
        lcd.print("Obj : Trop loin");
    } else {
        lcd.print("Obj : ");
        lcd.print(angle);
        lcd.print(" deg     ");
    }
}

void deplacerMoteur(int angle) {
    if (angle >= 10 && angle <= 170) {
        int steps = map(angle, 10, 170, minStep, maxStep);
        if(myStepper.distanceToGo()==0){
           
            myStepper.moveTo(steps);
        }    
    }
}