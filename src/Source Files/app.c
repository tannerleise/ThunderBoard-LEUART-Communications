/**
 * @file
 * app.c
 * @author
 * Tanner Leise
 * @date
 * 9/8/21
 * @brief
 * Initializes all peripherals and initializes LETIMER0 for PWM operation
 *
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#include "app.h"


//***********************************************************************************
// defined files
//***********************************************************************************
  //#define BLE_TEST_ENABLED

//***********************************************************************************
// Private variables
//***********************************************************************************
static uint32_t x = 3;
static uint32_t y = 0;
static int LED_COLOR;

//***********************************************************************************
// Private functions
//***********************************************************************************

static void app_letimer_pwm_open(float period, float act_period, uint32_t out0_route, uint32_t out1_route);

//***********************************************************************************
// Global functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 * Initializes all peripherals
 *
 *
 * @details
 * Calls all of the other functions in their respective drivers. Specifically
 * it sets up the cmu and gpio. It also sets up LETIMER0 for pwm operation, then
 * starts our timer.
 *
 *
 * @note
 * This function is to be called in main before we enter our loop of operation.
 * This will be the final step of configuration and setup before actual operation
 * begins.
 *
 ******************************************************************************/

void app_peripheral_setup(void){
  scheduler_open();    //I put it before everything because if the timer starts we may have an interrupt B4 we are set up
  sleep_open();
  cmu_open();
  gpio_open();
  Si1133_i2c_open();
  led_color_open();
  sleep_block_mode(SYSTEM_BLOCK_EM);
  ble_open(0,0);
  app_letimer_pwm_open(PWM_PER, PWM_ACT_PER, PWM_ROUTE_0, PWM_ROUTE_1);
  add_scheduled_event(BOOT_UP_CB); //check this position once we know what boot up does
}

/***************************************************************************//**
 * @brief
 * Initializes LETIMER0 for PWM operation
 *
 *
 * @details
 * Accesses members of the APP_LETIMER_PWM_TypeDef of the local variable
 * app_letimer_pwm_struct and sets it for the specific needs of the pwm signal.
 * That structure is then passed into the pwm open function written in
 * letimer.c. Refer to more detail there.
 *
 *
 *
 * @note
 *  This function should be called in the application file. It is called to
 *  set up and enable specific LETIMER for the user.
 *
 * @param[in] period
 * The overall period of the PWM waveform.
 *
 *
 *
 *
 * @param[in] act_period
 * This is the on period of the active waveform
 *
 ******************************************************************************/

void app_letimer_pwm_open(float period, float act_period, uint32_t out0_route, uint32_t out1_route){
  // Initializing LETIMER0 for PWM operation by creating the
  // letimer_pwm_struct and initializing all of its elements
  // APP_LETIMER_PWM_TypeDef is defined in letimer.h
  APP_LETIMER_PWM_TypeDef   app_letimer_pwm_struct;
  app_letimer_pwm_struct.active_period      = act_period;
  app_letimer_pwm_struct.debugRun           = false;
  app_letimer_pwm_struct.enable             = false;
  app_letimer_pwm_struct.out_pin_0_en       = true;   //We want pin 0 on
  app_letimer_pwm_struct.out_pin_1_en       = true;   //We want pin 1 on
  app_letimer_pwm_struct.out_pin_route0     = out0_route;
  app_letimer_pwm_struct.out_pin_route1     = out1_route;
  app_letimer_pwm_struct.period             = period;
  app_letimer_pwm_struct.comp0_irq_enable   = false;            //false means disable
  app_letimer_pwm_struct.comp0_cb           = LETIMER0_COMP0_CB;
  app_letimer_pwm_struct.comp1_irq_enable   = true;             //true means enable
  app_letimer_pwm_struct.comp1_cb           = LETIMER0_COMP1_CB;
  app_letimer_pwm_struct.uf_irq_enable      = true;
  app_letimer_pwm_struct.uf_cb              = LETIMER0_UF_CB;




  letimer_pwm_open(LETIMER0, &app_letimer_pwm_struct);
}
/***************************************************************************//**
 * @brief
 *  Sets the static variable for LED color and initializes the the LEDs
 *
 *
 * @details
 * Sets LED_COLOR = 0 and calls rgb_init() which sets up the out pins for the LEDs
 *
 *
 *
 *
 * @note
 * This should be called in peripheral setup
 *
 *
 *
 ******************************************************************************/
void led_color_open(void){
  LED_COLOR = 0;
  rgb_init();
}

/***************************************************************************//**
 * @brief
 * The Callback function for the comp0 interrupt. Does nothing in this program
 *
 *
 *
 * @details
 * Does nothing in this program
 *
 *
 *
 *
 *
 * @note
 * N/A
 ******************************************************************************/
void scheduled_letimer0_comp0_cb(void){
  //EFM_ASSERT(false);    //For now we don't use this, if we get here we done goofed
  //EFM_ASSERT(!(get_scheduled_events() & LETIMER0_COMP0_CB));
}
/***************************************************************************//**
 * @brief
 * The callback function for the comp1 interrupt that causes a sense of the Si1133
 *
 *
 *
 * @details
 * This calls the force_sense function
 *
 *
 *
 *
 * @note
 * Called when the comp1 interrupt is triggered
 *
 ******************************************************************************/
void scheduled_letimer0_comp1_cb(void){
    Si1133_force_sense();
  }
/***************************************************************************//**
 * @brief
 * This requests the result from the SI1133
 *
 *
 *
 * @details
 * calls Si1133_request_result then does the math with x, y, and z
 *
 *
 *
 *
 *
 * @note
 * Called when the uf interrupt is triggered
 ******************************************************************************/
void scheduled_letimer0_uf_cb(void){
  //EFM_ASSERT(!(get_scheduled_events() & LETIMER0_UF_CB));
  Si1133_request_result(SI1133_REG_READ_CB);
   x = x+3;
   y = y+1;
   float z = (float) x/y;

   char data[60];
   sprintf(data, "z = %.1f\n", z);
   ble_write(data);
}
/***************************************************************************//**
 * @brief
 * This compares the data received by the S1133 turns blue if it is dark
 * it turns off when it is light. Also transmits "its dark" and the reading when
 * the data is less than our expected value. If it is greater than or equal to our expected value it
 * transmits the data and days "its light outside"
 *
 *
 *
 * @details
 * Compares the data read from the si1133 to the the value of 20 which
 * was provided as the number needed in the lab doc. If is is less than 20
 * then we turn the blue led on. If the read data is greater than 20, then we
 * turn the led off.  Also transmits "its dark" and the reading when
 * the data is less than our expected value. If it is greater than or equal to our expected value it
 * transmits the data and days "its light outside"
 *
 *
 * @note
 * This callback is triggered after the comp1 interrupt has been triggered
 ******************************************************************************/
void scheduled_si1133_read_cb(void){
  uint32_t read_data;
  read_data = result_read();

  if(read_data < EXPECTED_VALUE){
      leds_enabled(RGB_LED_1, COLOR_BLUE, true);
      char data[60];
      int int_data = (int) read_data;
      sprintf(data, "It's dark = %d", int_data);
      ble_write(data);
  }
  else if(read_data >= EXPECTED_VALUE){
      leds_enabled(RGB_LED_1, COLOR_BLUE, false);
      char data[60];
      int int_data = (int) read_data;
      sprintf(data, "It's light outside = %d", int_data);
      ble_write(data);
  }
}


/***************************************************************************//**
 * @brief
 * This is a call back that is called upon start up. If needed it
 * runs the ble test. After, it transmits hello world then starts the LETIMER.
 *
 *
 *
 * @details
 * If requested, sets the board name and then runs the ble test. Then, it transmits the
 * phrase "Hello World". Finally, it starts the LETIMER.
 *
 *
 *
 * @note
 * Called in app_peripheral_setup
 *
 *
 ******************************************************************************/
void scheduled_boot_up_cb(void){
  #ifdef BLE_TEST_ENABLED
  char ble_name[13] =  "TannerCoolBrd";
  EFM_ASSERT(ble_test(ble_name));
  timer_delay(2000);
  #endif
  char data[12] = "Hello World\0";
  ble_write(data);
  letimer_start(LETIMER0, true);  //This command will initiate the start of the LETIMER0

}
/***************************************************************************//**
 * @brief
 * Not used in this applications
 *
 *
 * @details
 *
 *
 *
 * @note
 *
 *
 ******************************************************************************/



void scheduled_ble_tx_done_cb(void){

}
