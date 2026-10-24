#ifndef __SENSOR_H
#define __SENSOR_H

#include <Arduino.h>

typedef struct __SysStateType
{
    float mcu_time;
    float compute_time_cost;
    float run_mode;
    float rec_data_len;
    float rec_imu_l_freq;
    float rec_imu_r_freq;
    int8_t exo_state;

}_SysStateType;

typedef struct __ExoStateType
{
    float motor_pos;
    float motor_bias;
    float motor_vel;
    float motor_tau;

    float imu_angle;
    float imu_bias;
    float imu_gyro;
    float imu_accY;
    float imu_accZ;

    float run_mode;
    float des_tau;
    float des_vel;
    float des_pos;

    float gait_time;
    float gait_timer;
    float gait_trigger;
    float gait_steps;
    float gait_conti_steps;
    float gait_phase;
    float gait_phase_raw;

    float gait_theta_range;
    float gait_theta_middle;
    float gait_gyro_range;
    float gait_gyro_middle;

    float gait_theta_range_;
    float gait_theta_middle_;
    float gait_gyro_range_;
    float gait_gyro_middle_;

    float assist_profile;
    float motor_curr_cmd;
}_ExoStateType;

typedef union __ImuDataType
{
    int16_t raw_data[160];
    uint8_t b[80];
}_ImuDataType;

typedef union __BandDataType
{
    float data[25];
    uint8_t b[100];
}_BandDataType;

extern _ExoStateType exo_state_r_t;
extern _ExoStateType exo_state_l_t;
extern _SysStateType sys_state_t;
#endif