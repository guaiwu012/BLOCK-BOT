#include <IRremote.h> // 导入库文件

volatile int System_Time; // 系统时间
volatile int Speed; // 速度
volatile int Interval_Time; // 间隔时间
const int MAX_COMMANDS = 50; // 设置指令列表最大长度
long ir_commands[MAX_COMMANDS]; // 定义全局数组存储指令
int command_index = 0; // 当前指令索引

const String IR_PROTOCOL_TYPE[] = {
  "UNKNOWN", "NEC", "SONY", "RC5", "RC6", "DISH", "SHARP", "PANASONIC", "JVC", "SANYO", "MITSUBISHI", "SAMSUNG", "LG", "WHYNTER"
};
IRrecv irrecv_2(A3); // 设置2号管脚读取数值

const int LED = 13; // D13可以控制板子上的LED灯

bool recording = false; // 是否在录制指令的标志

void setup() {
  Serial.begin(9600); // 设置串口波特率为9600
  irrecv_2.enableIRIn(); // 声明函数
  Speed = 160;
  System_Time = 0;
  Interval_Time = 0;
  pinMode(5, OUTPUT);
  pinMode(4, OUTPUT);
  digitalWrite(5, LOW);
  digitalWrite(4, LOW);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  pinMode(LED, OUTPUT); // 板载LED引脚是D13 设置为输出
}

void MotorA_Run(int MASpeed) {
  if (MASpeed > 0) {
    digitalWrite(4, HIGH);
  } else {
    digitalWrite(4, LOW);
  }
  analogWrite(5, abs(MASpeed));
}

void MotorB_Run(int MBSpeed) {
  if (MBSpeed > 0) {
    digitalWrite(7, HIGH);
  } else {
    digitalWrite(7, LOW);
  }
  analogWrite(6, abs(MBSpeed));
}

// 停止
void Stop() {
  MotorA_Run(0);
  MotorB_Run(0);
}

// 前进
void Forward() {
  MotorA_Run(Speed * -1);
  MotorB_Run(Speed * -1);
}

// 右转
void Right_Turn() {
  MotorA_Run(0.5 *Speed);
  MotorB_Run(Speed * -0.5);
}

// 后退
void Back_Off() {
  MotorA_Run(Speed);
  MotorB_Run(Speed);
}

// 左转
void Left_Turn() {
  MotorA_Run(Speed * -0.5);
  MotorB_Run(0.5 *Speed);
}

// 执行数组中的所有指令
void ExecuteCommands() {
  for (int i = 0; i < command_index; i++) {
    long cmd = ir_commands[i];
    if (cmd == -417792256) { // 前进
      Forward();
      delay(1000);
      Stop();
    } else if (cmd == -1387069696) { // 后退
      Back_Off();
      delay(1000);
      Stop();
    } else if (cmd == -150405376) { // 左转
      Left_Turn();
      delay(1000);
      Stop();
    } else if (cmd == -1520763136) { // 右转
      Right_Turn();
      delay(1000);
      Stop();
    }
  }
  command_index = 0; // 重置指令索引
}

void loop() {
  if (irrecv_2.decode()) { // 如果红外模块接收到数据
    struct IRData *pIrData = &irrecv_2.decodedIRData;
    long ir_item = pIrData->decodedRawData;
    String irProtocol = IR_PROTOCOL_TYPE[pIrData->protocol];
    Serial.print("IR TYPE:" + irProtocol + "\tVALUE:"); // 输出IR类型和所读数据
    Serial.println(ir_item); // 输出所读数据

    if (ir_item == -484638976) { // 如果是特殊指令
      if (recording) {
        ExecuteCommands(); // 执行所有记录的指令
      } else {
        command_index = 0; // 清空数组
      }
      recording = !recording; // 切换录制状态
      digitalWrite(LED, HIGH); // LED亮
      delay(500); // 延时500ms
      digitalWrite(LED, LOW); // LED灭
    } else if (recording) {
      // 只录制前进、后退、左转、右转指令
      if ((ir_item == -417792256 || ir_item == -1387069696 || ir_item == -150405376 || ir_item == -1520763136) && command_index < MAX_COMMANDS) {
        ir_commands[command_index++] = ir_item; // 录制指令
        digitalWrite(LED, HIGH); // LED亮
        delay(500); // 延时500ms
        digitalWrite(LED, LOW); // LED灭
      }
    } else {
      // 其他直接执行的指令
      if (ir_item == -1169817856) { // 1
        Back_Off();
        delay(2000);
        Forward();
        delay(2000);
        Left_Turn();
        delay(2000);
        Stop();
      } else if (ir_item == -417792256) { // 前进
        Forward();
        delay(1000);
        Stop();
      } else if (ir_item == -1387069696) { // 后退
        Back_Off();
        delay(1000);
        Stop();
      } else if (ir_item == -150405376) { // 左转
        Left_Turn();
        delay(1000);
        Stop();
      } else if (ir_item == -1520763136) { // 右转
        Right_Turn();
        delay(1000);
        Stop();
      } else if (ir_item == -384368896) { // **
        Left_Turn();
        delay(300);
        Stop();
        Forward();
        delay(4000);
        Stop();
        Right_Turn();
        delay(1000);
        Stop();
        Forward();
        delay(2000);
        Stop();
        Right_Turn();
        delay(800);
        Stop();
        Forward();
        delay(2000);
        Stop();
        Left_Turn();
        delay(1000);
        Stop();
        Forward();
        delay(2000);
        Stop();
        Right_Turn();
        delay(800);
        Stop();
        Forward();
        delay(2000);
        Stop();
        Right_Turn();
        delay(1000);
        Stop();
        Forward();
        delay(4000);
        Stop();
      }
    }
    Interval_Time = System_Time + 130;
    irrecv_2.resume();
  } else {
    System_Time = millis();
  }
}
