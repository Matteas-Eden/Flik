/*******************************************************************************
* File Name: PWM_Timer_1_PM.c
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

#include "PWM_Timer_1.h"

static PWM_Timer_1_backupStruct PWM_Timer_1_backup;


/*******************************************************************************
* Function Name: PWM_Timer_1_SaveConfig
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
*  PWM_Timer_1_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void PWM_Timer_1_SaveConfig(void) 
{
    #if (!PWM_Timer_1_UsingFixedFunction)
        PWM_Timer_1_backup.TimerUdb = PWM_Timer_1_ReadCounter();
        PWM_Timer_1_backup.InterruptMaskValue = PWM_Timer_1_STATUS_MASK;
        #if (PWM_Timer_1_UsingHWCaptureCounter)
            PWM_Timer_1_backup.TimerCaptureCounter = PWM_Timer_1_ReadCaptureCount();
        #endif /* Back Up capture counter register  */

        #if(!PWM_Timer_1_UDB_CONTROL_REG_REMOVED)
            PWM_Timer_1_backup.TimerControlRegister = PWM_Timer_1_ReadControlRegister();
        #endif /* Backup the enable state of the Timer component */
    #endif /* Backup non retention registers in UDB implementation. All fixed function registers are retention */
}


/*******************************************************************************
* Function Name: PWM_Timer_1_RestoreConfig
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
*  PWM_Timer_1_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void PWM_Timer_1_RestoreConfig(void) 
{   
    #if (!PWM_Timer_1_UsingFixedFunction)

        PWM_Timer_1_WriteCounter(PWM_Timer_1_backup.TimerUdb);
        PWM_Timer_1_STATUS_MASK =PWM_Timer_1_backup.InterruptMaskValue;
        #if (PWM_Timer_1_UsingHWCaptureCounter)
            PWM_Timer_1_SetCaptureCount(PWM_Timer_1_backup.TimerCaptureCounter);
        #endif /* Restore Capture counter register*/

        #if(!PWM_Timer_1_UDB_CONTROL_REG_REMOVED)
            PWM_Timer_1_WriteControlRegister(PWM_Timer_1_backup.TimerControlRegister);
        #endif /* Restore the enable state of the Timer component */
    #endif /* Restore non retention registers in the UDB implementation only */
}


/*******************************************************************************
* Function Name: PWM_Timer_1_Sleep
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
*  PWM_Timer_1_backup.TimerEnableState:  Is modified depending on the
*  enable state of the block before entering sleep mode.
*
*******************************************************************************/
void PWM_Timer_1_Sleep(void) 
{
    #if(!PWM_Timer_1_UDB_CONTROL_REG_REMOVED)
        /* Save Counter's enable state */
        if(PWM_Timer_1_CTRL_ENABLE == (PWM_Timer_1_CONTROL & PWM_Timer_1_CTRL_ENABLE))
        {
            /* Timer is enabled */
            PWM_Timer_1_backup.TimerEnableState = 1u;
        }
        else
        {
            /* Timer is disabled */
            PWM_Timer_1_backup.TimerEnableState = 0u;
        }
    #endif /* Back up enable state from the Timer control register */
    PWM_Timer_1_Stop();
    PWM_Timer_1_SaveConfig();
}


/*******************************************************************************
* Function Name: PWM_Timer_1_Wakeup
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
*  PWM_Timer_1_backup.enableState:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void PWM_Timer_1_Wakeup(void) 
{
    PWM_Timer_1_RestoreConfig();
    #if(!PWM_Timer_1_UDB_CONTROL_REG_REMOVED)
        if(PWM_Timer_1_backup.TimerEnableState == 1u)
        {     /* Enable Timer's operation */
                PWM_Timer_1_Enable();
        } /* Do nothing if Timer was disabled before */
    #endif /* Remove this code section if Control register is removed */
}


/* [] END OF FILE */
