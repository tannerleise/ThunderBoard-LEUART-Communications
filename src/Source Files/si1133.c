/**
 * @file
 * si1133.c
 * @author
 * Tanner Leise
 * @date
 * 9/28/21
 * @brief
 * Generic Driver for si1133
 *
 */
//***********************************************************************************
// Include files
//***********************************************************************************
#include "si1133.h"
//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// Private variables
//***********************************************************************************
static uint32_t data;     //This is the read data, poor naming convention
static uint32_t si1133_write_data;
//***********************************************************************************
// Private functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 * Configures the si1133 to read from the light sensor and sets channel 0 as the
 * active channel
 *
 * @details
 * We first reset the reset cmd ctr to prevent any unwanted bugs. Then we read the
 * response0 register which is what holds the cmd ctr. We then store that value
 * for a check later. After, we then write the color of the white light sensor
 * to the input0 register. We then write the address we wish to send the input0 to,
 * This register is the ADC config register.Then we read the cmd counter again,
 * if it has incremented we move on. If not we fail an assert. After, we then write
 * Send the value of channel0 to input0. We then send chan list to the command register
 * to tell the board where to send input0. Finally we read response0 again and ensure
 * that cmd ctr has incremented again.
 *
 *
 *
 *
 * @note
 * Called in Si1133_i2c_open
 *
 ******************************************************************************/

static void si1133_config(){
  uint32_t cmd_ctr;
  //resets the counter, avoids issue where counter is at 14 or 15
  si1133_write_data = RESET_CMD_CTR;
  Si1133_write(1,COMMAND_REG,NULL_CB);
  while(!(is_available(I2C1)));

  //read response0
  Si1133_read(1, RESPONSE0, NULL_CB);

  //wait
  while(!(is_available(I2C1)));

  //store value of cmd_ctr (used later)
  cmd_ctr = data & 0x0F;      //Gets the lower 4 bits of respponse0 aka CMD_CTR



  //All of this is in the first example on page 14
  //Here we write the value to input zero
  si1133_write_data = WHITE;
  Si1133_write(1,INPUT0,NULL_CB);

  //Wait until we finish
  while(!(is_available(I2C1)));

  //Write the param_set and the address of ADC_CONFIG to the command register
  si1133_write_data = PARAM_SET | ADC_CONFIG0;
  Si1133_write(1,COMMAND_REG,NULL_CB);

  //Wait until we finish
  while(!(is_available(I2C1)));

  //read response0, then we check to to see if cmd_ctl has incremented
  Si1133_read(1, RESPONSE0, NULL_CB);
  while(!(is_available(I2C1)));

  if((data & 0x0F) != (cmd_ctr + 1)){
      EFM_ASSERT(false);
  }

  //Preps channel 0 to be active
  si1133_write_data = CHAN0_PREP;
  Si1133_write(1,INPUT0,NULL_CB);

  while(!(is_available(I2C1)));

  //This is what we are writing to the command register
  si1133_write_data = PARAM_SET | CHAN_LIST;
  Si1133_write(1,COMMAND_REG,NULL_CB);

  //wait
  while(!(is_available(I2C1)));

  //Read then check if the cmd_ctr has incremented
  Si1133_read(1, RESPONSE0, NULL_CB);

  while(!(is_available(I2C1)));

  //Problem occurs here, cmd ctr doesn't increment again for some reason
  if((data & 0x0F) != (cmd_ctr + 2)){
      EFM_ASSERT(false);
  }
  return;
}
//***********************************************************************************
// Global functions
//***********************************************************************************
/***************************************************************************//**
 * @brief
 * sets up the struct for the I2C with the SI1133
 *
 *
 * @details
 * creates a local struct for the I2C_OPEN_STRUCT then sets the values of the
 * struct accordingly. Calls I2C open with I2C1 and our local struct as an input
 *
 *
 *
 *
 * @note
 * Used to set up the I2C for the SI1133,called in app.c
 *
 *
 ******************************************************************************/

void Si1133_i2c_open(){
  I2C_OPEN_STRUCT si1133_struct_open;

  timer_delay(25); //delays 25 milliseconds for start up of si1133

  si1133_struct_open.freq = I2C_FREQ_FAST_MAX;
  si1133_struct_open.clhr = i2cClockHLRAsymetric ;
  si1133_struct_open.master = true;
  si1133_struct_open.enable = true;
  si1133_struct_open.out_scl_en = true;
  si1133_struct_open.out_sda_en = true;
  si1133_struct_open.refFreq = 0;         //If master, 0
  si1133_struct_open.scl_out_route0 = I2C_OUT_SCL_PC5;
  si1133_struct_open.sda_out_route0 = I2C_OUT_SDA_PC4;
  si1133_struct_open.ack_irq_enable = true;
  si1133_struct_open.rxdatav_irq_enable = true;
  si1133_struct_open.stop_irq_enable = true;

  i2c_open(I2C1,&si1133_struct_open);
  si1133_config();
}

/***************************************************************************//**
 * @brief
 * Sets up the i2c start function with values specific to the Si1133 for a read
 *
 *
 * @details
 * Calls I2C start and plugs in the values specific to the SI1133
 *
 *
 *
 *
 * @note
 * Called in CB function for comp1
 *
 * @param[in]
 * uint32_t bytes_expected
 * This tells us how many bytes we expect to read
 *
 * @param[in]
 * uint32_t register_address
 * This is the register address we are writing to
 *
 * @param[in]
 * uint32_t call_back
 * This is the call back function we are using for logic
 *
 ******************************************************************************/
void Si1133_read(uint32_t bytes_expected,uint32_t register_address, uint32_t call_back){
  bool mode = 1;
  uint32_t device_address = 0x55;

  i2c_start(mode, &data, bytes_expected, device_address, register_address, I2C1, call_back);

}
/***************************************************************************//**
 * @brief
 * Sets up the i2c start function with values specific to the Si1133 for a write
 *
 *
 * @details
 * Calls I2C start and plugs in the values specific to the SI1133
 *
 *
 *
 *
 * @note
 * Called in CB function for comp1
 *
 * @param[in]
 * uint32_t bytes_expected
 * This tells us how many bytes we expect to read
 *
 * @param[in]
 * uint32_t register_address
 * This is the register address we are writing to
 *
 * @param[in]
 * uint32_t call_back
 * This is the call back function we are using for logic
 *
 ******************************************************************************/

void Si1133_write(uint32_t bytes_expected,uint32_t register_address, uint32_t call_back){
  bool mode = 0;
  uint32_t device_address = 0x55;

  i2c_start(mode, &si1133_write_data, bytes_expected, device_address, register_address, I2C1, call_back);

}
/***************************************************************************//**
 * @brief
 * reads the data collected
 *
 *
 * @details
 * reads data collected
 *
 *
 *
 *
 * @note
 * Called in scheduled_si1133_read_cb
 *
 *
 ******************************************************************************/

uint32_t result_read(){
  return(data);
}


/***************************************************************************//**
 * @brief
 * Sends a force command to the si1133
 *
 * @details
 * writes the static data vairable to 0x11 which is the command for force.
 * Send the force command to the command register.
 *
 *
 *
 * @note
 * Called in comp1 cb
 *
 ******************************************************************************/
void Si1133_force_sense(){

  si1133_write_data = FORCE;
  Si1133_write(1,COMMAND_REG,NULL_CB);

  return;
}

/***************************************************************************//**
 * @brief
 * requests read from the si1133
 *
 * @details
 * Reads 2 bytes from the HOSTOUT0 register
 *
 *
 *
 * @note
 * Called in the ub cb
 *
 ******************************************************************************/
void Si1133_request_result(uint32_t callback){
  Si1133_read(2,HOSTOUT0,callback);
}
