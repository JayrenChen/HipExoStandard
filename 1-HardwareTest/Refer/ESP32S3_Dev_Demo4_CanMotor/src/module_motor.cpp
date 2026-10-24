#include <module_motor.h>
#include <map_pin.h>
#include <ESP32-TWAI-CAN.hpp>
#include <ds_sys.h>

#define MaxSpeed  29.5f  // rad/s
#define MaxTorque 11.5f    // 12.0f
#define MaxAngle  1440.0f    // 
#define Ratio     7.75

#define MOTOR_BIG_END

CanFrame rxFrame;
CanFrame txFrame;

uint8_t CanTx[8] = {0x00};
uint8_t CanTxNull[8] = {0x00};
uint32_t ExtID = 0x00;
uint32_t ModeVal = 0x00;
uint32_t DataVal = 0x00;

const uint32_t MotorIdVal = 0x7F;
const uint32_t MasterIdVal = 0x01;

_CyberGearMotorDataTypeDef motor_cmd;
_CyberGearMotorDataTypeDef motor_rec;

_CyberGearMotorStateTypeDef motor_state;

/**
 * @brief 电机数据发送的大端小端转换
 * 
 * @param motor_cmd 
 */
void MotorParm2Bytes(_CyberGearMotorDataTypeDef motor_cmd)
{
    #ifdef MOTOR_BIG_END
        for(int inx = 0; inx < 8; inx+=2) CanTx[inx] = motor_cmd.b[inx+1];
        for(int inx = 1; inx < 8; inx+=2) CanTx[inx] = motor_cmd.b[inx-1];
    #endif
    #ifdef MOTOR_LITTLE_END
        for(int inx = 0; inx < 8; inx++) CanTx[inx] = motor_cmd.b[inx];
    #endif
    
}

/**
 * @brief 电机数据接收的大端小端转换
 * 
 * @param RXbytes 
 */
void Bytes2MotorState(uint8_t RXbytes[])
{
    #ifdef MOTOR_BIG_END
        for(int inx = 0; inx < 8; inx+=2) motor_rec.b[inx] = RXbytes[inx+1];
        for(int inx = 1; inx < 8; inx+=2) motor_rec.b[inx] = RXbytes[inx-1];
    #endif
    #ifdef MOTOR_LITTLE_END
        for(int inx = 0; inx < 8; inx++) motor_rec.b[inx] = RXbytes[inx];
    #endif
}

void Setup_MotorCan(void)
{
    // CAN总线初始化, 参考ESP32-TWAI-CAN官方例程
    pinMode(PIN_CAN_TX,OUTPUT);pinMode(PIN_CAN_RX,INPUT);
	ESP32Can.setPins(PIN_CAN_TX, PIN_CAN_RX);
    // 设置数据接收队列
    ESP32Can.setRxQueueSize(10);
	ESP32Can.setTxQueueSize(1);
    // 设置总线通信速度
    ESP32Can.setSpeed(ESP32Can.convertSpeed(1000));
    // CAN总线使能
    if(!ESP32Can.begin()) {ESP_LOGE("DRV", "CAN总线初始化失败❌");}
    else {ESP_LOGI("DRV", "CAN总线初始化成功✅");}

}

void Enable_Motor(void)
{
    #ifdef CYBERGEAR
        ExtID = 0x00; ModeVal = 0x03;
        ExtID |= (ModeVal << 24); 
        ExtID |= (MasterIdVal << 8); 
        ExtID |= MotorIdVal;

        CanFrame sendFrame = { 0 };
        sendFrame.identifier = ExtID; 
        sendFrame.extd = 1;
        sendFrame.data_length_code = 8;
        ESP32Can.writeFrame(sendFrame);

    #endif
}

void Set_Motor_Zero(void)
{
    
    #ifdef CYBERGEAR
        ExtID = 0x00; ModeVal = 0x06;
        ExtID |= (ModeVal << 24); 
        ExtID |= (MasterIdVal << 8); 
        ExtID |= MotorIdVal;

        CanFrame sendFrame = { 0 };
        sendFrame.identifier = ExtID; 
        sendFrame.extd = 1;
        sendFrame.data_length_code = 8;
        sendFrame.data[0] = 0x01;
        ESP32Can.writeFrame(sendFrame);

    #endif
}

void Set_Motor_Speed(float des_vel)
{   
    #ifdef CYBERGEAR
        // 根据减速比计算转子速度
        des_vel = des_vel / 180.0f * 3.1415926f;

        // 输出限幅
        if(des_vel >  MaxSpeed) des_vel =  MaxSpeed;
        if(des_vel < -MaxSpeed) des_vel = -MaxSpeed;

        // 计算转速
        motor_cmd.parm[1] = (des_vel / 30.0f) * 0x7FFF + 0x7FFF;
        motor_cmd.parm[3] = 0.1 * 0xFFFF;
        motor_cmd.parm[0] = 0x7FFF;
        motor_cmd.parm[2] = 0x0000;

        MotorParm2Bytes(motor_cmd);

        ModeVal = 0x01; DataVal = 0x7FFF; ExtID = 0x00;
        ExtID |= (ModeVal << 24); ExtID |= (DataVal << 8); ExtID |= MotorIdVal;
        
        CanFrame sendFrame = { 0 };
        sendFrame.identifier = ExtID; 
        sendFrame.extd = 1;
        sendFrame.data_length_code = 8;
        memcpy(sendFrame.data, CanTx, 8);
        ESP32Can.writeFrame(sendFrame);
    #endif
}


void Get_Motor_Data(void)
{
    #ifdef CYBERGEAR
        static float last_motor_pos = 0.0;
        static float this_motor_pos = 0.0;

        motor_state.pos  = (float)(motor_rec.parm[0] - 0x7FFF)/32767.0f*4*180.0f;
        motor_state.vel  = (float)(motor_rec.parm[1] - 0x7FFF)/32767.0f*30/3.1415926f*180.0f;
        motor_state.tau  = (float)(motor_rec.parm[2] - 0x7FFF)/32767.0f*12;
        motor_state.temp = (float)motor_rec.parm[3]/10.0f;
    #endif
}

void Rec_Motor_State(void)
{
    // ESP_LOGI("CAN","Rec Motor Loop");
    while(ESP32Can.readFrame(rxFrame, 1))
    {
        if(rxFrame.extd == 1)
        {
            switch((rxFrame.identifier>>8) & 0xFF)
            {
                case MotorIdVal: Bytes2MotorState(rxFrame.data); Get_Motor_Data(); break;
                // case MotorRIdVal: Bytes2MotorRState(rxFrame.data); Get_MotorR_Data(); break;
                case 0x1001: ESP_LOGI("CAN","Motor Version: %02X.%02X.%02X", rxFrame.data[3],rxFrame.data[4],rxFrame.data[5]); break;
                case 0x1000: ESP_LOGI("CAN","Motor Version: %02X.%02X.%02X", rxFrame.data[3],rxFrame.data[4],rxFrame.data[5]); break;
                case 0x0001: ESP_LOGI("CAN","Motor Version: %02X.%02X.%02X", rxFrame.data[3],rxFrame.data[4],rxFrame.data[5]); break;
                default: break; 
            }
        }
    }
}

void Get_Motor_Version(void)
{
    // #ifdef CYBERGEAR
    // ExtID = 0x00; ModeVal = 0x03;
    // ExtID |= (ModeVal << 24); 
    // ExtID |= (MasterIdVal << 8); 
    // ExtID |= MotorIdVal;

    ExtID = 0x1001;

    CanFrame sendFrame = { 0 };
    sendFrame.identifier = ExtID; 
    sendFrame.extd = 1;
    sendFrame.data_length_code = 8;
    sendFrame.data[0] = 0xC5;
    sendFrame.data[1] = 0x01;
    sendFrame.data[2] = 0x20;
    sendFrame.data[3] = 0x00;
    sendFrame.data[4] = 0x00;
    sendFrame.data[5] = 0x00;
    sendFrame.data[6] = 0xE6;
    sendFrame.data[7] = 0x5C;
    ESP32Can.writeFrame(sendFrame);

    // #endif
}

void Set_Motor_Tor(void)
{
    // #ifdef CYBERGEAR
    // ExtID = 0x00; ModeVal = 0x03;
    // ExtID |= (ModeVal << 24); 
    // ExtID |= (MasterIdVal << 8); 
    // ExtID |= MotorIdVal;

    ExtID = 0x1001;

    CanFrame sendFrame = { 0 };
    sendFrame.identifier = ExtID; 
    sendFrame.extd = 1;
    sendFrame.data_length_code = 8;

    // sendFrame.data[0] = 0xC5;
    // sendFrame.data[1] = 0x01;
    // sendFrame.data[2] = 0xF0;
    // sendFrame.data[3] = 0x00;
    // sendFrame.data[4] = 0x03;
    // sendFrame.data[5] = 0xE8;
    // sendFrame.data[6] = 0xA1;
    // sendFrame.data[7] = 0x5C;

    sendFrame.data[1] = 0xC5;
    sendFrame.data[0] = 0x01;
    sendFrame.data[3] = 0xF0;
    sendFrame.data[2] = 0x00;
    sendFrame.data[5] = 0x03;
    sendFrame.data[4] = 0xE8;
    sendFrame.data[7] = 0xA1;
    sendFrame.data[6] = 0x5C;

    ESP32Can.writeFrame(sendFrame);
    ESP_LOGI("CAN","Set Motor Torque: %02X.%02X.%02X.%02X.%02X.%02X.%02X.%02X", sendFrame.data[0],sendFrame.data[1],sendFrame.data[2],sendFrame.data[3],sendFrame.data[4],sendFrame.data[5],sendFrame.data[6],sendFrame.data[7]);

    // #endif
}