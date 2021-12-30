/**
 * @file
 * LEDs_thunderboard.c
 * @author
 * Tanner Leise
 * @date
 * 9/25/21
 * @brief
 * File for communicating with the LEDs on board
 *
 */


//***********************************************************************************
// Include files
//***********************************************************************************
#include "LEDs_thunderboard.h"

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// Private variables
//***********************************************************************************
bool	rgb_enabled_status;

//***********************************************************************************
// Private functions
//***********************************************************************************


//***********************************************************************************
// Global functions
//***********************************************************************************
/***************************************************************************//**
 * @brief
 * Initializes the pins our LEDs
 *
 *
 * @details
 * Clears the pin out for each LED on the board. It then enables the RGB port and pin.
 *
 *
 *
 *
 * @note
 * This is called in our led_open function
 ******************************************************************************/
void rgb_init(void) {
	rgb_enabled_status = false;
  GPIO_PinOutClear(RGB0_PORT,RGB0_PIN);
  GPIO_PinOutClear(RGB1_PORT,RGB1_PIN);
  GPIO_PinOutClear(RGB2_PORT,RGB2_PIN);
  GPIO_PinOutClear(RGB3_PORT,RGB3_PIN);
  GPIO_PinOutSet(RGB_ENABLE_PORT,RGB_ENABLE_PIN);
}
/***************************************************************************//**
 * @brief
 * Turns on or off color in LEDs
 *
 *
 * @details
 * This function takes in the LED that we want to talk to, the color we want to set
 * and whether we want to turn it on or off.
 *
 *
 *
 *
 * @note
 * Called in the callback functions.
 *
 * @param[in] leds
 * This input is the LED which u want to communicate with
 *
 *
 * @param[in] color
 * The color you want to toggle
 *
 *
 * @param[in] enable
 * This is either true of false. If true, turn it on and if false turn it off.
 ******************************************************************************/

void leds_enabled(uint32_t leds, uint32_t color, bool enable){

    if ((color & COLOR_RED) && enable) {
      GPIO_PinOutSet(RGB_RED_PORT,RGB_RED_PIN);
    } else if ((color & COLOR_RED) && !enable) GPIO_PinOutClear(RGB_RED_PORT,RGB_RED_PIN);

    if ((color & COLOR_GREEN) && enable) {
      GPIO_PinOutSet(RGB_GREEN_PORT,RGB_GREEN_PIN);
    } else if ((color & COLOR_GREEN) && !enable)  GPIO_PinOutClear(RGB_GREEN_PORT,RGB_GREEN_PIN);

    if ((color & COLOR_BLUE) && enable) {
      GPIO_PinOutSet(RGB_BLUE_PORT,RGB_BLUE_PIN);
    } else if ((color & COLOR_BLUE) && !enable)  GPIO_PinOutClear(RGB_BLUE_PORT,RGB_BLUE_PIN);

		if ((leds & RGB_LED_0) && enable) {
			GPIO_PinOutSet(RGB0_PORT,RGB0_PIN);
		} else if ((leds & RGB_LED_0) && !enable) GPIO_PinOutClear(RGB0_PORT,RGB0_PIN);

		if ((leds & RGB_LED_1) && enable) {
			GPIO_PinOutSet(RGB1_PORT,RGB1_PIN);
		} else if ((leds & RGB_LED_1) && !enable)  GPIO_PinOutClear(RGB1_PORT,RGB1_PIN);

		if ((leds & RGB_LED_2) && enable) {
			GPIO_PinOutSet(RGB0_PORT,RGB2_PIN);
		} else if ((leds & RGB_LED_2) && !enable)  GPIO_PinOutClear(RGB2_PORT,RGB2_PIN);

		if ((leds & RGB_LED_3) && enable) {
			GPIO_PinOutSet(RGB3_PORT,RGB3_PIN);
		} else if ((leds & RGB_LED_3) && !enable)  GPIO_PinOutClear(RGB3_PORT,RGB3_PIN);

}

