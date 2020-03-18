# Edge_teste_CPqD

To install the toolchain follow [sparkfun guide](https://learn.sparkfun.com/tutorials/using-sparkfun-edge-board-with-ambiq-apollo3-sdk/toolchain-setup)

This code is based on [/example/example1_edge_test](https://github.com/sparkfun/SparkFun_Edge_BSP)

## Troubleshooting

if using **linux** or mac, you may find some [drive update issue](https://github.com/sparkfun/SparkFun_Edge_BSP/issues/3) that can be fixed folloing [this tutorial](https://learn.sparkfun.com/tutorials/how-to-install-ch340-drivers/all#linux) that guides you on how to install [this patch](https://github.com/juliagoda/CH341SER)

## Programming


## Changing baud rate

Default baud is 115200, to use a different baud it is necessary to change am_bsp.c in line

```
static const am_hal_uart_config_t g_sBspUartConfig =
{
    //
    // Standard UART settings: 115200-8-N-1
    //
    .ui32BaudRate = 115200,
    .ui32DataBits = AM_HAL_UART_DATA_BITS_8,
    .ui32Parity = AM_HAL_UART_PARITY_NONE,
    .ui32StopBits = AM_HAL_UART_ONE_STOP_BIT,
    .ui32FlowControl = AM_HAL_UART_FLOW_CTRL_NONE,

    //
    // Set TX and RX FIFOs to interrupt at half-full.
    //
    .ui32FifoLevels = (AM_HAL_UART_TX_FIFO_1_2 |
                       AM_HAL_UART_RX_FIFO_1_2),

    //
    // The default interface will just use polling instead of buffers.
    //
    .pui8TxBuffer = 0,
    .ui32TxBufferSize = 0,
    .pui8RxBuffer = 0,
    .ui32RxBufferSize = 0,
};
```

to 
```
static const am_hal_uart_config_t g_sBspUartConfig =
{
    //
    // Standard UART settings: 115200-8-N-1
    //
//    .ui32BaudRate = 115200,               //original baud
    .ui32BaudRate = 1200,                   // desired baud
    .ui32DataBits = AM_HAL_UART_DATA_BITS_8,
    .ui32Parity = AM_HAL_UART_PARITY_NONE,
    .ui32StopBits = AM_HAL_UART_ONE_STOP_BIT,
    .ui32FlowControl = AM_HAL_UART_FLOW_CTRL_NONE,

    //
    // Set TX and RX FIFOs to interrupt at half-full.
    //
    .ui32FifoLevels = (AM_HAL_UART_TX_FIFO_1_2 |
                       AM_HAL_UART_RX_FIFO_1_2),

    //
    // The default interface will just use polling instead of buffers.
    //
    .pui8TxBuffer = 0,
    .ui32TxBufferSize = 0,
    .pui8RxBuffer = 0,
    .ui32RxBufferSize = 0,
};
```

and 
```
   //
    // Run the RTC off the LFRC.
    //
    am_hal_rtc_osc_select(AM_HAL_RTC_OSC_LFRC);
```
to
```
   //
    // Run the RTC off the LFRC.
    //
    am_hal_rtc_osc_select(AM_HAL_CLKGEN_CLKOUT_LFRC_1024);
```

then `cd $AMS_ROOT/boards/SparkFun_Edge_BSP/bsp/gcc`
and run `make`