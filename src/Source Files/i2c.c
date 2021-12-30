/**
 * @file
 * i2c.c
 * @author
 * Tanner Leise
 * @date
 * 9/28/21
 * @brief
 * Generic Driver for i2c
 *
 */
//***********************************************************************************
// Include files
//***********************************************************************************
#include "i2c.h"
//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// Private variables
//***********************************************************************************
static I2C_STATE_MACHINE i2c0_state;
static I2C_STATE_MACHINE i2c1_state;
//***********************************************************************************
// Private functions
//***********************************************************************************
/***************************************************************************//**
 * @brief
 * This is the function handler for when an ack interrupt is received
 *
 *
 * @details
 * this is a state machine that handles the ack interrupt. If we are in the state
 * initialize_write of our state machine, then we are going to send the register address
 * and set to the next state communicate state.
 *
 * If in communicate state, then we are going to send a start command, send a write command
 * with the device_address. Then we are going to increment the state to initialize read.
 *
 *
 *
 * @note
 * called when ack interrupts
 *
 *
 * @param[in]
 * I2C_STATE_MACHINE *i2c_sm
 * Holds information needed for the state machine
 ******************************************************************************/
static void ack_func(I2C_STATE_MACHINE *i2c_sm){
  switch(i2c_sm->current_state){
//--------------------------------
    case initialize_write:
      //send register address
      i2c_sm->i2cx->TXDATA = i2c_sm->register_address;
      if(i2c_sm->mode == 0){
          i2c_sm->current_state = write_data;
      }
      else if(i2c_sm->mode == 1){
          i2c_sm->current_state = communicate_register;
      }
      break;
//---------------------------------
    case communicate_register:
      i2c_sm->i2cx->CMD = I2C_CMD_START;
      i2c_sm->i2cx->TXDATA = (i2c_sm->device_address << 1 | i2c_sm->mode);
      i2c_sm->current_state = initialize_read;
      break;
//---------------------------------
    case initialize_read:
      break;
//---------------------------------
      //code here to write data
    case write_data:
      i2c_sm->bytes_expected--;
      //*(i2c_sm->data) &= ~(0xFF<<(8*i2c_sm->bytes_expected));
      i2c_sm->i2cx->TXDATA = ((*(i2c_sm->data) >> (8*i2c_sm->bytes_expected)) & 0xFF);

      if (i2c_sm->bytes_expected == 0){
          i2c_sm->i2cx->CMD = I2C_CMD_STOP;
          i2c_sm->current_state = receive_data;
          break;
      }
      break;
//---------------------------------
    case receive_data:
      break;
//---------------------------------
    case verify_stop:
    default:
      EFM_ASSERT(false);
      break;
  }
}

/***************************************************************************//**
 * @brief
 * This is the function handler for when an rxdatav interrupt is received
 *
 *
 * @details
 * this is a state machine that handles the rxdatav interrupt. If we are in the state
 * initialize_read, then we are going to decrement the bytes expected, clear the
 * data variable which is a member of the i2c sm input into the function. We then
 * set the data member equal to the data in the RXdata. If the bytes expected is
 * not equal to 0 then we send an ack to the device and dont change states. If we got all the bytes expected,
 * then we send a nack, stop command, then move to the reveive data state.
 *
 *
 *
 *
 * @note
 * called when rxdatav interrupts
 *
 *
 * @param[in]
 * I2C_STATE_MACHINE *i2c_sm
 * Holds information needed for the state machine
 *
 ******************************************************************************/

static void rxdatav_func(I2C_STATE_MACHINE *i2c_sm){
  switch(i2c_sm->current_state){

//-------------------------------------
    case initialize_write:
      EFM_ASSERT(false);
      break;
//---------------------------------------
    case communicate_register:
      EFM_ASSERT(false);
      break;
//-------------------------------------
    case initialize_read:
      i2c_sm->bytes_expected--;
      *(i2c_sm->data) &= ~(0xFF<<(8*i2c_sm->bytes_expected));
      *(i2c_sm->data) |= (i2c_sm->i2cx->RXDATA <<(8*i2c_sm->bytes_expected));

      if (i2c_sm->bytes_expected != 0){
          i2c_sm->i2cx->CMD = I2C_CMD_ACK;    //Send ack
          break;
      }
      i2c_sm->i2cx->CMD = I2C_CMD_NACK;
      i2c_sm->i2cx->CMD = I2C_CMD_STOP;
      i2c_sm->current_state = receive_data;
      break;
//--------------------------------------

    case receive_data:
//-------------------------------------
    case verify_stop:
    default:
      EFM_ASSERT(false);
      break;
  }
}

/***************************************************************************//**
 * @brief
 * This is the function handler for when an stop interrupt is received
 *
 *
 * @details
 * this is a state machine that handles the stop interrupt. When this is recieved
 * we unblock sleep, set the gecko to available, set the state back to initialize_write,
 * then finally add the i2c_callback.
 *
 *
 *
 *
 * @note
 * called when stop interrupts
 *
 *
 * @param[in]
 * I2C_STATE_MACHINE *i2c_sm
 * Holds information needed for the state machine
 *
 ******************************************************************************/

static void stop_func(I2C_STATE_MACHINE *i2c_sm){
  switch(i2c_sm->current_state){

//-------------------------------------
    case initialize_write:
      EFM_ASSERT(false);
      break;
//---------------------------------------
    case communicate_register:
      EFM_ASSERT(false);
      break;
//-------------------------------------
    case initialize_read:
      EFM_ASSERT(false);
      break;
//--------------------------------------
    case receive_data:
          sleep_unblock_mode(I2C_EM_BLOCK);
          i2c_sm->i2c_available = true;
          i2c_sm->current_state = initialize_write;
          add_scheduled_event(i2c_sm->i2c_call_back);
     break;
//-------------------------------------
    case verify_stop:
      EFM_ASSERT(false);
      break;

    default:
      EFM_ASSERT(false);
      break;
}
}
/***************************************************************************//**
 * @brief
 * This resets the i2c bus
 *
 *
 * @details
 * resets the i2c peripheral state machine, saves the state of interrupts then
 * clears the flags and buffers. It then sends a stat and stop command in accordance to the data sheet.
 *
 *
 *
 *
 *
 * @note
 * called in i2c open
 *
 *
 * @param[in]
 * I2C_TypeDef *i2c
 * This typedef is specific to I2C0 or I2C1
 *
 ******************************************************************************/
static void i2c_bus_reset(I2C_TypeDef *i2c){
  uint32_t save_state;    //variable to save the state of interrupt

  i2c->CMD = I2C_CMD_ABORT; //reset the micro-controller I2C peripheral state machine
  save_state = i2c->IEN;   //Saves the state of the IEN register
  i2c->IEN = 0;            //Clears the interrupts in the IEN
  i2c->IFC = i2c->IF;      //Clear the interrupt flags
  i2c->CMD = I2C_CMD_CLEARTX;   //clears the buffer
  i2c->CMD = (I2C_CMD_START |  I2C_CMD_STOP); //Starts then stops in accordance to datasheet

  while(!(i2c->IF & I2C_IF_MSTOP)); //Hold until the STOP has been completed by checking the MSTOP bit

  i2c->IFC = i2c->IF;      //Clear the interrupt flags that may have been caused by start stop
  i2c->CMD = I2C_CMD_ABORT; //reset the micro-controller I2C peripheral state machine
  i2c->IEN = save_state;   //Saves the state of the IEN register

}



//***********************************************************************************
// Global functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 * this enables the clock for i2c and sets a local type def which is input into function
 *
 *
 * @details
 * Checks for the i2c input and sets the i2c accordingly. we then set the local type def to
 * the values input to the function. We then set the route locations and enable
 * interrupts int IEN and the NVIC.
 *
 *
 *
 * @note
 * Called in si1133_i2c_open
 *
 *
 * @param[in]
 * I2C_TypeDef *i2c
 * This typedef is specific to I2C0 or I2C1
 *
 * @param[in]
 * I2C_OPEN_STRUCT *i2c_init
 * Holds information needed for the state machine
 *
 *
 ******************************************************************************/
void i2c_open(I2C_TypeDef *i2c, I2C_OPEN_STRUCT *i2c_init){
  I2C_Init_TypeDef i2c_values;


  if(i2c == I2C0){          //Enables I2C0 if input
      CMU_ClockEnable(cmuClock_I2C0, true);
      i2c0_state.i2c_available = true;
  }

  else if(i2c == I2C1){         //Enables I2C1 if input
      CMU_ClockEnable(cmuClock_I2C1, true);
      i2c1_state.i2c_available = true;
  }

  //Checks for proper clock operation
  if ((i2c->IF & 0x01) == 0) {
      i2c->IFS = 0x01;
      EFM_ASSERT(i2c->IF & 0x01);
      i2c->IFC = 0x01;
  }
  else {
      i2c->IFC = 0x01;
      EFM_ASSERT(!(i2c->IF & 0x01));
  }

  //Sets local type def to the ones defined in (most likely) app.c
  i2c_values.clhr =     i2c_init->clhr;
  i2c_values.freq =     i2c_init->freq;
  i2c_values.master =   i2c_init->master;
  i2c_values.refFreq =  i2c_init->refFreq;
  i2c_values.enable =   i2c_init->enable;

  //Initializes the  struct
  I2C_Init(i2c, &i2c_values);


  i2c->ROUTELOC0 = i2c_init->scl_out_route0 |i2c_init->sda_out_route0;

  i2c->ROUTEPEN |= (I2C_ROUTEPEN_SCLPEN * i2c_init->out_scl_en);
  i2c->ROUTEPEN |= (I2C_ROUTEPEN_SDAPEN * i2c_init->out_sda_en);

  i2c->IEN |= (I2C_IEN_ACK * i2c_init->ack_irq_enable);
  i2c->IEN |= (I2C_IEN_RXDATAV * i2c_init->rxdatav_irq_enable);
  i2c->IEN |= (I2C_IEN_MSTOP * i2c_init->stop_irq_enable);

  if(i2c == I2C0){
     NVIC_EnableIRQ(I2C0_IRQn);
   }

  else if(i2c == I2C1){
     NVIC_EnableIRQ(I2C1_IRQn);
  }

  i2c_bus_reset(i2c);
}
/***************************************************************************//**
 * @brief
 * sets up the i2c for the device needed
 *
 *
 * @details
 * Checks what I2C was input into the function then sets a local pointer equal to that.
 * We then block sleep mode and set availability to false.After, we set up the local
 * I2C_STATE_MACHINE variable to the input values and set values for the i2c.
 * We then set the current state to initialize write. Finally we send a start command to the i2c
 * and then send the device address and then send a write command since the i2c
 * always starts out writing.
 *
 *
 *
 *
 * @note
 * Called in Si1133_read
 *
 *
 * @param[in]
 * bool mode
 * This tells us if we are reading or writing.
 *
 *
 * @param[in]
 * uint32_t *data
 * This is the variable used to hold the data.
 *
 * @param[in]
 * uint32_t bytes_expected
 * This is how many bytes we expect.
 *
 * @param[in]
 * uint32_t device_address
 * This is the device address we are talking to.
 *
 * @param[in]
 * uint32_t register_address
 * The register we are communicating with.
 *
 * @param[in]
 * I2C_TypeDef *i2cx
 * This is the i2c we are using, either I2C1 or I2C0.
 *
 * @param[in]
 * uint32_t call_back
 * This is the call back function we are using.
 ******************************************************************************/
//Check to see if we need two different structs according to which i2c we are using
void i2c_start(bool mode, uint32_t *data, uint32_t bytes_expected, uint32_t device_address, uint32_t register_address, I2C_TypeDef *i2cx, uint32_t call_back){
  I2C_STATE_MACHINE *i2c_local_sm;

  if(i2cx == I2C0){
      i2c_local_sm = &i2c0_state;
  }
  else if(i2cx == I2C1){
      i2c_local_sm = &i2c1_state;
  }
  else{
      EFM_ASSERT(false);
  }

  while(!i2c_local_sm->i2c_available);
  EFM_ASSERT((i2cx->STATE & _I2C_STATE_STATE_MASK) == I2C_STATE_STATE_IDLE);

  sleep_block_mode(I2C_EM_BLOCK);
  i2c_local_sm->i2c_available = false;

  i2c_local_sm->mode = mode;
  i2c_local_sm->data = data;
  i2c_local_sm->bytes_expected = bytes_expected;
  i2c_local_sm->device_address = device_address;
  i2c_local_sm->register_address = register_address;
  i2c_local_sm->i2cx = i2cx;
  i2c_local_sm->i2c_call_back = call_back;



  i2c_local_sm->current_state = initialize_write;    //Sets the current state to 0

  i2cx->CMD = I2C_CMD_START;
  i2cx->TXDATA = (i2c_local_sm->device_address << 1 | 0);
}

/***************************************************************************//**
 * @brief
 * Checks if the i2c is available
 *
 * @details
 * takes in which i2c used, returns the whether or not the i2c is available.
 *
 *
 * @note
 * used multiple times in si1133_config.
 *
 ******************************************************************************/
bool is_available(I2C_TypeDef *i2c){
  if(i2c == I2C0){
      return(i2c0_state.i2c_available);
  }
  if(i2c == I2C1){         //Enables I2C1 if input
      return(i2c1_state.i2c_available);
  }
      EFM_ASSERT(false);
      return(false);
  }
/***************************************************************************//**
 * @brief
 * The IRQ handler for i2c0
 *
 *
 * @details
 * This checks whether an ack, rxdatav, or stop interrupt wasa triggered, then
 * sends us to the function handler.
 *
 *
 *
 * @note
 * Used to properly guide the state machine.
 *
 *
 *
 ******************************************************************************/
void I2C0_IRQHandler(){
  uint32_t int_flag = I2C0->IF & I2C0->IEN;
  I2C0->IFC = int_flag;

  if(int_flag & I2C_IF_ACK){
      ack_func(&i2c0_state);
  }

  if(int_flag & I2C_IF_RXDATAV){
      rxdatav_func(&i2c0_state);
  }
  if(int_flag & I2C_IF_MSTOP){
      stop_func(&i2c0_state);
  }
}
/***************************************************************************//**
 * @brief
 * The IRQ handler for i2c1
 *
 *
 * @details
 * This checks whether an ack, rxdatav, or stop interrupt wasa triggered, then
 * sends us to the function handler.
 *
 *
 *
 * @note
 * Used to properly guide the state machine.
 *
 *
 *
 ******************************************************************************/
void I2C1_IRQHandler(){
  uint32_t int_flag = I2C1->IF & I2C1->IEN;
   I2C1->IFC = int_flag;

   if(int_flag & I2C_IF_ACK){
       ack_func(&i2c1_state);
   }

   if(int_flag & I2C_IF_RXDATAV){
       rxdatav_func(&i2c1_state);
   }

   if(int_flag & I2C_IF_MSTOP){
       stop_func(&i2c1_state);
   }
 }
