#include <SoftwareSerial.h>
#define RXD 4 //RXD에 연결(모듈의 수신부)
#define TXD 3 //TXD에 연결(모듈의 송신부)
SoftwareSerial BT(TXD,RXD);

void setup() {
    Serial.begin(9600);
    BT.begin(9600);
    
    BT.println("AT+BAUD4"); //보트레이트를 9600으로 설정, 
    BT.println("AT+NAMEHJ Arduino "); //블루투스 장치 이름 설정
    BT.println("AT+PIN1234"); //블루투스 장치 비번 설정

    Serial.println("hello");
/*
    pinMode(LED_BUILTIN, OUTPUT);

    for(int i=0; i<3; i++) {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(500);
        digitalWrite(LED_BUILTIN, LOW);
        delay(500);
    }
*/

}

void loop() {
    if(BT.available() ) {
        Serial.write(BT.read() );
    }

    if(Serial.available() ) {
        BT.write(Serial.read() );
    }
    
}
