/**************************************************************************
 * @file sleep.c
 ***************************************************************************
 * @section License* <b>(C) Copyright 2015 Silicon Labs, http://www.silabs.com</b>
 * ***************************************************************************
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software.
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Silicon Labs has no
 * obligation to support this Software. Silicon Labs is providing the
 * Software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitnessfor any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 * Silicon Labs will not be liable for any consequential, incidental, or
 * special damages, or any other relief, or for any claim by any third party,
 * arising from your use of this Software.
 ****************************************************************************/
/*
 * sleep_routines.c
 *
 *  Created on: Sep 21, 2021
 *      Author: tanle
 */
//***********************************************************************************
// Include files
//***********************************************************************************
#include "sleep_routines.h"

//***********************************************************************************
// defined files
//***********************************************************************************



//***********************************************************************************
// Private variables
//***********************************************************************************
static int lowest_energy_mode[MAX_ENERGY_MODES];

//***********************************************************************************
// Global functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 * initializes the array lowest_energy_modes
 *
 *
 *
 * @details
 * iterates through the array and sets each element to 0
 *
 *
 *
 *
 *
 * @note
 * Used in app.c
 ******************************************************************************/
void sleep_open(void){
  for(int i = 0; i < MAX_ENERGY_MODES; i++){    //If weird iteration, do MAX_ENERGY_MODES - 1
      lowest_energy_mode[i] = 0;

  }
}

/***************************************************************************//**
 * @brief
 * blocks us from entering a specific energy mode
 *
 *
 *
 * @details
 *: Utilized by a peripheral to prevent the
 *: board from going into that sleep mode while the peripheral is active.
 *: It will increment the associated array element in lowest_energy_mode[] by one.
 *
 *
 * @note
 * N/A
 *
 *
 *
 * @param[in] EM
 * This is the energy mode that we wish to block
 *
 *
 ******************************************************************************/

void sleep_block_mode(uint32_t EM){
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  lowest_energy_mode[EM]++;
  EFM_ASSERT(lowest_energy_mode[EM] < 5);
  CORE_EXIT_CRITICAL();
}

/***************************************************************************//**
 * @brief
 * unblocks a specific energy mode
 *
 *
 *
 * @details
 * Utilized to release the processor from going into a sleep mode with
 * a peripheral that is no longer active. It will decrement the associated
 * array element in lowest_energy_mode[] by one.
 *
 *
 *
 *
 *
 * @note
 *
 * @param[in] EM
 * This is the energy mode that we wish to unblock
 *
 *
 *
 *
 ******************************************************************************/
void sleep_unblock_mode(uint32_t EM){
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  lowest_energy_mode[EM]--;
  EFM_ASSERT (lowest_energy_mode[EM] >= 0);
  CORE_EXIT_CRITICAL();
}
/***************************************************************************//**
 * @brief
 * Picks which sleep mode to enter
 *
 * @details
 * Function that will enter the appropriate sleep Energy Mode based on the
 * first non-zero array element in lowest_energy_mode[].
 *
 *
 *
 *
 *
 * @note
 * N/A
 *
 *
 *
 ******************************************************************************/
void enter_sleep(void){
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();

  if(lowest_energy_mode[EM0] > 0){
      CORE_EXIT_CRITICAL();
      return;
  }
  else if(lowest_energy_mode[EM1] > 0){
      CORE_EXIT_CRITICAL();
      return;
  }
  else if(lowest_energy_mode[EM2] > 0){
      CORE_EXIT_CRITICAL();
      EMU_EnterEM1();
      return;
  }
  else if(lowest_energy_mode[EM3] > 0){
      EMU_EnterEM2(true);
      CORE_EXIT_CRITICAL();
      return;
  }
  else{
      EMU_EnterEM3(true);
      CORE_EXIT_CRITICAL();
      return;
  }
}
/***************************************************************************//**
 * @brief
 * Returns which sleep mode is blocked
 *
 *
 *
 * @details
 * Function that returns which energy mode that the current system cannot enter,
 * the first non-zero array element in lowest_energy_mode[].
 *
 *
 *
 *
 *
 * @note
 ******************************************************************************/
uint32_t current_block_energy_mode(void){
  int i = 0;
  while(i < MAX_ENERGY_MODES){
    if(lowest_energy_mode[i] != 0){
       return i;
       }
    i++;
  }
  return MAX_ENERGY_MODES-1;
}

