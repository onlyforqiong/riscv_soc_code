#include <motor_periph.h>
#include <pid_alg.h>
#include <can_util.h>
#include <rtthread.h>

Motor_MotorGroupTypedef Motor_chassisMotors;
Motor_MotorTypedef Motor_chassisLeft1;
Motor_MotorTypedef Motor_chassisLeft2;
Motor_MotorTypedef Motor_chassisRight1;
Motor_MotorTypedef Motor_chassisRight2;
PID_PIDparam_Typedef Motor_chassismotorpidparam = {fixedpt_fromint(10), fixedpt_rconst(0.0), fixedpt_rconst(0.0), fixedpt_fromint(10000), fixedpt_fromint(200000)};

#define can_base_addr 0x21000000
static can_rx_handle_t chassis_motor_rx = {0, can_base_addr};
static can_tx_handle_t chassis_motor_tx = {0, can_base_addr};

void Motor_SendMessage(Motor_MotorGroupTypedef *pmotor)
{

    if (pmotor->motor_type == Motor_CANMOTOR)
    {
        pmotor->hcan_tx.send_data[1] = (uint8_t)(((int16_t)fixedpt_toint((pmotor->motor_motorhandler[0].motor_pidhander.output))) >> 8);
        pmotor->hcan_tx.send_data[0] = (uint8_t)fixedpt_toint((pmotor->motor_motorhandler[0].motor_pidhander.output));
        pmotor->hcan_tx.send_data[3] = (uint8_t)(((int16_t)fixedpt_toint((pmotor->motor_motorhandler[1].motor_pidhander.output))) >> 8);
        pmotor->hcan_tx.send_data[2] = (uint8_t)fixedpt_toint((pmotor->motor_motorhandler[1].motor_pidhander.output));
        pmotor->hcan_tx.send_data[5] = (uint8_t)(((int16_t)fixedpt_toint((pmotor->motor_motorhandler[2].motor_pidhander.output))) >> 8);
        pmotor->hcan_tx.send_data[4] = (uint8_t)fixedpt_toint((pmotor->motor_motorhandler[2].motor_pidhander.output));
        pmotor->hcan_tx.send_data[7] = (uint8_t)(((int16_t)fixedpt_toint((pmotor->motor_motorhandler[3].motor_pidhander.output)) >> 8));
        pmotor->hcan_tx.send_data[6] = (uint8_t)fixedpt_toint((pmotor->motor_motorhandler[3].motor_pidhander.output));

        CAN_SendData(&(pmotor->hcan_tx));
    }
    else if (pmotor->motor_type == Motor_PWMMOTOR)
    {
    }
    else
    {
    }
}

void Motor_MotorEncoderDecode(uint8_t rxdata[], Motor_MotorTypedef *pmotor)
{

    pmotor->angle = rxdata[0] << 8 | rxdata[1];
    pmotor->speed = rxdata[2] << 8 | rxdata[3];
    pmotor->current = rxdata[4] << 8 | rxdata[5];
    pmotor->temp = rxdata[6];
    pmotor->motor_pidhander.fdb = fixedpt_fromint(pmotor->speed);
    
    //   if(flag == 1)
    //   pmotor->motor_pidhander.ref = 0;

    //   if(flag == 1&&pmotor->speed != 0) {
    //       power+=-pmotor->current*24.0*20*0.001/16384;
    //       if(pmotor->speed>=100)
    //       counter_value++;
    //   }
}

void Motor_MotorGroupsPID_Clac(Motor_MotorGroupTypedef *pmotor)
{

    uint8_t motor_num = pmotor->motor_number;
    for (; motor_num > 0; motor_num--)
    {
        Motor_MotorPID_Clac(&pmotor->motor_motorhandler[motor_num - 1]);
        if(motor_num == 4) {
            char out_string[20];

            fixedpt_str(pmotor->motor_motorhandler[3].motor_pidhander.fdb,out_string,-1); 
            rt_kprintf("3 fdb %s \n",out_string);
            fixedpt_str(pmotor->motor_motorhandler[3].motor_pidhander.output,out_string,-1);
            rt_kprintf("3 output %s \n",out_string); 
            fixedpt_str(pmotor->motor_motorhandler[3].motor_pidhander.ref,out_string,-1);
            rt_kprintf("3 ref %s \n",out_string); 
            fixedpt_str(pmotor->motor_motorhandler[3].motor_pidhander.sumi,out_string,-1);
            rt_kprintf("3 sum %s \n",out_string); 
            // fixedpt_str(pmotor->motor_motorhandler[3].motor_pidparamhander.kp,out_string,-1);
            // rt_kprintf("3 kp %s \n",out_string); 
        }
    }
}

void Motor_MotorPID_Clac(Motor_MotorTypedef *pmotor)
{

    PID_ClacPID(&pmotor->motor_pidhander, &pmotor->motor_pidparamhander);
}

void Motor_MotorGroupInit(Motor_MotorGroupTypedef *pmotor, uint8_t motor_num, Motor_MotorTypeEnum motors_type, can_tx_handle_t tx_handle, can_rx_handle_t rx_handle)
{

    pmotor->motor_number = motor_num;
    pmotor->motor_type = motors_type;
    pmotor->hcan_tx.id = tx_handle.id;
    pmotor->hcan_tx.can_instance_addr = tx_handle.can_instance_addr;
    pmotor->hcan_rx.id = rx_handle.id;
    pmotor->hcan_rx.can_instance_addr = rx_handle.can_instance_addr;
    pmotor->motor_motorhandler[0] = Motor_chassisLeft1;
    pmotor->motor_motorhandler[1] = Motor_chassisLeft2;
    pmotor->motor_motorhandler[2] = Motor_chassisRight1;
    pmotor->motor_motorhandler[3] = Motor_chassisRight2;

    Motor_motorInit(&pmotor->motor_motorhandler[0], motors_type, Motor_chassismotorpidparam);
    Motor_motorInit(&pmotor->motor_motorhandler[1], motors_type, Motor_chassismotorpidparam);
    Motor_motorInit(&pmotor->motor_motorhandler[2], motors_type, Motor_chassismotorpidparam);
    Motor_motorInit(&pmotor->motor_motorhandler[3], motors_type, Motor_chassismotorpidparam);
}

void Motor_motorInit(Motor_MotorTypedef *pmotor, Motor_MotorTypeEnum motortype, PID_PIDparam_Typedef motorpidparam)
{

    pmotor->angle = 0;
    pmotor->current = 0;
    pmotor->temp = 0;
    pmotor->speed = 0;

    pmotor->last_angle = 0;
    pmotor->limited_angle = 0;
    pmotor->motor_pidparamhander = motorpidparam;
    pmotor->has_init = Motor_HAS_INIT;
    pmotor->consequent_angle = 0;
    pmotor->motor_type = motortype;
    pmotor->init_offset = 0;
    //   pmotor->motor_pidparamhander = motorpidparam;
    PID_InitPID(&pmotor->motor_pidhander);
}

void Motor_Chassis_Decoder(Motor_MotorGroupTypedef *pmotor)
{
    //   HAL_CAN_GetRxMessage(phcan, CAN_RX_FIFO0, &rxhander, Can_RxData);
    CAN_GetData(&(pmotor->hcan_rx));
    int32_t speed =pmotor->hcan_rx.receive_data[2] << 8 | pmotor->hcan_rx.receive_data[3];
    rt_kprintf("id %x\n", pmotor->hcan_rx.id);
    switch (pmotor->hcan_rx.id)
    {
    case 0x201:
        // rt_kprintf("1 %d\n",speed);
        Motor_MotorEncoderDecode(pmotor->hcan_rx.receive_data, &pmotor->motor_motorhandler[0]);
        break;
    case 0x202:
        // rt_kprintf("2 %d\n",speed);
        Motor_MotorEncoderDecode(pmotor->hcan_rx.receive_data, &pmotor->motor_motorhandler[1]);
        break;
    case 0x203:
        // rt_kprintf("3 %d\n",speed);
        Motor_MotorEncoderDecode(pmotor->hcan_rx.receive_data, &pmotor->motor_motorhandler[2]);
        break;
    case 0x204:
        // rt_kprintf("4 %d\n",speed);
        Motor_MotorEncoderDecode(pmotor->hcan_rx.receive_data, &pmotor->motor_motorhandler[3]);
        break;
    }
}

void Task_MotorControl(void *argument)
{
    /* USER CODE BEGIN Task_MotorControl */
    // Can_IntFilterAndStart(&hcan1);
    Motor_MotorGroupInit(&Motor_chassisMotors, 4, Motor_CANMOTOR, chassis_motor_tx, chassis_motor_rx);
    for (int i = 0; i < 4; i++)
    {
        Motor_chassisMotors.motor_motorhandler[i].motor_pidhander.ref = fixedpt_fromint(2000);
    }
    /* Infinite loop */
    for (;;)
    {
        // Motor_chassisMotors.motor_motorhandler[]
        // rt_kprintf("ref is %d  %d",fixedpt_toint( Motor_chassisMotors.motor_motorhandler[0].motor_pidhander.ref),
        //    fixedpt_toint(Motor_chassisMotors.motor_motorhandler[3].motor_pidhander.ref)  );
        // rt_kprintf("come here\n");
        Motor_MotorGroupsPID_Clac(&Motor_chassisMotors);
        Motor_SendMessage(&Motor_chassisMotors);
        rt_thread_mdelay(20);
    }
    /* USER CODE END Task_MotorControl */
}
