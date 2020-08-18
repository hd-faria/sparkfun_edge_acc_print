//*****************************************************************************
//
//! @file main.c
//!
//! @brief Edge Test is an example to demonstrate the built-in hardware on the Edge board
//!
//! 1200 baud, 8-n-1 mode.
//
//*****************************************************************************

#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"
//#include "am_util_delay.h"
#include "tf_adc.h"
#include "tf_accelerometer.h"
#include "communication.h"

#define TIME_DELAY_MILISECONDS 5000
#define TIME_DELAY_COMM_SET_UP 25

static int  boardSetup(void);
static void boardTeardown(void);
static int  testADC(void);

axis3bit16_t data_raw_acceleration;
float acceleration_mg[3];

//*****************************************************************************
//
//  GPIO pin 1.
//
//*****************************************************************************
#define AM_BSP_GPIO_1                   1
extern const am_hal_gpio_pincfg_t       g_AM_BSP_GPIO_1;

//*****************************************************************************
//
// Main function.
//
//*****************************************************************************
int main(void)
{
    boardSetup();

    am_util_stdio_terminal_clear();

    am_util_stdio_printf("SparkFun Edge Board Test\n\n");

    am_util_stdio_printf("Initializing accelerometer... \r\n");
    int accInitRes = initAccelerometer();
    am_util_stdio_printf("Accelerometer init returned %8x\r\n\n", accInitRes);

    testADC();
    am_hal_gpio_output_clear(AM_BSP_GPIO_1);
    am_hal_gpio_output_clear(AM_BSP_GPIO_LED_RED);
    am_hal_gpio_output_clear(AM_BSP_GPIO_LED_BLUE);
    am_hal_gpio_output_clear(AM_BSP_GPIO_LED_YELLOW);
    
    am_util_stdio_printf("Interval between readings: ~%d", TIME_DELAY_MILISECONDS);
    am_util_stdio_printf("ms\r\n\n");

    am_util_stdio_printf("AccX [mg], AccY [mg], AccZ [mg]\r\n");

    /*
    * Read samples in polling mode (no int)
    */
    while(1)
    {
        // Use Button 14 to break the loop and shut down
        uint32_t pin14Val = 1; 
        am_hal_gpio_state_read( AM_BSP_GPIO_14, AM_HAL_GPIO_INPUT_READ, &pin14Val);
        if( pin14Val == 0 ){ break; }

        lis2dh12_reg_t reg;

        /*
        * Read output only if new value available
        */
        lis2dh12_xl_data_ready_get(&dev_ctx, &reg.byte);
        if (reg.byte)
        {
            am_hal_gpio_output_set(AM_BSP_GPIO_1);
            am_hal_gpio_output_set(AM_BSP_GPIO_LED_BLUE);
            am_util_delay_ms(TIME_DELAY_COMM_SET_UP);

            /* Read accelerometer data */
            memset(data_raw_acceleration.u8bit, 0x00, 3*sizeof(int16_t));
            lis2dh12_acceleration_raw_get(&dev_ctx, data_raw_acceleration.u8bit);
            acceleration_mg[0] =
            lis2dh12_from_fs2_hr_to_mg(data_raw_acceleration.i16bit[0]);
            acceleration_mg[1] =
            lis2dh12_from_fs2_hr_to_mg(data_raw_acceleration.i16bit[1]);
            acceleration_mg[2] =
            lis2dh12_from_fs2_hr_to_mg(data_raw_acceleration.i16bit[2]);
            
            am_util_stdio_printf("%04.2f ,%04.2f ,%04.2f \r\n",
                    acceleration_mg[0], acceleration_mg[1], acceleration_mg[2] );

            am_util_delay_ms(TIME_DELAY_COMM_SET_UP);
            am_hal_gpio_output_clear(AM_BSP_GPIO_1);
            am_hal_gpio_output_clear(AM_BSP_GPIO_LED_BLUE);

            am_util_delay_ms(TIME_DELAY_MILISECONDS - (2*TIME_DELAY_COMM_SET_UP));


        }

        
        
    }

    // Turn off leds
    boardTeardown();

    // Disable debug
    am_bsp_debug_printf_disable();
    
    // Go to Deep Sleep.
    am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_DEEP);
}

static int boardSetup(void)
{
    // Set the clock frequency.
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_SYSCLK_MAX, 0);

    // Set the default cache configuration
    am_hal_cachectrl_config(&am_hal_cachectrl_defaults);
    am_hal_cachectrl_enable();

    // Configure the board for low power operation.
    am_bsp_low_power_init();

/*
    // Initialize the printf interface for ITM/SWO output.
    am_bsp_uart_printf_enable(); // Enable UART - will set debug output to UART
    //am_bsp_itm_printf_enable(); // Redirect debug output to SWO
*/
    ////////////////////
    // Testing new uart
    ////////////////////
    
    //
    // Initialize the printf interface for UART output.
    //
    CHECK_ERRORS(am_hal_uart_initialize(0, &phUART));
    CHECK_ERRORS(am_hal_uart_power_control(phUART, AM_HAL_SYSCTRL_WAKE, false));
    CHECK_ERRORS(am_hal_uart_configure(phUART, &g_sUartConfig));

    //
    // Enable the UART pins.
    //
    am_hal_gpio_pinconfig(AM_BSP_GPIO_COM_UART_TX, g_AM_BSP_GPIO_COM_UART_TX);
    am_hal_gpio_pinconfig(AM_BSP_GPIO_COM_UART_RX, g_AM_BSP_GPIO_COM_UART_RX);

    //
    // Enable interrupts.
    //
    NVIC_EnableIRQ((IRQn_Type)(UART0_IRQn + AM_BSP_UART_PRINT_INST));
    am_hal_interrupt_master_enable();

    //
    // Set the main print interface to use the UART print function we defined.
    //
    am_util_stdio_printf_init(uart_print);

    //
    // Print the banner.
    //
    am_util_stdio_terminal_clear();
    am_util_stdio_printf("Hello World!\n\n");
    
    ///////////////////////
    //end testing new uart
    //////////////////////
    
    // Setup LED's as outputs
    am_hal_gpio_pinconfig(AM_BSP_GPIO_LED_RED, g_AM_HAL_GPIO_OUTPUT_12);
    am_hal_gpio_pinconfig(AM_BSP_GPIO_LED_BLUE, g_AM_HAL_GPIO_OUTPUT_12);
    am_hal_gpio_pinconfig(AM_BSP_GPIO_LED_GREEN, g_AM_HAL_GPIO_OUTPUT_12);
    am_hal_gpio_pinconfig(AM_BSP_GPIO_LED_YELLOW, g_AM_HAL_GPIO_OUTPUT_12);

    // Set up button 14 as input (has pullup resistor on hardware)
    am_hal_gpio_pinconfig(AM_BSP_GPIO_14, g_AM_HAL_GPIO_INPUT);

    // Set up Pin 1 as output
    am_hal_gpio_pinconfig(AM_BSP_GPIO_1, g_AM_HAL_GPIO_OUTPUT_12);

    // Turn on the LEDs
    am_hal_gpio_output_set(AM_BSP_GPIO_LED_RED);
    am_hal_gpio_output_set(AM_BSP_GPIO_LED_BLUE);
    am_hal_gpio_output_set(AM_BSP_GPIO_LED_GREEN);
    am_hal_gpio_output_set(AM_BSP_GPIO_LED_YELLOW);

    am_hal_gpio_output_set(AM_BSP_GPIO_1);

    return 0;
}

static void boardTeardown(void)
{
    // Lights out
    am_hal_gpio_output_clear(AM_BSP_GPIO_LED_RED);
    am_hal_gpio_output_clear(AM_BSP_GPIO_LED_BLUE);
    am_hal_gpio_output_clear(AM_BSP_GPIO_LED_GREEN);
    am_hal_gpio_output_clear(AM_BSP_GPIO_LED_YELLOW);

    am_hal_gpio_output_clear(AM_BSP_GPIO_1);
}

static int testADC(void)
{
    initADC();
    enableAdcInterrupts();
    //triggerAdc();

    return 0;
}

//*****************************************************************************
//
//  GPIO pin 1.
//
//*****************************************************************************
const am_hal_gpio_pincfg_t g_AM_BSP_GPIO_1 =
{
    .uFuncSel            = AM_HAL_PIN_1_GPIO,
    .eDriveStrength      = AM_HAL_GPIO_PIN_DRIVESTRENGTH_2MA
};
