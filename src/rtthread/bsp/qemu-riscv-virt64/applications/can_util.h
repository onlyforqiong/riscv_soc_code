/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CAN_UTIL_H
#define __CAN_UTIL_H

#ifdef __cplusplus
extern "C"
{
#endif

#include<rtfixpoint.h>
typedef struct {
    int id;
    uint32_t can_instance_addr;
    uint8_t send_data[8];
    // uint8_t receive_data[8];
}can_tx_handle_t;
typedef struct {
    int id;
    uint32_t can_instance_addr;
    uint8_t receive_data[8];
}can_rx_handle_t;



void CAN_ErrorHandler(void);
void CAN_GetData(can_rx_handle_t* phandle);
void CAN_SendData(can_tx_handle_t* phandle);


#ifdef __cplusplus
}
#endif
#endif