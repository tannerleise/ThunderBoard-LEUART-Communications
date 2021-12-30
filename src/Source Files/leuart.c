/**
 * @file leuart.c
 * @author
 * @date
 * @brief Contains all the functions of the LEUART peripheral
 *
 */

//***********************************************************************************
// Include files
//***********************************************************************************

//** Standard Library includes
#include <string.h>

//** Silicon Labs include files
#include "em_gpio.h"
#include "em_cmu.h"

//** Developer/user include files
#include "leuart.h"
#include "scheduler.h"

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// private variables
//***********************************************************************************
uint32_t	rx_done_evt;
uint32_t	tx_done_evt;
bool		leuart0_tx_busy;

static LEUART_STATE_MACHINE leuart0_state;

/***************************************************************************//**
 * @brief LEUART driver
 * @details
 *  This module contains all the functions to support the driver's state
 *  machine to transmit a string of data across the LEUART bus.  There are
 *  additional functions to support the Test Driven Development test that
 *  is used to validate the basic set up of the LEUART peripheral.  The
 *  TDD test for this class assumes that the LEUART is connected to the HM-18
 *  BLE module.  These TDD support functions could be used for any TDD test
 *  to validate the correct setup of the LEUART.
 *
 ******************************************************************************/

//***********************************************************************************
// Private functions
//***********************************************************************************

/***************************************************************************//**
 *@brief
 * This is the state machine function for writing data when the TXBL interrupt is triggered.
 *
 *
 * @details
 * This is the write data function. It transmits one byte at a time and will trigger
 * until the count is equal to the length. Then it turns off the TBXL interrupt,
 * switches states to stop, and then turns on TXC interrupts.
 *
 *@note
 * called when TXBL is triggered
 *
 *@param[in] leuart_sm
 * This is the state machine struct initialized as a private static variable above.
 *
 ******************************************************************************/
static void write_data_func(LEUART_STATE_MACHINE *leuart_sm){
  switch(leuart_sm->current_state){
//--------------------------------
    case write_data_uart:
      if(leuart_sm->count != leuart_sm->length){
          leuart_app_transmit_byte(leuart_sm->leuart, leuart_sm->data[leuart_sm->count]); //Send data, either char or byte
          leuart_sm->count++;
          break;
      }
    else{
          leuart_sm->leuart->IEN &= ~LEUART_IEN_TXBL;
          leuart_sm->current_state = stop;
          leuart_sm->leuart->IEN |= LEUART_IEN_TXC;
          break;
      }
    case stop:
      EFM_ASSERT(false);
      break;
    default:
      EFM_ASSERT(false);
      break;
  }
}

/***************************************************************************//**
 * @brief
 * This is the stop function for the state machine.
 *
 * @details
 * This turns off the TXC interrupt, switches the state back to write data,
 * changes the device to available, then finally unblocks sleep mode.
 *
 *@note
 * called when TXC is triggered
 *
 *@param[in] leuart_sm
 * This is the state machine struct initialized as a private static variable above.
 *
 ******************************************************************************/
static void stop_func(LEUART_STATE_MACHINE *leuart_sm){
  switch(leuart_sm->current_state){
//--------------------------------
    case write_data_uart:
      EFM_ASSERT(false);
      break;
    case stop:
      leuart_sm->leuart->IEN &= ~LEUART_IEN_TXC;
      leuart_sm->current_state = write_data_uart;
      leuart0_state.available = true;
      sleep_unblock_mode(LEUART_TX_EM);
      break;
//--------------------------------
    default:
      EFM_ASSERT(false);
      break;
  }
}


//***********************************************************************************
// Global functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 * This is the function for leuart, handles routing, enabling the clock, initializes, and enables
 * the leuart.
 *
 *
 * @details
 * Starts by enabling the clock to LEUART, then sets values of the local init typedef.
 * It then initializes the LEUART, then routes the pins for the LEUART. Finally it enables
 * the LEUART and enables the NVIC.
 *
 * @note
 * called by ble open.
 *
 * @param[in] LEUART_TypeDef *leuart
 * This is the type of LEUART we are using.
 *
 * @param[in] LEUART_OPEN_STRUCT *leuart_settings
 * This is the open struct used to set the local typedef to.
 *
 ******************************************************************************/

void leuart_open(LEUART_TypeDef *leuart, LEUART_OPEN_STRUCT *leuart_settings){
  LEUART_Init_TypeDef leuart_values;

   CMU_ClockEnable(cmuClock_LEUART0, true);


    leuart->STARTFRAME = true;  //Writing something to start frame
    while(leuart->SYNCBUSY);    //wait for write operation
    EFM_ASSERT(leuart->STARTFRAME & true);  //verify
    leuart->STARTFRAME = false; //undo what we did
    while(leuart->SYNCBUSY);// wait til complete
    //This is proof that the clock operation is working properly

  LEUART_Enable_TypeDef disableLEUART = leuartDisable;
  //Sets the Init Type Def
  leuart_values.baudrate =      leuart_settings->baudrate;
  leuart_values.databits =      leuart_settings->databits;
  leuart_values.parity   =      leuart_settings->parity;
  leuart_values.refFreq  =      leuart_settings->refFreq;
  leuart_values.enable   =      disableLEUART;
  leuart_values.stopbits =      leuart_settings->stopbits;
  leuart0_state.available = true;

  //Initializes the struct
  LEUART_Init(leuart, &leuart_values);
  while(leuart->SYNCBUSY);



  leuart->ROUTELOC0 = leuart_settings->tx_loc | leuart_settings->rx_loc;


  leuart->ROUTEPEN |= (LEUART_ROUTEPEN_TXPEN * leuart_settings->tx_pin_en);
  leuart->ROUTEPEN |= (LEUART_ROUTEPEN_RXPEN * leuart_settings->rx_pin_en);

  leuart->STARTFRAME = LEUART_RXDATA_RXDATA_DEFAULT;      //Will clear the RxRegister because the start frame sends its contents there
  while(leuart->SYNCBUSY);
  leuart->TXDATA = LEUART_TXDATA_TXDATA_DEFAULT;         //Clears the register

  while(leuart->SYNCBUSY);


  LEUART_Enable(leuart,leuart_settings->enable);
  while(!(leuart->STATUS & LEUART_STATUS_TXENS));
  while(!(leuart->STATUS & LEUART_STATUS_RXENS));
  EFM_ASSERT((leuart->STATUS & (LEUART_STATUS_TXENS | LEUART_STATUS_RXENS))); //Checks if the TX and RX are enabled


  leuart->IFC = _LEUART_IFC_MASK;
  NVIC_EnableIRQ(LEUART0_IRQn);

}

/***************************************************************************//**
 * @brief
 * This is the start function for the leuart. This sets values of the private
 * struct for the state machine.
 *
 *
 * @details
 * This starts by making the function atomic. Then it blocks the sleep mode, and then sets the
 * values of the struct and sets the state to the write state. Finally it copies the input string to our data variable
 * and then turns on the TXBL interrupt.
 *
 * @note
 * called by ble start.
 *
 * @param[in] LEUART_TypeDef *leuart
 * This is the type of LEUART we are using.
 *
 * @param[in] char *string
 * This is the string we want to input.
 *
 * @param[in] uint32_t string_len
 * length of the input string.
 ******************************************************************************/

void leuart_start(LEUART_TypeDef *leuart, char *string, uint32_t string_len){
  while(!(leuart0_state.available));

  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();


  sleep_block_mode(LEUART_TX_EM);


  leuart0_state.available = false;
  leuart0_state.length = string_len;
  leuart0_state.count = 0;
  leuart0_state.leuart = leuart;
  leuart0_state.current_state = write_data_uart;

  strcpy(leuart0_state.data, string);

  leuart->IEN |= LEUART_IEN_TXBL;
  CORE_EXIT_CRITICAL();

}

/***************************************************************************//**
 * @brief
 *
 ******************************************************************************/

//bool leuart_tx_busy(LEUART_TypeDef *leuart){
//}

/***************************************************************************//**
 * @brief
 *   LEUART STATUS function returns the STATUS of the peripheral for the
 *   TDD test
 *
 * @details
 * 	 This function enables the LEUART STATUS register to be provided to
 * 	 a function outside this .c module.
 *
 * @param[in] *leuart
 *   Defines the LEUART peripheral to access.
 *
 * @return
 * 	 Returns the STATUS register value as an uint32_t value
 *
 ******************************************************************************/

uint32_t leuart_status(LEUART_TypeDef *leuart){
	uint32_t	status_reg;
	status_reg = leuart->STATUS;
	return status_reg;
}

/***************************************************************************//**
 * @brief
 *   LEUART CMD Write sends a command to the CMD register
 *
 * @details
 * 	 This function is used by the TDD test function to program the LEUART
 * 	 for the TDD tests.
 *
 * @note
 *   Before exiting this function to update  the CMD register, it must
 *   perform a SYNCBUSY while loop to ensure that the CMD has by synchronized
 *   to the lower frequency LEUART domain.
 *
 * @param[in] *leuart
 *   Defines the LEUART peripheral to access.
 *
 * @param[in] cmd_update
 * 	 The value to write into the CMD register
 *
 ******************************************************************************/

void leuart_cmd_write(LEUART_TypeDef *leuart, uint32_t cmd_update){

	leuart->CMD = cmd_update;
	while(leuart->SYNCBUSY);
}

/***************************************************************************//**
 * @brief
 *   LEUART IF Reset resets all interrupt flag bits that can be cleared
 *   through the Interrupt Flag Clear register
 *
 * @details
 * 	 This function is used by the TDD test program to clear interrupts before
 * 	 the TDD tests and to reset the LEUART interrupts before the TDD
 * 	 exits
 *
 * @param[in] *leuart
 *   Defines the LEUART peripheral to access.
 *
 ******************************************************************************/

void leuart_if_reset(LEUART_TypeDef *leuart){
	leuart->IFC = 0xffffffff;
}

/***************************************************************************//**
 * @brief
 *   LEUART App Transmit Byte transmits a byte for the LEUART TDD test
 *
 * @details
 * 	 The BLE module will respond to AT commands if the BLE module is not
 * 	 connected to the phone app.  To validate the minimal functionality
 * 	 of the LEUART peripheral, write and reads to the LEUART will be
 * 	 performed by polling and not interrupts.
 *
 * @note
 *   In polling a transmit byte, a while statement checking for the TXBL
 *   bit in the Interrupt Flag register is required before writing the
 *   TXDATA register.
 *
 * @param[in] *leuart
 *   Defines the LEUART peripheral to access.
 *
 * @param[in] data_out
 *   Byte to be transmitted by the LEUART peripheral
 *
 ******************************************************************************/

void leuart_app_transmit_byte(LEUART_TypeDef *leuart, uint8_t data_out){
	while (!(leuart->IF & LEUART_IF_TXBL));
	leuart->TXDATA = data_out;
}


/***************************************************************************//**
 * @brief
 *   LEUART App Receive Byte polls a receive byte for the LEUART TDD test
 *
 * @details
 * 	 The BLE module will respond to AT commands if the BLE module is not
 * 	 connected to the phone app.  To validate the minimal functionality
 * 	 of the LEUART peripheral, write and reads to the LEUART will be
 * 	 performed by polling and not interrupts.
 *
 * @note
 *   In polling a receive byte, a while statement checking for the RXDATAV
 *   bit in the Interrupt Flag register is required before reading the
 *   RXDATA register.
 *
 * @param[in] leuart
 *   Defines the LEUART peripheral to access.
 *
 * @return
 * 	 Returns the byte read from the LEUART peripheral
 *
 ******************************************************************************/

uint8_t leuart_app_receive_byte(LEUART_TypeDef *leuart){
	uint8_t leuart_data;
	while (!(leuart->IF & LEUART_IF_RXDATAV));
	leuart_data = leuart->RXDATA;
	return leuart_data;
}

/***************************************************************************//**
 * @brief
 * This is the IRQ handler for the LEUART
 *
 * @details
 * This saves the flags that are raised then clears the flags. It then checks the flags,
 * if the TXBL is raised we call the handler function for write. If the TXC interrupt is
 * raised, we call the stop function.
 *
 * @note
 * N/A
 *
 ******************************************************************************/

void LEUART0_IRQHandler(void){
  uint32_t int_flag = LEUART0->IF & LEUART0->IEN;
  LEUART0->IFC = int_flag;

  if(int_flag & LEUART_IF_TXBL){
      write_data_func(&leuart0_state);
  }

  if(int_flag & LEUART_IF_TXC){
      stop_func(&leuart0_state);
  }

}


