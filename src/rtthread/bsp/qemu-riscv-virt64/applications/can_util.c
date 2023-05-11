#include "can_util.h"
#include <rtthread.h>

void CAN_GetData(can_rx_handle_t* phandle){
    uint32_t rx_data_addr = (phandle->can_instance_addr + 8) ;
    for(int i = 0; i < 8;i++){
        phandle->receive_data[i] =   ((uint8_t*)rx_data_addr)[7 - i] ;
    }
    phandle->id = ((uint32_t*)(phandle->can_instance_addr))[5];
    ((uint32_t*)(phandle->can_instance_addr))[4] = 0xaaaaaaaa;

}
void CAN_SendData(can_tx_handle_t* phandle){
    uint32_t tx_data_addr = (phandle->can_instance_addr ) ;
    ((uint32_t*)tx_data_addr)[0] = (*(uint16_t*)(&(phandle->send_data[4])) << 16 )| 
        (*(uint16_t*)(&(phandle->send_data[6])));
    
   
    ((uint32_t*)tx_data_addr)[1] = (*(uint16_t*)(&(phandle->send_data[0])) << 16 )| 
        (*(uint16_t*)(&(phandle->send_data[2])));
    // for(int i = 0; i < 8;i++){
    //     ((uint8_t*)tx_data_addr)[i] = phandle->send_data[7-i]; 
    // }
}

void CAN_ErrorHandler(void)
{
	
   while(1) {
		 return;
       
   }
}