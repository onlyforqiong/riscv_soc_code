/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-05-20     bigmagic     first version
 */
#include <rthw.h>
#include <rtdevice.h>

#include "board.h"
#include <uart_util.h>

#include <stdio.h>
#include "sbi.h"
#include "interrupt.h"

#define UART_CONFIG_NUM 3

struct device_uart
{
    rt_ubase_t  hw_base;
    rt_uint32_t irqno;
};

static rt_err_t  rt_uart_configure(struct rt_serial_device *serial, struct serial_configure *cfg);
static rt_err_t uart_control(struct rt_serial_device *serial, int cmd, void *arg);
static int       drv_uart_putc(struct rt_serial_device *serial, char c);
static int       drv_uart_getc(struct rt_serial_device *serial);

void uart_init(uart_handle* phandle)
{
    //http://byterunner.com/16550.html
    uart_write_reg(phandle,IER, 0x00);

    uint8_t lcr = uart_read_reg(phandle,LCR);
    uart_write_reg(phandle,LCR, lcr | (1 << 7));
    // uart_write_reg(DLL, 0x01);
    // uart_write_reg(DLM, 0x00);

    uart_write_reg(phandle,DLM,(UART_CONFIG_NUM & (0xff00)) >> 8 );
    uart_write_reg(phandle,DLL,UART_CONFIG_NUM & (0x00ff) );

    lcr = 0;
    uart_write_reg(phandle,LCR, lcr | (3 << 0));

    /*
     * enable receive interrupts.
     */
    uint8_t ier = uart_read_reg(phandle,IER);
    uart_write_reg(phandle,IER, ier | (1 << 0));
}

/*
 * UART interface
 */
static rt_err_t uart_configure(struct rt_serial_device *serial, struct serial_configure *cfg)
{
    struct device_uart *uart;

    RT_ASSERT(serial != RT_NULL);
    serial->config = *cfg;

    return (RT_EOK);
}

static rt_err_t uart_control(struct rt_serial_device *serial, int cmd, void *arg)
{
    struct device_uart *uart;

    uart = serial->parent.user_data;
    rt_uint32_t channel = 1;

    RT_ASSERT(uart != RT_NULL);
    RT_ASSERT(channel != 3);

    switch (cmd)
    {
    case RT_DEVICE_CTRL_CLR_INT:
        break;

    case RT_DEVICE_CTRL_SET_INT:
        break;
    }

    return (RT_EOK);
}

static int uart_putc(uart_handle *phandle, char c)
{
    while ((uart_read_reg(phandle,LSR) & LSR_TX_IDLE) == 0);
    return uart_write_reg(phandle,THR, c);
}

static int uart_getc(uart_handle *phandle)
{
    if (uart_read_reg(phandle,LSR) & LSR_RX_READY){
        return uart_read_reg(phandle,RHR);
    } else {
        return -1;
    }
    //return sbi_console_getchar();
}

static void rt_hw_uart_isr(int irqno, void *param)
{
    struct rt_serial_device *serial = (struct rt_serial_device*)param;
    rt_hw_serial_isr(serial, RT_SERIAL_EVENT_RX_IND);
}

struct rt_serial_device  serial2;
struct device_uart       uart2;

const struct rt_uart_ops uart_ops =
{
    uart_configure,
    uart_control,
    uart_putc,
    uart_getc,
    RT_NULL
};

/*
 * UART Initiation
 */
int hw_uart_init(uart_handle *phandle)
{
    struct rt_serial_device *serial;
    struct device_uart      *uart;
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;
    {
        serial  = &serial2;
        uart    = &uart2;

        serial->ops              = &uart_ops;
        serial->config           = config;
        serial->config.baud_rate = UART_DEFAULT_BAUDRATE;

        uart->hw_base   = phandle->uart_base;
        uart->irqno     = phandle->irq;

        uart_init(phandle);

        rt_hw_serial_register(serial,
                              "uart",
                              RT_DEVICE_FLAG_STREAM | RT_DEVICE_FLAG_RDWR, // | RT_DEVICE_FLAG_INT_RX,
                              uart);
        rt_hw_interrupt_install(uart->irqno, rt_hw_uart_isr, serial, "uart");

        rt_hw_interrupt_umask(uart->irqno);
    }

    return 0;
}
