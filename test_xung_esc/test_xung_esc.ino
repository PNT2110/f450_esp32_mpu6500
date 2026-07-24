#include <ESP32Servo.h>


const int PIN_ESC1 = 27; 
const int PIN_ESC2 = 26; 
const int PIN_ESC3 = 25; 
const int PIN_ESC4 = 33; 

 
Servo esc1; Servo esc2; Servo esc3; Servo esc4;

int selected_esc = 0;   
int current_pwm = 1000; 

void setup() {
  Serial.begin(115200);
  
  ESP32PWM::allocateTimer(0); ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2); ESP32PWM::allocateTimer(3);
  
  esc1.setPeriodHertz(50); esc2.setPeriodHertz(50);
  esc3.setPeriodHertz(50); esc4.setPeriodHertz(50);

  esc1.attach(PIN_ESC1, 1000, 2000);
  esc2.attach(PIN_ESC2, 1000, 2000);
  esc3.attach(PIN_ESC3, 1000, 2000);
  esc4.attach(PIN_ESC4, 1000, 2000);

  
  updateESCs(1000);

  Serial.println("\n=======================================================");
  Serial.println("         TOOL DÒ NGƯỠNG KHỞI ĐỘNG MOTOR (1us)          ");
  Serial.println("=======================================================");
  Serial.println("HƯỚNG DẪN SỬ DỤNG (Gõ phím vào ô trống và nhấn Enter):");
  Serial.println(" [1], [2], [3], [4] : Chọn motor muốn test độc lập.");
  Serial.println(" [0]                : Chọn điều khiển CẢ 4 motor cùng lúc.");
  Serial.println(" [w]                : Tăng +1 us (Tinh chỉnh chậm).");
  Serial.println(" [s]                : Giảm -1 us.");
  Serial.println(" [e]                : Tăng +10 us (Tăng nhanh).");
  Serial.println(" [d]                : Giảm -10 us.");
  Serial.println(" [x]                : DỪNG KHẨN CẤP (Về thẳng 1000us).");
  Serial.println("=======================================================\n");
}

void updateESCs(int pwm) {
  
  esc1.writeMicroseconds((selected_esc == 1 || selected_esc == 0) ? pwm : 1000);
  esc2.writeMicroseconds((selected_esc == 2 || selected_esc == 0) ? pwm : 1000);
  esc3.writeMicroseconds((selected_esc == 3 || selected_esc == 0) ? pwm : 1000);
  esc4.writeMicroseconds((selected_esc == 4 || selected_esc == 0) ? pwm : 1000);
}

void printStatus() {
  Serial.print("Đang điều khiển ESC: ");
  if (selected_esc == 0) Serial.print("TẤT CẢ (1,2,3,4)");
  else Serial.print(selected_esc);
  
  Serial.print("   |   Mức xung hiện tại: ");
  Serial.print(current_pwm);
  Serial.println(" us");
}

void loop() {
  if (Serial.available() > 0) {
    char c = Serial.read();
    
    
    if (c == '\n' || c == '\r' || c == ' ') return; 

    bool changed = false;

    
    if (c >= '0' && c <= '4') {
      selected_esc = c - '0';  
      current_pwm = 1000;      
      changed = true;
      Serial.println("\n>> Đã chuyển kênh điều khiển!");
    } 
    
    else if (c == 'w' || c == 'W') { current_pwm += 1;  changed = true; }
    else if (c == 's' || c == 'S') { current_pwm -= 1;  changed = true; }
    else if (c == 'e' || c == 'E') { current_pwm += 10; changed = true; }
    else if (c == 'd' || c == 'D') { current_pwm -= 10; changed = true; }
    else if (c == 'x' || c == 'X') { current_pwm = 1000; changed = true; Serial.println("\n>> ĐÃ DỪNG KHẨN CẤP!"); }

    if (changed) {
      
      current_pwm = constrain(current_pwm, 1000, 2000);
      
      updateESCs(current_pwm);
      printStatus();
    }
  }
}
