/**
 * @file
 * gpio.c
 * @author
 * Tanner Leise
 * @date
 * 9/8/21
 * @brief
 * Sets up the GPIO pins for the board
 *
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#include "gpio.h"

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************


//***********************************************************************************
// functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 * Sets up the GPIO pins for the board
 *
 *
 * @details
 * The application code calls this function to set up the GPIO. Specifically
 * the drive strength is se for the Red LED and the Green LED. The function
 * also sets the pin mode for the respective LEDs
 *
 *
 * @note
 *  This function can be called whenever needed to set up the LEDs as it does
 *  not depend on any other functions.
 *
 ******************************************************************************/

void gpio_open(void){

  CMU_ClockEnable(cmuClock_GPIO, true);

	// Configure LED pins
	GPIO_DriveStrengthSet(LED_RED_PORT, LED_RED_DRIVE_STRENGTH);
	GPIO_PinModeSet(LED_RED_PORT, LED_RED_PIN, LED_RED_GPIOMODE, LED_RED_DEFAULT);

	GPIO_DriveStrengthSet(LED_GREEN_PORT, LED_GREEN_DRIVE_STRENGTH);
	GPIO_PinModeSet(LED_GREEN_PORT, LED_GREEN_PIN, LED_GREEN_GPIOMODE, LED_GREEN_DEFAULT);


	// Set RGB LED gpio pin configurations
	GPIO_PinModeSet(RGB_ENABLE_PORT, RGB_ENABLE_PIN, gpioModePushPull, RGB_DEFAULT_OFF);
	GPIO_PinModeSet(RGB0_PORT, RGB0_PIN, gpioModePushPull, RGB_DEFAULT_OFF);
	GPIO_PinModeSet(RGB1_PORT, RGB1_PIN, gpioModePushPull, RGB_DEFAULT_OFF);
	GPIO_PinModeSet(RGB2_PORT, RGB2_PIN, gpioModePushPull, RGB_DEFAULT_OFF);
	GPIO_PinModeSet(RGB3_PORT, RGB3_PIN, gpioModePushPull, RGB_DEFAULT_OFF);
	GPIO_PinModeSet(RGB_RED_PORT, RGB_RED_PIN, gpioModePushPull, COLOR_DEFAULT_OFF);
	GPIO_PinModeSet(RGB_GREEN_PORT, RGB_GREEN_PIN, gpioModePushPull, COLOR_DEFAULT_OFF);
	GPIO_PinModeSet(RGB_BLUE_PORT, RGB_BLUE_PIN, gpioModePushPull, COLOR_DEFAULT_OFF);


//Configure Si1133 (light sensor) pins
  GPIO_DriveStrengthSet(SI1133_SENSOR_EN_PORT, SI1133_DRIVE_STRENGTH);
  GPIO_PinModeSet(SI1133_SENSOR_EN_PORT, SI1133_SENSOR_EN_PIN, gpioModePushPull, SI1133_SENSOR_EN_DEFAULT);

 //Configure SDA and SCL lines
  GPIO_PinModeSet(SI1133_SCL_PORT, SI1133_SCL_PIN, gpioModeWiredAnd, SI1133_SCL_DEFAULT);
  GPIO_PinModeSet(SI1133_SDA_PORT, SI1133_SDA_PIN, gpioModeWiredAnd, SI1133_SDA_DEFAULT);

  //Configure the UART pins
  GPIO_PinModeSet(LEUART_TX_PORT, LEUART_TX_PIN, gpioModePushPull, LEUART_TX_DEFAULT);
  GPIO_PinModeSet(LEUART_RX_PORT, LEUART_RX_PIN, gpioModeInput, LEUART_RX_DEFAULT);

  GPIO_DriveStrengthSet(LEUART_TX_PORT, LEUART_TX_DRIVE_STRENGTH);
}
