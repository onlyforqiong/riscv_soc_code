#include<remote_periph.h>
#include <uart_util.h>
#include <stddef.h>
#include <motor_periph.h>

#define uart_get_length 6
int start_counter = 0;
uint8_t remote_buffer[12];
void remote_uart_get(void *param){
    hw_uart_init(&remote_uart_handle);
    rx_handle.receive_buffer = remote_buffer;
    rx_handle.receive_num   = uart_get_length;
    rx_handle.receive_counter = 0;
    while(1) {
        if(uart_getc(&remote_uart_handle) == 0) {
            if(rx_handle.receive_counter == 0 && remote_uart_handle.receive_data == 0xa5 ) {
                start_counter = 1;
            }
            if(start_counter == 1) {
                remote_buffer[rx_handle.receive_counter] = remote_uart_handle.receive_data;
                rx_handle.receive_counter = rx_handle.receive_counter + 1 ;
            }
            // rt_kprintf("data is %02x counter is %d\n",remote_uart_handle.receive_data,rx_handle.receive_counter);
            
            if(rx_handle.receive_counter == rx_handle.receive_num) {
                remote_decode(remote_buffer);
                rx_handle.receive_counter  = 0;
                 start_counter = 0;
            }
        }
       
        rt_thread_mdelay(1);
    }
}

void remote_decode(uint8_t*buffer) {
    if(buffer == NULL) {
        return;
    }else if(buffer[0] != 0xA5 || buffer[5] != 0x5a) {
        return;
    }else {
        for(int i   = 0; i < 6; i++) {
            rt_kprintf("%02x ", buffer[i]);
            // char out_string[20];
            // fixedpt_str(Motor_chassisMotors.motor_motorhandler[i].motor_pidhander.ref, out_string,-1);
            // rt_kprintf("ref %d  %s\n",i,out_string);
        }
        rt_kprintf("\n");
        int32_t x_data = (int8_t)buffer[1];
        int32_t y_data = (int8_t)buffer[2];
        int32_t row_data = (int8_t)buffer[3];
        x_data <<= 4;
        y_data <<= 4;
        row_data <<= 4;
        // char string_buffer[20];
        // fixedpt_str(fixedpt_fromint(x_data),string_buffer,-1);
        // rt_kprintf("x is %s\n",string_buffer);
        // Motor_chassisMotors.motor_motorhandler->motor_pidhander.ref
        
        Motor_chassisMotors.motor_motorhandler[0].motor_pidhander.ref =  fixedpt_fromint(-(y_data+x_data+row_data));
        Motor_chassisMotors.motor_motorhandler[1].motor_pidhander.ref =  fixedpt_fromint(-(y_data-x_data+row_data));
        Motor_chassisMotors.motor_motorhandler[2].motor_pidhander.ref =  fixedpt_fromint((y_data-x_data-row_data));
        Motor_chassisMotors.motor_motorhandler[3].motor_pidhander.ref =  fixedpt_fromint(  (y_data+x_data-row_data));


    }
}
