#include <DabbleESP32.h>

// Định nghĩa các chân điều khiển động cơ
#define MOTOR_AIN1 26
#define MOTOR_AIN2 25
#define MOTOR_PWMA 33
#define MOTOR_BIN1 14
#define MOTOR_BIN2 12
#define MOTOR_PWMB 13
#define MOTOR_STBY 27

// Định nghĩa các nút điều khiển từ Gamepad trong Dabble
#define UP_BIT 0
#define DOWN_BIT 1
#define LEFT_BIT 2
#define RIGHT_BIT 3

// Định nghĩa các kênh PWM
#define PWM_CHANNEL_A 0
#define PWM_CHANNEL_B 1

// Cài đặt thông số
int motorASpeed = 0; // Tốc độ motor A (0 - 255)
int motorBSpeed = 0; // Tốc độ motor B (0 - 255)
unsigned long lastReceivedTime = 0; // Thời gian cuối nhận được dữ liệu
const unsigned long timeout = 5000; // Thời gian chờ (5 giây)

bool checkBluetoothConnection();
void controlMotors();
void stopMotors();
void moveMotors(int speedA, int speedB, bool dirA, bool dirB);
void setMotorDirection(bool dirA, bool dirB);

void setup() {
  // Khởi tạo các chân điều khiển động cơ
  pinMode(MOTOR_AIN1, OUTPUT);
  pinMode(MOTOR_AIN2, OUTPUT);
  pinMode(MOTOR_BIN1, OUTPUT);
  pinMode(MOTOR_BIN2, OUTPUT);
  pinMode(MOTOR_STBY, OUTPUT);

  // Cấu hình PWM
  ledcSetup(PWM_CHANNEL_A, 5000, 8); // Tần số 5kHz, độ phân giải 8-bit
  ledcSetup(PWM_CHANNEL_B, 5000, 8);
  ledcAttachPin(MOTOR_PWMA, PWM_CHANNEL_A);
  ledcAttachPin(MOTOR_PWMB, PWM_CHANNEL_B);

  // Kích hoạt chế độ Standby cho TB6612
  digitalWrite(MOTOR_STBY, HIGH);

  // Khởi tạo kết nối Bluetooth với Dabble
  Dabble.begin("ESP32_Motor_Control");
  Serial.begin(115200);
  Serial.println("ESP32 is ready to control the motors via Dabble Gamepad");
}

void loop() {
  Dabble.processInput(); // Xử lý dữ liệu từ ứng dụng Dabble

  // Kiểm tra kết nối Bluetooth
  if (checkBluetoothConnection()) {
    controlMotors(); // Điều khiển động cơ dựa trên lệnh từ Gamepad
  } else {
    stopMotors(); // Dừng động cơ khi mất kết nối
  }
}

// Kiểm tra trạng thái kết nối Bluetooth
bool checkBluetoothConnection() {
  if (Dabble.isAppConnected()) {
    lastReceivedTime = millis();
    return true;
  } else if (millis() - lastReceivedTime > timeout) {
    Serial.println("Bluetooth connection lost. Waiting for reconnection...");
    return false;
  }
  return true;
}

// Điều khiển động cơ dựa trên lệnh từ Gamepad
void controlMotors() {
  if (GamePad.isPressed(UP_BIT)) {
    moveMotors(255, 255, true, true); // Tiến
    Serial.println("Moving forward");
  } else if (GamePad.isPressed(DOWN_BIT)) {
    moveMotors(255, 255, false, false); // Lùi
    Serial.println("Moving backward");
  } else if (GamePad.isPressed(LEFT_BIT)) {
    moveMotors(200, 0, true, true); // Rẽ trái
    Serial.println("Turning left");
  } else if (GamePad.isPressed(RIGHT_BIT)) {
    moveMotors(0, 200, true, true); // Rẽ phải
    Serial.println("Turning right");
  } else {
    stopMotors(); // Dừng động cơ
    Serial.println("Motors stopped");
  }
}

// Điều khiển tốc độ và hướng động cơ
void moveMotors(int speedA, int speedB, bool dirA, bool dirB) {
  motorASpeed = constrain(speedA, 0, 255);
  motorBSpeed = constrain(speedB, 0, 255);
  setMotorDirection(dirA, dirB);
  ledcWrite(PWM_CHANNEL_A, motorASpeed);
  ledcWrite(PWM_CHANNEL_B, motorBSpeed);
}

// Cài đặt hướng động cơ
void setMotorDirection(bool forwardA, bool forwardB) {
  digitalWrite(MOTOR_AIN1, forwardA ? HIGH : LOW);
  digitalWrite(MOTOR_AIN2, forwardA ? LOW : HIGH);
  digitalWrite(MOTOR_BIN1, forwardB ? HIGH : LOW);
  digitalWrite(MOTOR_BIN2, forwardB ? LOW : HIGH);
}

// Hàm dừng động cơ
void stopMotors() {
  motorASpeed = 0;
  motorBSpeed = 0;
  ledcWrite(PWM_CHANNEL_A, motorASpeed);
  ledcWrite(PWM_CHANNEL_B, motorBSpeed);
  digitalWrite(MOTOR_AIN1, LOW);
  digitalWrite(MOTOR_AIN2, LOW);
  digitalWrite(MOTOR_BIN1, LOW);
  digitalWrite(MOTOR_BIN2, LOW);
}
