/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-05-20     bigmagic     first version
 */

#ifndef __UART_UTIL_H__
#define __UART_UTIL_H__

#include <rtthread.h>

#define UART1_IRQ   (10)

#define UART_DEFAULT_BAUDRATE               115200

// #define UART_BASE            (0x10000000L)

#define RHR 0    // Receive Holding Register (read mode)
#define THR 0    // Transmit Holding Register (write mode)
#define DLL 0    // LSB of Divisor Latch (write mode)
#define IER 1    // Interrupt Enable Register (write mode)
#define DLM 1    // MSB of Divisor Latch (write mode)
#define FCR 2    // FIFO Control Register (write mode)
#define ISR 2    // Interrupt Status Register (read mode)
#define LCR 3    // Line Control Register
#define MCR 4    // Modem Control Register
#define LSR 5    // Line Status Register
#define MSR 6    // Modem Status Register
#define SPR 7    // ScratchPad Register

// #define UART_REG(reg) ((volatile uint8_t *)(UART_BASE + reg))

#define LSR_RX_READY (1 << 0)
#define LSR_TX_IDLE  (1 << 5)

// #define uart_read_reg(reg) (*(UART_REG(reg)))
// #define uart_write_reg(reg, v) (*(UART_REG(reg)) = (v))

typedef struct {
    uint32_t uart_base;
    int irq;
    uint16_t div_value;
    uint8_t send_data;
    uint8_t receive_data;
} uart_handle;

typedef struct {
    uart_handle *rx_handle;
    int* receive_buffer;
    int receive_num;
    int receive_counter;
} uart_rx;

void uart_init(uart_handle* phandle);
uint8_t inline uart_read_reg(uart_handle * phandle,uint32_t reg ) {
    return (*(volatile uint8_t *)(phandle->uart_base + reg));
}
uint8_t inline uart_write_reg(uart_handle * phandle,uint32_t reg,uint8_t data ) {
    return (*(volatile uint8_t *)(phandle->uart_base + reg)) = data;
}

int hw_uart_init(uart_handle *phandle);
int uart_putc(uart_handle *phandle, char c);
int uart_getc(uart_handle *phandle);

extern uart_handle remote_uart_handle;
extern uart_rx rx_handle;
#endif /* __DRV_UART_H__ */
