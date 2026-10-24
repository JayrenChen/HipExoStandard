#include <module_ble.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <Update.h>
#include <StreamString.h>
#include <module_wifi.h>

// 定义服务UUID，与手机一致
#define SERVICE_UUID        "0000ffe5-0000-1000-8000-00805f9b34fb"

#define Data_CHARACTERISTIC_UUID "0000ffe1-0000-1000-8000-00805f9b34fb" // 外骨骼数据传输

// #define Info_CHARACTERISTIC_UUID "00001234-0000-1000-8000-008080808080" // 外骨骼信息特征
// #define Data_CHARACTERISTIC_UUID "00001235-0000-1000-8000-008080808080" // 外骨骼数据传输
// #define OTA_CHARACTERISTIC_UUID  "00001236-0000-1000-8000-008080808080" // 外骨骼固件升级

_AppSendDataType app_send_data_t;
_AppSendInfoType app_send_info_t;
_AppRecDataType  app_rec_data_t;

_FirmwareSize firmware_size_t;
static unsigned long rec_firmware_sum = 0;
static unsigned long rec_firmware_len = 0;

bool app_deviceConnected = false;
bool app_old_deviceConnected = false;
bool app_ota_in_progress = false;

uint8_t ota_rec[512] = {0};

_BleMasterSendDataType ble_master_send_data_t;

BLEServer *pServer = NULL;
BLEService *pService = NULL;
BLEAdvertising *pAdvertising = NULL;

BLECharacteristic* Info_Characteristic = NULL;
BLECharacteristic* Data_Characteristic = NULL;
BLECharacteristic* OTA_Characteristic = NULL;

_ImuBleDataType imu_ble_data_t;

const float deg_to_rad = 0.01745329252f;

/**
 * @brief 蓝牙连接回调，当手机端连接或断开时触发，用于更新连接状态标志
 */
class AppServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        app_deviceConnected = true;
        ESP_LOGI("BLE", "APP连接成功🔊");
        uint16_t bleID = pServer->getConnId();
        pServer->updatePeerMTU(bleID, 512);
        ESP_LOGI("BLE", "更新MTU到%i字节", pServer->getPeerMTU(bleID));
    };

    void onDisconnect(BLEServer* pServer) {
        app_deviceConnected = false;
        ESP_LOGI("BLE", "APP连接断开🔇");
    }
};

/**
 * @brief OTA数据处理回调，只处理固件上传相关的指令和数据包
 * 
 * 通信协议：
 * 1. 接收固件大小：7A XX XX XX XX 7B  （XX XX XX XX为4字节固件大小，低字节在前）
 * 2. 升级开始指令：7A A1 01 7B
 * 3. 升级结束指令：7A A1 10 7B
 * 4. 固件数据包：二进制数据包，最大512字节
 */
class OtaCharacterCallbacks: public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *pCharacteristic)
    {
        std::string value = pCharacteristic->getValue();
        if (value.length() > 0) {

            rec_firmware_len = value.length();
            for (int i = 0; i < rec_firmware_len; i++) ota_rec[i] = value[i]; // 转为字节数组

            if(ota_rec[0]==0x7A && ota_rec[rec_firmware_len-1] == 0x7B) {   // 处理传输控制指令
                if(4==rec_firmware_len && ota_rec[1]==0xA1 && ota_rec[2]==0x01) { // 升级开始指令
                    if(!Update.begin(firmware_size_t.data)) {
                        StreamString error;
                        Update.getError(); Update.printError(error); error.trim(); 
                        ESP_LOGE("BLE", "Update.begin failed! (%s)\n", error.c_str());
                    }
                    app_ota_in_progress = true;
                    ESP_LOGI("BLE", "Update Begin");
                }
                if(4==rec_firmware_len && ota_rec[1]==0xA1 && ota_rec[2]==0x10) { // 结束固件传输
                    if(!Update.end()) {
                        StreamString error;
                        Update.getError(); Update.printError(error); error.trim(); 
                        ESP_LOGE("BLE", "Update.end failed! (%s)\n", error.c_str());
                    }
                    app_ota_in_progress = false;
                    ESP_LOGI("BLE", "OTA升级成功🤖");
                }
                if(6==rec_firmware_len) { // 接收固件大小指令
                    memcpy(firmware_size_t.b,&ota_rec[1],4);
                    ESP_LOGI("BLE", "Firmware Size :%d", firmware_size_t.data);
                }
            }
            else { // 固件升级进程
                // 计算升级进度
                rec_firmware_sum += rec_firmware_len;
                Serial0.print("OTA Process :");
                Serial0.print((rec_firmware_sum*100.0)/firmware_size_t.data);
                Serial0.println(" %");
                // 直接写入固件数据
                Update.write(ota_rec,rec_firmware_len);
            }
        }
    }
};

/**
 * @brief 数据上传处理回调，主要用于接收数据上传的APP应答，目前仅打印接收到的数据
 */
class DataCharacterCallbacks: public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *pCharacteristic)
    {
        std::string value = pCharacteristic->getValue();
        if (value.length() > 0)
        {
            for (int i = 0; i < value.length(); i++)
                Serial0.print(value[i]);
        }
    }
};

/**
 * @brief 信息上传处理回调，主要用于接收APP的模式切换指令，目前仅打印接收到的数据
 */
class InfoCharacterCallbacks: public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *pCharacteristic)
    {
        std::string value = pCharacteristic->getValue();
        if (value.length() > 0)
        {
            for (int i = 0; i < value.length(); i++)
                Serial0.print(value[i]);
        }
    }
};

/**
 * @brief 初始化BLE服务和特征
 */
void Init_BLE_Server(void)
{
    // 初始化BLE服务器
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new AppServerCallbacks());

    // 创建BLE服务
    pService = pServer->createService(SERVICE_UUID);

    // 创建数据传输特征
    Data_Characteristic = pService->createCharacteristic(Data_CHARACTERISTIC_UUID,
                                                        BLECharacteristic::PROPERTY_READ |
                                                        BLECharacteristic::PROPERTY_WRITE |
                                                        BLECharacteristic::PROPERTY_NOTIFY |
                                                        BLECharacteristic::PROPERTY_INDICATE);
    Data_Characteristic->setCallbacks(new DataCharacterCallbacks());
    Data_Characteristic->addDescriptor(new BLE2902());

    // // 创建信息传输特征
    // Info_Characteristic = pService->createCharacteristic(Info_CHARACTERISTIC_UUID,
    //                                                     BLECharacteristic::PROPERTY_READ |
    //                                                     BLECharacteristic::PROPERTY_WRITE |
    //                                                     BLECharacteristic::PROPERTY_NOTIFY |
    //                                                     BLECharacteristic::PROPERTY_INDICATE);
    // Info_Characteristic->setCallbacks(new InfoCharacterCallbacks());
    // Info_Characteristic->addDescriptor(new BLE2902());
    
    // // 创建OTA升级特征
    // OTA_Characteristic = pService->createCharacteristic(OTA_CHARACTERISTIC_UUID,
    //                                                     BLECharacteristic::PROPERTY_READ |
    //                                                     BLECharacteristic::PROPERTY_WRITE |
    //                                                     BLECharacteristic::PROPERTY_NOTIFY |
    //                                                     BLECharacteristic::PROPERTY_INDICATE);
    // OTA_Characteristic->setCallbacks(new OtaCharacterCallbacks());
    // OTA_Characteristic->addDescriptor(new BLE2902());

    // 启动BLE服务
    pService->start();
    pAdvertising = pServer->getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(false);
    pAdvertising->setMinPreferred(0x00);
    // pAdvertising->setMinPreferred(0x06);  // 用于兼容ios设备
    // pAdvertising->setMinPreferred(0x12);
    pAdvertising->start();
    ESP_LOGI("BLE", "BLE启动完成, 等待APP连接");
}

/**
 * @brief 上传运动数据到手机APP，用于云端存储
 * 
 */
void Send_App_Data(uint8_t *data, uint16_t len)
{
    if(len > 512) {
        len = 512; 
        ESP_LOGW("BLE", "发送数据过长！");
    }
    // memcpy(app_send_data_t.b, data, len);
    // Data_Characteristic->setValue(app_send_data_t.b, len);  //设置电压的特征值
    // Data_Characteristic->indicate();                  //发送通知
    Data_Characteristic->setValue(data, len);  //设置电压的特征值
    Data_Characteristic->notify();                  //发送通知
}

/**
 * @brief 上传外骨骼状态信息到手机APP,用于UI显示
 */
void Send_App_Info()
{
    static int16_t bat = 88;
    static int16_t vel = 56;
    static int16_t steps = 0;

    app_send_info_t.data[0] = bat;
    app_send_info_t.data[1] = vel;
    app_send_info_t.data[2] = steps++;
    Info_Characteristic->setValue(app_send_info_t.b,6);  //设置电压的特征值
    Info_Characteristic->indicate();                  //发送通知
    ESP_LOGI("BLE", "传输状态信息✅");
}

_QuatType EulerToQuaternion(float roll, float pitch, float yaw)
{
    _QuatType q;
    
    float cy = cos(yaw * 0.5 * deg_to_rad);
    float sy = sin(yaw * 0.5 * deg_to_rad);
    float cr = cos(roll * 0.5 * deg_to_rad);
    float sr = sin(roll * 0.5 * deg_to_rad);
    float cp = cos(pitch * 0.5 * deg_to_rad);
    float sp = sin(pitch * 0.5 * deg_to_rad);

    q.w = cy * cr * cp + sy * sr * sp;
    q.x = cy * sr * cp - sy * cr * sp;
    q.y = cy * cr * sp + sy * sr * cp;
    q.z = sy * cr * cp - cy * sr * sp;

    return q;
}

// void Init_IMU_BLE_Data(void)
// {
//     for(int i = 0; i < 15; i++)
//     {
//         imu_ble_data_t[i].data.head1 = 0x7f - i;
//         imu_ble_data_t[i].data.head2 = 0xf1 + i;
//         imu_ble_data_t[i].data.tail1 = 0xe0 + i;
//         imu_ble_data_t[i].data.tail2 = 0xef - i;
//     }
// }

void Send_IMU_BLE(void)
{
    _IMUDataTypeDef imu_data_t;
    for(int i = 0; i < 6; i++)
    {
        memcpy(imu_data_t.b, IMU_Data[i].b, 28);
        imu_ble_data_t.data.head1 = 0x7F - i;
        imu_ble_data_t.data.head2 = 0xF1 + i;
        imu_ble_data_t.data.idx = i+1;
        _QuatType q = EulerToQuaternion(imu_data_t.data[10]/32768.0*180.0, imu_data_t.data[11]/32768.0*180.0, imu_data_t.data[12]/32768.0*180.0);
        
        imu_ble_data_t.data.x = q.x;
        imu_ble_data_t.data.y = q.y;
        imu_ble_data_t.data.z = q.z;
        imu_ble_data_t.data.w = q.w;

        // imu_ble_data_t.data.ax = -imu_data_t.data[0]/32768.0*16.0;
        // imu_ble_data_t.data.ay = -imu_data_t.data[1]/32768.0*16.0;
        // imu_ble_data_t.data.az = -imu_data_t.data[2]/32768.0*16.0;
        
        // 调整IMU正方向
        imu_ble_data_t.data.ax =  imu_data_t.data[1]/32768.0*16.0;
        imu_ble_data_t.data.ay = -imu_data_t.data[0]/32768.0*16.0;
        imu_ble_data_t.data.az = -imu_data_t.data[2]/32768.0*16.0;

        imu_ble_data_t.data.tail1 = 0xE0 + i;
        imu_ble_data_t.data.tail2 = 0xEF - i;
        Send_App_Data((uint8_t*)imu_ble_data_t.b, 18);
        Send_App_Data((uint8_t*)imu_ble_data_t.b+18, 18);
    }
}

/**
 * @brief 主循环处理函数，处理连接状态变化和定时数据上传
 */
void BLE_Process()
{
    static int16_t info_counter = 0;
    if(app_deviceConnected)
    {
        Send_IMU_BLE();
        // if(++info_counter >= 100) 
        // {
        //     info_counter = 0;
        //     if(app_ota_in_progress == false)
        //         Send_App_Info();
        // }
    }
    // 断开连接时重新广播
    if(!app_deviceConnected && app_old_deviceConnected) // 连接状态变化
    {
        vTaskDelay(500 / portTICK_PERIOD_MS); // give the bluetooth stack the chance to get things ready
        pAdvertising->start(); // restart advertising
        ESP_LOGI("BLE", "重新开启广播, 等待APP连接");
        app_old_deviceConnected = app_deviceConnected;
    }
    // 更新连接状态
    if(app_deviceConnected && !app_old_deviceConnected) 
    {
        app_old_deviceConnected = app_deviceConnected; // do stuff here on connecting
    }
}

/**
 * @brief 初始化BLE设备，IMU和手机APP都需要BLE，这里统一初始化
 *
 * @param deviceName
 */
void Setup_BLE(std::string deviceName)
{
    BLEDevice::init(deviceName);
    BLEDevice::setMTU(517);

    // Init_IMU_BLE_Data();
    Init_BLE_Server();
}
