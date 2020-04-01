# Edge_teste_CPqD

[[_TOC_]]

This code is based on [/example/example1_edge_test](https://github.com/sparkfun/SparkFun_Edge_BSP)

## Installation process

To install the toolchain follow [sparkfun guide](https://learn.sparkfun.com/tutorials/using-sparkfun-edge-board-with-ambiq-apollo3-sdk/toolchain-setup) according to your operating system.

 - Tested compiler: gcc-arm-none-eabi-9-2019-q4-major
 - Tested SDK: AmbiqSuite-Rel2.2.0
 - [Sparkfun Board Support Package(BSP)](https://github.com/sparkfun/SparkFun_Edge_BSP): commit 6ea7ef137132bb0cb4b27abe837c913c7643f11e

On Windows 10, it is important to note that python3 is installed as python.exe, and since the toolchains expects for python3, an easy fix is to rename the executable and the shortcut that points to it.

On linux, on the Hausekeeping part of [sparkfun guide](https://learn.sparkfun.com/tutorials/using-sparkfun-edge-board-with-ambiq-apollo3-sdk/toolchain-setup) it may also be necessary to give execution permission to ```$AMB_ROOT/tools/apollo3_scripts/create_cust_wireupdate_blob.py``` 

[Sparkfun guide](https://learn.sparkfun.com/tutorials/using-sparkfun-edge-board-with-ambiq-apollo3-sdk/example-applications) says:
```
 - On Mac, you can simply list ls /dev/tty.*
 - On Unix-like operating systems you should also be able to show all available serial ports by entering ls /dev/cu* into the Bash shell.
```
where it should say
```
 - On Mac, you can simply list ls /dev/cu*
 - On Unix-like operating systems you should also be able to show all available serial ports by entering ls /dev/tty* into the Bash shell.
```
in [Pop!OS](https://system76.com/pop), a variant of [Ubuntu](https://ubuntu.com/), it is /dev/ttyUSB*

## Known linux issues

```bash
Makefile:193: recipe for target 'wired_update' failed
make: *** [wired_update] Error 126
```

try ```sudo chmod +x ../../../../../tools/apollo3_scripts/create_cust_wireupdate_blob.py```

---
if using **linux** or mac, you may find some errors such as

```bash
Makefile:197: recipe for target 'bootload' failed
make: *** [bootload] Error 1
```

[drive update issue](https://github.com/sparkfun/SparkFun_Edge_BSP/issues/3) that can be fixed folloing [this tutorial](https://learn.sparkfun.com/tutorials/how-to-install-ch340-drivers/all#linux) that guides you on how to install [this patch](https://github.com/juliagoda/CH341SER)

## Programming

### CH34x

[CH34x](./Edge-CH34x.png)

### FTDI232

[FTDI232](./Edge-FTDI232.png)

### Changing baud rate

Default baud is 115200, to use a different baud it is necessary to change am_bsp.c in line

```c
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

```c
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

```c
   //
    // Run the RTC off the LFRC.
    //
    am_hal_rtc_osc_select(AM_HAL_RTC_OSC_LFRC);
```

to

```c
   //
    // Run the RTC off the LFRC.
    //
    am_hal_rtc_osc_select(AM_HAL_CLKGEN_CLKOUT_LFRC_1024);
```

then `cd $AMS_ROOT/boards/SparkFun_Edge_BSP/bsp/gcc`
and run `make`
