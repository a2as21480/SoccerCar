#include <MotorDriver.h>
#include <EEPROM.h>

#define EN1 6
#define IN1 7
#define IN2 8

#define EN2 9
#define IN3 10
#define IN4 11
#define REVERSE 1

MotorDriver left(EN1, IN1, IN2);
MotorDriver right(EN2, IN3, IN4);

#include <SoftwareSerial.h>
#define RXD 4 //RXD에 연결(모듈의 수신부)
#define TXD 3 //TXD에 연결(모듈의 송신부)
SoftwareSerial BT(TXD,RXD);

#define DEBUG

void waitBT(unsigned int t) {
    unsigned long t0 = millis();
    while(millis() - t0 < t) {
        if(BT.available() || Serial.available() ) return 0;
    };
}

void setup() {
    Serial.begin(9600);
    BT.begin(9600);

    Serial.println("Hello, Arduino");
    Serial.println("Soccer Car");

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, 1);

    //설정값 불러오기, 0이 아니라면
    uint8_t L_max = EEPROM.read(10);
    uint8_t R_max = EEPROM.read(11);
    if(L_max * R_max) {
        left.set_limit(L_max);
        right.set_limit(R_max);
    }
    
    
}

void loop() {
    char buf = ' ';
    if(BT.available() ) {
        buf = BT.read();
        Serial.write(buf);
    }

    switch(buf) { // ''는 문자, ""는 문자열 일때 사용해야 오류를 일으키지 않는다.
        case ' ':
            break;
            
        case 'w':
            #ifdef DEBUG
            Serial.println(" << forward");
            BT.println("forward");
            #endif

            left.go(255);
            right.go(255);     
            break;
            
        case 's':
            #ifdef DEBUG
            Serial.println(" << back");
            BT.println("back");
            #endif
            
            left.go(255, REVERSE);
            right.go(255, REVERSE);
            break;
            
        case 'a':
            #ifdef DEBUG
            Serial.println(" << left");
            BT.println("left");
            #endif
            
            left.go(255, REVERSE);
            right.go(255);
            break;
            
        case 'd':
            #ifdef DEBUG
            Serial.println(" << right");
            BT.println("right");
            #endif
            
            left.go(255);
            right.go(255, REVERSE);
            break;

        case 'b': //브레이크
            #ifdef DEBUG
            Serial.println(" << break");
            BT.println("break");
            #endif

            left.stop();
            right.stop();
            break;

        case 'p': //프로그래밍 모드
            //현재 상태 출력
            uint8_t L_max = EEPROM.read(10);
            uint8_t R_max = EEPROM.read(11);
            String info = String("Current State\nL: " + String(L_max) + "\nR: " + String(R_max) );
            Serial.println(info);
            BT.println(info);

            //시작 메세지 출력
            bool settingmode =false;
            const char message[] = " << Program Setting\nChoose Motor to apply the setting 'L' or 'R'\n or put 'c' -> cancel";
            Serial.println(message);
            BT.println(message);

            //입력 대기
            while(1) {
                if(BT.available() ) {
                    buf = BT.read();
                    Serial.write(buf);
                    if(buf == 'c') break;

                    if(buf == 'L' || buf == 'R') {
                        Serial.println("Set max velocity of this motor(1~255)");
                        BT.println("Set max velocity of this motor(1~255)");
                        settingmode = true;
                        break;
                    }
                    else {settingmode = false;}

                    digitalWrite(LED_BUILTIN, 0);
                    waitBT(500);
                    digitalWrite(LED_BUILTIN, 1);
                    waitBT(500);
                }
            }

            //입력받은 명령 수행
            while(settingmode) {
                if(BT.available() ) {
                    String temp = BT.readStringUntil('\n');
                    int tempdata = atoi(temp.c_str() );
                    uint8_t data = constrain(tempdata, 1, 255);
                    Serial.println(data);

                    if(buf == 'L') {
                        left.set_limit(data);
                        EEPROM.update(10, data); //10번 인덱스에 정보 업데이트
                        settingmode = false;
                        Serial.println("Left OK " + String(data) );
                        BT.println("Left OK " + String(data) );
                        break; //while(settingmode)
                    }
                    if(buf == 'R') {
                        right.set_limit(data);
                        EEPROM.update(11, data); //11번 인덱스에 정보 업데이트
                        Serial.println("Right OK " + String(data) );
                        BT.println("Right OK " + String(data) );
                        settingmode = false;
                        break; //while(settingmode)
                    } 
               }
               //대기중 내장 LED 깜빡거리기
               digitalWrite(LED_BUILTIN, 0);
               waitBT(500);
               digitalWrite(LED_BUILTIN, 1);
               waitBT(500);
           }
           break; //switch

        default:
           left.stop();
           right.stop();
           break;
    }

    waitBT(1000);
    buf = ' ';
    left.stop();
    right.stop();
}
