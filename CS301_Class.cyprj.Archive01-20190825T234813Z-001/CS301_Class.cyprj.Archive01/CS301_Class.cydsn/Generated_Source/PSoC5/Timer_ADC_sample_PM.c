/*******************************************************************************
* File Name: Timer_ADC_sample_PM.c
* Version 2.70
*
*  Description:
*     This file provides the power management source code to API for the
*     Timer.
*
*   Note:
*     None
*
*******************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

#include "Timer_ADC_sample.h"

static Timer_ADC_sample_backupStruct Timer_ADC_sample_backup;


/*******************************************************************************
* Function Name: Timer_ADC_sample_SaveConfig
********************************************************************************
*
* Summary:
*     Save the current user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Timer_ADC_sample_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void Timer_ADC_sample_SaveConfig(void) 
{
    #if (!Timer_ADC_sample_UsingFixedFunction)
        Timer_ADC_sample_backup.TimerUdb = Timer_ADC_sample_ReadCounter();
        Timer_ADC_sample_backup.InterruptMaskValue = Timer_ADC_sample_STATUS_MASK;
        #if (Timer_ADC_sample_UsingHWCaptureCounter)
            Timer_ADC_sample_backup.TimerCaptureCounter = Timer_ADC_sample_ReadCaptureCount();
        #endif /* Back Up capture counter register  */

        #if(!Timer_ADC_sample_UDB_CONTROL_REG_REMOVED)
            Timer_ADC_sample_backup.TimerControlRegister = Timer_ADC_sample_ReadControlRegister();
        #endif /* Backup the enable state of the Timer component */
    #endif /* Backup non retention registers in UDB implementation. All fixed function registers are retention */
}


/*******************************************************************************
* Function Name: Timer_ADC_sample_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration.
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Timer_ADC_sample_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void Timer_ADC_sample_RestoreConfig(void) 
{   
    #if (!Timer_ADC_sample_UsingFixedFunction)

        Timer_ADC_sample_WriteCounter(Timer_ADC_sample_backup.TimerUdb);
        Timer_ADC_sample_STATUS_MASK =Timer_ADC_sample_backup.InterruptMaskValue;
        #if (Timer_ADC_sample_UsingHWCaptureCounter)
            Timer_ADC_sample_SetCaptureCount(Timer_ADC_sample_backup.TimerCaptureCounter);
        #endif /* Restore Capture counter register*/

        #if(!Timer_ADC_sample_UDB_CONTROL_REG_REMOVED)
            Timer_ADC_sample_WriteControlRegister(Timer_ADC_sample_backup.TimerControlRegister);
        #endif /* Restore the enable state of the Timer component */
    #endif /* Restore non retention registers in the UDB implementation only */
}


/*******************************************************************************
* Function Name: Timer_ADC_sample_Sleep
********************************************************************************
*
* Summary:
*     Stop and Save the user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Timer_ADC_sample_backup.TimerEnableState:  Is modified depending on the
*  enable state of the block before entering sleep mode.
*
*******************************************************************************/
void Timer_ADC_sample_Sleep(void) 
{
    #if(!Timer_ADC_sample_UDB_CONTROL_REG_REMOVED)
        /* Save Counter's enable state */
        if(Timer_ADC_sample_CTRL_ENABLE == (Timer_ADC_sample_CONTROL & Timer_ADC_sample_CTRL_ENABLE))
        {
            /* Timer is enabled */
            Timer_ADC_sample_backup.TimerEnableState = 1u;
        }
        else
        {
            /* Timer is disabled */
            Timer_ADC_sample_backup.TimerEnableState = 0u;
        }
    #endif /* Back up enable state from the Timer control register */
    Timer_ADC_sample_Stop();
    Timer_ADC_sample_SaveConfig();
}


/*******************************************************************************
* Function Name: Timer_ADC_sample_Wakeup
********************************************************************************
*
* Summary:
*  Restores and enables the user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Timer_ADC_sample_backup.enableState:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void Timer_ADC_sample_Wakeup(void) 
{
    Timer_ADC_sample_RestoreConfig();
    #if(!Timer_ADC_sample_UDB_CONTROL_REG_REMOVED)
        if(Timer_ADC_sample_backup.TimerEnableState == 1u)
        {     /* Enable Timer's operation */
                Timer_ADC_sample_Enable();
        } /* Do nothing if Timer was disabled before */
    #endif /* Remove this code section if Control register is removed */
}


/* [] END OF FILE */
