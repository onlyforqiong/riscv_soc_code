/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-05-20     bigmagic     first version
 */

#include <rtthread.h>
#include<rtfixpoint.h>
#include<pid_alg.h>
#include<can_util.h>
#include<motor_periph.h>

#define THREAD_PRIORITY   3
#define THREAD_STACK_SIZE 512
#define THREAD_TIMESLICE  5

static rt_thread_t tid1 = RT_NULL;
static rt_thread_t tid2 = RT_NULL;
// static char out_buf[16];
// extern char *itoa(unsigned int val, char *str, int base);

//*****************************

#define  can_base_addr  0x21000000

#define  can_data_addr_0 (can_base_addr + 0)
#define  can_data_addr_1 (can_base_addr + 4)
#define  can_data_addr_2 (can_base_addr + 8)
#define  can_data_addr_3 (can_base_addr + 12)
#define  can_data_addr_4 (can_base_addr + 16)
//********************************

// char v_string[20]  ;

static can_rx_handle_t chassis_motor_rx = {0,can_base_addr};
static can_tx_handle_t chassis_motor_tx = {0,can_base_addr};
static void low_prior_entry(void *parameter)
{
    rt_uint32_t count = 0, id;
    fixedptd v_motor = fixedpt_fromint(0);
    char out_string [20] ;
    // volatile uint32_t* prxdata = can_base_addr;
    // prxdata[0] = 2000<<16 | 2000;
    // prxdata[1] = 2000<<16 | 2000;
    PID_PIDTypeDef motor_pid_handle = {};
    motor_pid_handle.ref = fixedpt_fromint(2000);
    int counter = 0;
    while (1)
    {
        id = (rt_uint32_t)parameter;

        CAN_GetData(&chassis_motor_rx);
        int16_t final_16_data = (int16_t)(chassis_motor_rx.receive_data[2] << 8 | chassis_motor_rx.receive_data[3]);
        int32_t final_32_data = final_16_data;
        v_motor = fixedpt_fromint(final_32_data);
        fixedpt_str(v_motor,out_string,-1);
        rt_kprintf("v_motor is %s and all data is %lx\n",out_string,(uint64_t*)(chassis_motor_rx.receive_data));
        motor_pid_handle.fdb = v_motor;
        PID_ClacPID(&motor_pid_handle,&PID_ChassisMotorpidparam);
        int cal_out = fixedpt_toint(motor_pid_handle.output);
        int16_t send_data = cal_out;

        // chassis_motor_tx.send_data[0] = send_data;
        // chassis_motor_tx.send_data[1] = send_data >> 8; 
        chassis_motor_tx.send_data[2] = send_data;
        chassis_motor_tx.send_data[3] = send_data >> 8; 
        chassis_motor_tx.send_data[4] = send_data;
        chassis_motor_tx.send_data[5] = send_data >> 8; 
        // chassis_motor_tx.send_data[6] = send_data;
        // chassis_motor_tx.send_data[7] = send_data >> 8; 
        CAN_SendData(&chassis_motor_tx);
        
        if(counter == 5) {

          fixedpt_str(motor_pid_handle.output,out_string,-1);
          rt_kprintf("  %s \n",out_string);
          counter = 0;
        }else{
          counter ++;
        }
        
        rt_thread_mdelay(1);
    }
    rt_kprintf("thread%u exit\n", id);
}

// ALIGN(RT_ALIGN_SIZE)
// static char thread3_stack[1024];
// static struct rt_thread thread3;
// static void high_prior_entry(void *param)
// {
//      rt_kprintf("motor chassis start\n");
//     while(1) {
//         Motor_Chassis_Decoder(&Motor_chassisMotors);
//         rt_thread_mdelay(1);
//     }
//     rt_kprintf("thread3 exit\n");
// }

static void motor_read_entry(void *param)
{
    // rt_kprintf("motor read start\n");
    while(1) {
        Motor_Chassis_Decoder(&Motor_chassisMotors);
        rt_thread_mdelay(1);
    }
    // rt_kprintf("thread3 exit\n");
}

int thread_sample(void)
{
    tid1 = rt_thread_create("can_motor_control",
                            Task_MotorControl, (void*)1,
                            1024,
                            THREAD_PRIORITY, THREAD_TIMESLICE);

    if (tid1 != RT_NULL)
    {
        rt_thread_startup(tid1);
    }
    // tid1 = rt_thread_create("low_prior_entry",
    //                         low_prior_entry, (void*)1,
    //                         1024,
    //                         THREAD_PRIORITY, THREAD_TIMESLICE);

    // if (tid1 != RT_NULL)
    // {
    //     rt_thread_startup(tid1);
    // }


    tid2 = rt_thread_create("motor_read_entry",
                            motor_read_entry, (void*)2,
                            1024,
                            2, 1);

    if (tid2 != RT_NULL)
    {
        rt_thread_startup(tid2);
    }
        

    // rt_thread_init(&thread3,
    //                "thread3",
    //                high_prior_entry,
    //                RT_NULL,
    //                &thread3_stack[0],
    //                sizeof(thread3_stack),
    //                THREAD_PRIORITY - 1, THREAD_TIMESLICE);
    // rt_thread_startup(&thread3);

    return 0;
}


int main(void)
{
    rt_kprintf("Hello RISC-V!\n");
    thread_sample();
    return 0;
}
