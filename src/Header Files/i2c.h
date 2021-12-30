/*
 * i2c.h
 *
 *  Created on: Sep 30, 2021
 *      Author: tanle
 */
//***********************************************************************************
// Include files
//***********************************************************************************
#ifndef I2C_HG
#define I2C_HG

/* System include statements */


/* Silicon Labs include statements */
#include "em_i2c.h"
#include <stdbool.h>
#include "em_cmu.h"

/* The developer's include statements */
#include "sleep_routines.h"
#include "scheduler.h"

//***********************************************************************************
// defined files
//***********************************************************************************

#define I2C_EM_BLOCK EM2 // 2 = first mode it cannot enter


//***********************************************************************************
// global variables
//***********************************************************************************
typedef struct {
  bool                  enable;
  bool                  master;
  uint32_t              refFreq;
  uint32_t              freq;
  I2C_ClockHLR_TypeDef  clhr;
  uint32_t              scl_out_route0;   //Check these 4 things
  uint32_t              sda_out_route0;   //Do we need to do more to route this?
  bool                  out_sda_en;       // enable out 0 route
  bool                  out_scl_en;       // enable out 1 route

  bool                  ack_irq_enable;
  bool                  rxdatav_irq_enable;
  bool                  stop_irq_enable;


} I2C_OPEN_STRUCT ;


typedef enum{
  initialize_write,
  communicate_register,
  initialize_read,
  write_data,
  receive_data,
  verify_stop
}DEFINED_STATES;


typedef struct {
  uint32_t        *data;                    //This holds the data that was sent to us
  uint32_t        bytes_expected;           //This will be used in iterating, tells us how many bytes we expect
  volatile bool   i2c_available;                //Tells whether the pearl gecko is available or not
  DEFINED_STATES  current_state;            //gives the current state of the state machine
  uint32_t        device_address;           //This is the address of the device we are communicating with
  I2C_TypeDef     *i2cx;                     //Tells us which i2c to use
  uint32_t        register_address;        //This is the address we wish to talk to
  bool            mode;                    //0 for write 1 for read
  uint32_t        i2c_call_back;              //Tells us which interrupts were triggered
} I2C_STATE_MACHINE;


//***********************************************************************************
// function prototypes
//***********************************************************************************
void i2c_open(I2C_TypeDef *i2c, I2C_OPEN_STRUCT *i2c_init);
void i2c_start(bool mode, uint32_t *data, uint32_t bytes_expected, uint32_t device_address, uint32_t register_address, I2C_TypeDef *i2cx,uint32_t call_back);

void I2C0_IRQHandler();
void I2C1_IRQHandler();
bool is_available(I2C_TypeDef *i2c);

#endif /* HEADER_FILES_I2C_H_ */
