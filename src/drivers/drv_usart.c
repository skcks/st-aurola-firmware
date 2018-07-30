/*
 * File      : usart.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006-2013, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2016-08-30     Aubr.Cool       the first version
 */
#include "drv_usart.h"

#ifdef RT_USING_UART

#ifndef RT_USING_DEVICE
#error "you must define RT_USING_DEVICE with uart device"
#endif

#ifndef RT_UART_RX_BUFFER_SIZE
#define RT_UART_RX_BUFFER_SIZE 64
#endif

/* STM32 uart driver */
struct stm32_uart
{
    struct rt_device parent;

    UART_HandleTypeDef huart;
    IRQn_Type uart_irq;

    /* buffer for reception */
    rt_uint8_t read_index, save_index;
    rt_uint8_t rx_buffer[RT_UART_RX_BUFFER_SIZE];
};
#ifdef RT_USING_UART1
struct stm32_uart uart1_device;
#endif

#ifdef RT_USING_UART2
struct stm32_uart uart2_device;
#endif

static void uart_irq_handler(struct stm32_uart* uart)
{
    rt_ubase_t level;
    /* enter interrupt */
    rt_interrupt_enter();
    
    
    level = rt_hw_interrupt_disable();
 
    if ((__HAL_UART_GET_FLAG(&uart->huart, UART_FLAG_RXNE) != RESET) && 
            (__HAL_UART_GET_IT_SOURCE(&uart->huart, UART_IT_RXNE) != RESET)) 
    {

        if (__HAL_UART_GET_FLAG(&uart->huart, UART_FLAG_RXNE) != RESET)
        {
            uart->rx_buffer[uart->save_index] = (rt_uint8_t)(uart->huart.Instance->RDR & 0xff);
            uart->save_index ++;
            if (uart->save_index >= RT_UART_RX_BUFFER_SIZE)
            {
                uart->save_index = 0;
            }
        }
    }

    rt_hw_interrupt_enable(level);
    
    /* invoke callback */
    if (uart->parent.rx_indicate != RT_NULL)
    {
        rt_size_t length;
        if (uart->read_index > uart->save_index)
            length = RT_UART_RX_BUFFER_SIZE - uart->read_index + uart->save_index;
        else
            length = uart->save_index - uart->read_index;

        uart->parent.rx_indicate(&uart->parent, length);
    }

    __HAL_UART_CLEAR_FLAG(&uart->huart, UART_FLAG_RXNE);

    /* leave interrupt */
    rt_interrupt_leave();
}


#ifdef RT_USING_UART1
void USART1_IRQHandler(void)
{
    uart_irq_handler(&uart1_device);
}
#endif
#ifdef RT_USING_UART2
void USART2_IRQHandler(void)
{
    uart_irq_handler(&uart2_device);
}
#endif

static void uart_io_init(UART_HandleTypeDef *huart)
{
#ifdef RT_USING_UART1
    if (huart->Instance == USART1)
    {
        GPIO_InitTypeDef GPIO_InitStruct;

        /* Peripheral clock enable */
        __HAL_RCC_GPIOA_CLK_ENABLE();
        

        /**USART1 GPIO Configuration  
        PA10   ------> USART1_TX
        PA9  ------> USART1_RX 
        */
        GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_9;
        GPIO_InitStruct.Alternate = GPIO_AF1_USART1;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Alternate = GPIO_AF1_USART1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
#endif

#ifdef RT_USING_UART2
    if (uart_base == USART2)
    {
        LL_GPIO_InitTypeDef GPIO_InitStruct;

        /* Peripheral clock enable */
        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);
        LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);

        /**USART2 GPIO Configuration  
        PA2   ------> USART2_TX
        PA3   ------> USART2_RX 
        */
        GPIO_InitStruct.Pin = LL_GPIO_PIN_2 | LL_GPIO_PIN_3;
        GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
        GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
        GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
        GPIO_InitStruct.Alternate = LL_GPIO_AF_4;
        LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
#endif
}

static void uart_hal_init(UART_HandleTypeDef *huart)
{
    UART_InitTypeDef *UART_InitStruct = &huart->Init;

    UART_InitStruct->BaudRate = 115200;
    UART_InitStruct->WordLength = UART_WORDLENGTH_8B;
    UART_InitStruct->StopBits = UART_STOPBITS_1;
    UART_InitStruct->Parity = UART_PARITY_NONE;
    UART_InitStruct->Mode = UART_MODE_TX_RX;
    UART_InitStruct->HwFlowCtl = UART_HWCONTROL_NONE;
    UART_InitStruct->OverSampling = UART_OVERSAMPLING_16;
    
    HAL_StatusTypeDef rst = HAL_UART_Init(huart);
    
    RT_ASSERT(rst == HAL_OK);
    
}

static rt_err_t rt_uart_init (rt_device_t dev)
{
    struct stm32_uart* uart;
    RT_ASSERT(dev != RT_NULL);
    uart = (struct stm32_uart *)dev;
    
    uart_io_init(&uart->huart);
    uart_hal_init(&uart->huart);

    return RT_EOK;
}

static rt_err_t rt_uart_open(rt_device_t dev, rt_uint16_t oflag)
{
    struct stm32_uart* uart;
    RT_ASSERT(dev != RT_NULL);
    uart = (struct stm32_uart *)dev;

    if (dev->flag & RT_DEVICE_FLAG_INT_RX)
    {
        /* Enable the UART Interrupt */
        NVIC_EnableIRQ(uart->uart_irq);

        /* enable interrupt */
        __HAL_UART_ENABLE_IT(&uart->huart, USART_IT_RXNE);
    }

    return RT_EOK;
}

static rt_err_t rt_uart_close(rt_device_t dev)
{
    struct stm32_uart* uart;
    RT_ASSERT(dev != RT_NULL);
    uart = (struct stm32_uart *)dev;

    if (dev->flag & RT_DEVICE_FLAG_INT_RX)
    {
        /* Disable the UART Interrupt */
        NVIC_DisableIRQ(uart->uart_irq);

          /* disable interrupt */
        __HAL_UART_DISABLE_IT(&uart->huart, USART_IT_RXNE);
    }

    return RT_EOK;
}

static rt_size_t rt_uart_read(rt_device_t dev, rt_off_t pos, void* buffer, rt_size_t size)
{
    struct stm32_uart* uart = (struct stm32_uart *)dev;
    rt_uint8_t *ptr;
    rt_size_t length;
    
    RT_ASSERT(dev != RT_NULL);
    RT_ASSERT(buffer != RT_NULL);
    
    ptr = (rt_uint8_t *) buffer;
    while (size)
    {
        /* interrupt receive */
        rt_base_t level;
        
        /* disable interrupt */
        level = rt_hw_interrupt_disable();
        if (uart->read_index != uart->save_index)
        {
            *ptr = uart->rx_buffer[uart->read_index];

            uart->read_index ++;
            if (uart->read_index >= RT_UART_RX_BUFFER_SIZE)
                uart->read_index = 0;
        }
        else
        {
            /* no data in rx buffer */

            /* enable interrupt */
            rt_hw_interrupt_enable(level);
            break;
        }

        /* enable interrupt */
        rt_hw_interrupt_enable(level);

        ptr ++;
        size --;
    }

    length = (rt_uint32_t)ptr - (rt_uint32_t)buffer;
    return length;
}

static rt_size_t rt_uart_write(rt_device_t dev, rt_off_t pos, const void* buffer, rt_size_t size)
{
    char *ptr = (char*) buffer;
    struct stm32_uart* uart = (struct stm32_uart *)dev;
    
    RT_ASSERT(dev != RT_NULL);
    RT_ASSERT(buffer != RT_NULL);

    if (dev->open_flag & RT_DEVICE_FLAG_STREAM)
    {
        /* stream mode */
        while (size)
        {
            if (*ptr == '\n')
            {
                while (RESET == __HAL_UART_GET_FLAG(&uart->huart, UART_FLAG_TXE));
                //填入换行符
                uart->huart.Instance->TDR = '\r'; 
   
            }

            //等待发送
            while (RESET == __HAL_UART_GET_FLAG(&uart->huart, UART_FLAG_TXE));

            //填入新数据
            uart->huart.Instance->TDR = *ptr; 

            ptr ++;
            size --;
        }
    }
    else
    {
        while (size)
        {
            //等待发送
            while (RESET == __HAL_UART_GET_FLAG(&uart->huart, UART_FLAG_TXE));

            //填入新数据
            uart->huart.Instance->TDR = *ptr; 

            ptr++;
            size--;
        }
    }

    return (rt_size_t)ptr - (rt_size_t)buffer;
}

int rt_hw_usart_init(void)
{
#ifdef RT_USING_UART1
    {
        struct stm32_uart* uart;

        /* get uart device */
        uart = &uart1_device;

        /* device initialization */
        uart->parent.type = RT_Device_Class_Char;
        uart->huart.Instance = USART1;
        uart->uart_irq = USART1_IRQn;
        uart->read_index = 0;
        uart->save_index = 0;
        rt_memset(uart->rx_buffer, 0, sizeof(uart->rx_buffer));

        /* device interface */
        uart->parent.init 	    = rt_uart_init;
        uart->parent.open 	    = rt_uart_open;
        uart->parent.close      = rt_uart_close;
        uart->parent.read 	    = rt_uart_read;
        uart->parent.write      = rt_uart_write;
        uart->parent.control    = RT_NULL;
        uart->parent.user_data  = RT_NULL;

        rt_device_register(&uart->parent, "uart1", RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
    }
#endif /* RT_USING_UART1 */

#ifdef RT_USING_UART2
    {
        struct stm32_uart* uart;

        /* get uart device */
        uart = &uart2_device;

        /* device initialization */
        uart->uart_base = USART2;
        uart->uart_irq  = USART2_IRQn;
        uart->read_index = 0;
        uart->save_index = 0;
        rt_memset(uart->rx_buffer, 0, sizeof(uart->rx_buffer));

        /* device interface */
        uart->parent.type       = RT_Device_Class_Char;
        uart->parent.init 	    = rt_uart_init;
        uart->parent.open 	    = rt_uart_open;
        uart->parent.close      = rt_uart_close;
        uart->parent.read 	    = rt_uart_read;
        uart->parent.write      = rt_uart_write;
        uart->parent.control    = RT_NULL;
        uart->parent.user_data  = RT_NULL;

        rt_device_register(&uart->parent, "uart2", RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
    }
#endif /* RT_USING_UART2 */
    return 0;
}
INIT_BOARD_EXPORT(rt_hw_usart_init);

#endif /*RT_USING_UART*/
