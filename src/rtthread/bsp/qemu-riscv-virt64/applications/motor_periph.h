#ifndef __MOTOR_PERIPH_H
#define __MOTOR_PERIPH_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <pid_alg.h>
#include <can_util.h>
#include <rtfixpoint.h>



typedef enum {
    Motor_UNCONNECTED = 0,
    Motor_CANMOTOR = 1,
    Motor_PWMMOTOR = 2
} Motor_MotorTypeEnum;

typedef enum {
    Motor_DIS_INIT = 0,
    Motor_HAS_INIT = 1
} Motor_MotorHasInit;

typedef struct {
    int16_t angle;
    int16_t speed;
    int16_t current;
    int16_t temp;

    int16_t last_angle;
    int16_t round_counter;
    Motor_MotorHasInit has_init;
    Motor_MotorTypeEnum motor_type;
    fixedpt init_offset;
    fixedpt limited_angle;
    fixedpt consequent_angle;

    PID_PIDparam_Typedef motor_pidparamhander;
    PID_PIDTypeDef motor_pidhander;

} Motor_MotorTypedef;

typedef struct {
    uint8_t motor_number;
    Motor_MotorTypeEnum motor_type;
    Motor_MotorTypedef motor_motorhandler[4];
    can_rx_handle_t hcan_rx;
    can_tx_handle_t hcan_tx;
} Motor_MotorGroupTypedef;

extern Motor_MotorGroupTypedef Motor_chassisMotors;


void Motor_SendMessage(Motor_MotorGroupTypedef* pmotor);
void Motor_MotorEncoderDecode(uint8_t rxdata[], Motor_MotorTypedef *pmotor);
void Motor_MotorGroupsPID_Clac(Motor_MotorGroupTypedef *pmotor);
void Motor_MotorPID_Clac(Motor_MotorTypedef *pmotor);
void Motor_MotorGroupInit(Motor_MotorGroupTypedef* pmotor, uint8_t motor_num, Motor_MotorTypeEnum motors_type, can_tx_handle_t tx_handle,can_rx_handle_t rx_handle);
void Motor_motorInit(Motor_MotorTypedef * pmotor, Motor_MotorTypeEnum motortype, PID_PIDparam_Typedef motorpidparam);
void Motor_Chassis_Decoder(Motor_MotorGroupTypedef* pmotor);
void Task_MotorControl(void  * argument);  


#ifdef __cplusplus
}
#endif



#endif

