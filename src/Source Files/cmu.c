/**
 * @file
 * cmu.c
 * @author
 * Tanner Leise
 * @date
 * 9/8/21
 * @brief
 * responsible in enabling all oscillators and routing the clock tree for the application.
 *
 */
//***********************************************************************************
// Include files
//***********************************************************************************
#include "cmu.h"

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// Private variables
//***********************************************************************************


//***********************************************************************************
// Private functions
//***********************************************************************************


//***********************************************************************************
// Global functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 * responsible in enabling all oscillators and routing the clock tree for the application.
 *
 *
 * @details
 * This function is responsible for enabling all clocks and oscillators for the
 * CMU. Specifically for this function it routes the LF clock to the LF clock tree
 * and enables the clock tree onto the LE clock branch.
 *
 *
 * @note
 * This should be used in the application file. Used to set up the CMU.
 *
 *
 ******************************************************************************/

void cmu_open(void){

    CMU_ClockEnable(cmuClock_HFPER, true);

    // By default, LFRCO is enabled, disable the LFRCO oscillator
    // Disable the LFRCO oscillator
    // What is the enumeration required for LFRCO?
    // It can be found in the online HAL documentation
    CMU_OscillatorEnable(cmuOsc_LFRCO, false, false);       //Disables the Low Frequency Oscillator

    // Disable the LFXO oscillator
    // What is the enumeration required for LFXO?
    // It can be found in the online HAL documentation
    CMU_OscillatorEnable(cmuOsc_LFXO, true, true);       //Enable the Low Frequency crystal oscillator

    // No requirement to enable the ULFRCO oscillator.  It is always enabled in EM0-4H1

    // Route LF clock to the LF clock tree
    // What is the enumeration required to placed the ULFRCO onto the proper clock branch?
    // It can be found in the online HAL documentation
    CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_ULFRCO);    // routing ULFRCO to proper Low Freq clock tree

    CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFXO);    // routing ULFRCO to proper Low Freq clock tree

    // What is the proper enumeration to enable the clock tree onto the LE clock branches?
    // It can be found in the Assignment 2 documentation
    CMU_ClockEnable(cmuClock_CORELE, true);

}
