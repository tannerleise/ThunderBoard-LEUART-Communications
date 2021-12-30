/**
 * @file
 *  scheduler.c
 * @author
 *  Tanner Leise
 * @date
 *  9/16/21
 * @brief
 *
 */
//***********************************************************************************
// Include files
//***********************************************************************************
#include "scheduler.h"

//***********************************************************************************
// defined files
//***********************************************************************************



//***********************************************************************************
// Private variables
//***********************************************************************************

static unsigned int event_scheduled;


//***********************************************************************************
// Global functions
//***********************************************************************************


/***************************************************************************//**
 * @brief
 * Initializes the scheduler
 *
 *
 * @details
 * Sets the static variable event_scheduled to 0
 *
 *
 *
 *
 * @note
 * called in the peripheral setup
 *
 ******************************************************************************/
void scheduler_open(void){
  event_scheduled = 0;

}

/***************************************************************************//**
 * @brief
 * Adds an event to the scheduler
 *
 *
 * @details
 * Disables interrupts then adds the event to the event scheduled.
 *
 *
 *
 *
 * @note
 * Called in the main to schedule event
 *
 *
 *
 * @param[in] event
 * This is the event we wish to schedule.
 *
 *
 ******************************************************************************/

void add_scheduled_event(uint32_t event){
    CORE_DECLARE_IRQ_STATE;
    CORE_ENTER_CRITICAL();
    event_scheduled |= event;
    CORE_EXIT_CRITICAL();
}

/***************************************************************************//**
 * @brief
 * Removes a scheduled event
 *
 *
 *
 * @details
 * Disables interrupts then removes the event to the event scheduled.
 *
 *
 *
 *
 * @note
 *  * Called in the main to remove scheduled event
 *
 *
 *
 * @param[in]
 * This is the event we wish to remove.
 *
 ******************************************************************************/
void remove_scheduled_event(uint32_t event){
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  event_scheduled &= ~event;
  CORE_EXIT_CRITICAL();
}

/***************************************************************************//**
 * @brief
 * gets the scheduled event
 *
 *
 * @details
 * Simply returns the event_scheduled static variable
 *
 *
 *
 *
 * @note
 * N/A
 *
 *
 ******************************************************************************/

uint32_t get_scheduled_events(void){
  return(event_scheduled);
}



