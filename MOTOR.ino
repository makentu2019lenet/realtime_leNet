#include <Servo.h>

#define In1 7  //右馬達
#define In2 8
#define ENA 6
#define In3 12  //左馬達
#define In4 13
#define ENB 11
#define trigPin 4 // 超音波Pin
#define echoPin 5
#define servoPin 9  //  伺服馬達 Pin
#define servo_demo_Pin 3
unsigned long d=10;    // 前方障礙物距離
const int leftSpeed = 115 ;  //左輪轉速,經過實際測試,若左右輪轉速相同時,自走車無法走直線. P
const int rightSpeed =135;   //右輪轉速


Servo servo ,servo_demo; // 步進馬達
void setup() {
  // put your setup code here, to run once:
  //馬達腳位設定
  pinMode(In1,OUTPUT); //右馬達
  pinMode(In2,OUTPUT);
  pinMode(In3,OUTPUT); //左馬達
  pinMode(In4,OUTPUT);
  pinMode(ENA,OUTPUT);
  pinMode(ENB,OUTPUT);

 // 超音波腳位設定
  pinMode(trigPin,OUTPUT) ;
  pinMode(echoPin,INPUT) ;
 //超音波初始化
  servo.attach(servo_demo_Pin) ;
  servo.attach(servoPin) ;
  servo.write(85) ;  //因為組裝的緣故,經測試,伺服馬達在85度時方能在正前方
  delay(1000) ;
  forward();  //自走車開始前進

  Serial.begin(9600);
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);
}

// 超音波距離偵測,傳回 cm
unsigned long ping_d() {
    digitalWrite(trigPin,HIGH) ;
    delayMicroseconds(1000) ;
    digitalWrite(trigPin,LOW);
    return  (pulseIn(echoPin,HIGH)/58) ;
}

// 後退函數   ok
void backward() {
   // 右馬達
    digitalWrite(In1,HIGH);
    digitalWrite(In2,LOW);
    analogWrite(ENA,rightSpeed) ;
    // 左馬達
    digitalWrite(In3,HIGH);
    digitalWrite(In4,LOW);
    analogWrite(ENB,leftSpeed) ;
  }

// 前進函數 OK
 void forward() {
    // 右馬達
    digitalWrite(In1,LOW);
    digitalWrite(In2,HIGH);
    analogWrite(ENA,rightSpeed) ;
    // 左馬達
    digitalWrite(In3,LOW);
    digitalWrite(In4,HIGH);
    analogWrite(ENB,leftSpeed) ;
  }

 // 左轉彎函數  turnLeft ok
 void turnLeft(){
   // 右馬達 前進
    digitalWrite(In1,LOW);
    digitalWrite(In2,HIGH);
    analogWrite(ENA,rightSpeed) ;
    // 左馬達 後退
    digitalWrite(In3,HIGH);
    digitalWrite(In4,LOW);
    analogWrite(ENB,leftSpeed) ;
  }

  // 右轉彎函數  turnRight ok
 void turnRight(){
   // 右馬達 前進
    digitalWrite(In1,HIGH);
    digitalWrite(In2,LOW);
    analogWrite(ENA,rightSpeed) ;
    // 左馬達 後退
    digitalWrite(In3,LOW);
    digitalWrite(In4,HIGH);
    analogWrite(ENB,leftSpeed) ;
  }

  //停止函數 OK
  void motoStop(){
     // 右馬達 前進
    digitalWrite(In1,LOW);
    digitalWrite(In2,LOW);
    analogWrite(ENA,rightSpeed) ;
    // 左馬達 後退
    digitalWrite(In3,LOW);
    digitalWrite(In4,LOW);
    analogWrite(ENB,leftSpeed) ;
   }

void loop() {

  // print the string when a newline arrives:
  if (stringComplete) {
    Serial.println(inputString);
    // clear the string:
    inputString = "";
    stringComplete = false;
  }

 // put your main code here, to run repeatedly:
 int left_d, right_d ; //紀錄左,右邊障礙物距離

 d = ping_d() ;  //偵測前方障礙物距離

 //  如果前方30cm處有障礙物,自走車需要進入判斷模式,決定行進方式
 if( d<=30) {
    motoStop() ; //自走車停止前進
    servo.write(20) ; // 伺服馬達轉向右邊
    delay(500) ;
    right_d = ping_d() ; // 取得右邊障礙物距離
    delay(20) ;
    servo.write(150) ; // 伺服馬達轉向左邊
    delay(500) ;
    left_d = ping_d() ; // 取得左邊障礙物的距離
    servo.write(85) ;  // 轉動伺服馬達,使超音波回到正前方

    // 如果左邊空間大 且 障礙物距離超過30cm以上 ---> 左轉彎後繼續前進
    if( (left_d>right_d) && (left_d>30)) { //左邊有空間
        turnLeft() ;
        delay(350) ;
        forward() ;
     } else if( (right_d>=left_d) && (right_d>30)) { // 右邊空間大且右邊障礙物距離大於30cm以上 -->右轉彎後前進
        turnRight() ;
         delay(350) ;
         forward() ;
      } else {  // 前,左,右障礙物距離都小於30公分 --->後退->轉彎->前進
         backward() ;
         delay(1500) ;
         turnRight() ;
         delay(350) ;
         forward() ;
       }
  } // end if d<=30
   delay(30) ;
}


void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }

    int r = Serial.read() - '0'; // converting the character into integer
    if(r==1) servo_demo.write(20) ;
    if(r==2) servo_demo.write(85) ;
    if(r==3) servo_demo.write(150) ;
  }

}
