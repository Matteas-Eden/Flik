/*******************************************************************************
* File Name: ENM1.h  
* Version 2.20
*
* Description:
*  This file contains Pin function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_ENM1_H) /* Pins ENM1_H */
#define CY_PINS_ENM1_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "ENM1_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 ENM1__PORT == 15 && ((ENM1__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    ENM1_Write(uint8 value);
void    ENM1_SetDriveMode(uint8 mode);
uint8   ENM1_ReadDataReg(void);
uint8   ENM1_Read(void);
void    ENM1_SetInterruptMode(uint16 position, uint16 mode);
uint8   ENM1_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the ENM1_SetDriveMode() function.
     *  @{
     */
        #define ENM1_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define ENM1_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define ENM1_DM_RES_UP          PIN_DM_RES_UP
        #define ENM1_DM_RES_DWN         PIN_DM_RES_DWN
        #define ENM1_DM_OD_LO           PIN_DM_OD_LO
        #define ENM1_DM_OD_HI           PIN_DM_OD_HI
        #define ENM1_DM_STRONG          PIN_DM_STRONG
        #define ENM1_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define ENM1_MASK               ENM1__MASK
#define ENM1_SHIFT              ENM1__SHIFT
#define ENM1_WIDTH              1u

/* Interrupt constants */
#if defined(ENM1__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in ENM1_SetInterruptMode() function.
     *  @{
     */
        #define ENM1_INTR_NONE      (uint16)(0x0000u)
        #define ENM1_INTR_RISING    (uint16)(0x0001u)
        #define ENM1_INTR_FALLING   (uint16)(0x0002u)
        #define ENM1_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define ENM1_INTR_MASK      (0x01u) 
#endif /* (ENM1__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define ENM1_PS                     (* (reg8 *) ENM1__PS)
/* Data Register */
#define ENM1_DR                     (* (reg8 *) ENM1__DR)
/* Port Number */
#define ENM1_PRT_NUM                (* (reg8 *) ENM1__PRT) 
/* Connect to Analog Globals */                                                  
#define ENM1_AG                     (* (reg8 *) ENM1__AG)                       
/* Analog MUX bux enable */
#define ENM1_AMUX                   (* (reg8 *) ENM1__AMUX) 
/* Bidirectional Enable */                                                        
#define ENM1_BIE                    (* (reg8 *) ENM1__BIE)
/* Bit-mask for Aliased Register Access */
#define ENM1_BIT_MASK               (* (reg8 *) ENM1__BIT_MASK)
/* Bypass Enable */
#define ENM1_BYP                    (* (reg8 *) ENM1__BYP)
/* Port wide control signals */                                                   
#define ENM1_CTL                    (* (reg8 *) ENM1__CTL)
/* Drive Modes */
#define ENM1_DM0                    (* (reg8 *) ENM1__DM0) 
#define ENM1_DM1                    (* (reg8 *) ENM1__DM1)
#define ENM1_DM2                    (* (reg8 *) ENM1__DM2) 
/* Input Buffer Disable Override */
#define ENM1_INP_DIS                (* (reg8 *) ENM1__INP_DIS)
/* LCD Common or Segment Drive */
#define ENM1_LCD_COM_SEG            (* (reg8 *) ENM1__LCD_COM_SEG)
/* Enable Segment LCD */
#define ENM1_LCD_EN                 (* (reg8 *) ENM1__LCD_EN)
/* Slew Rate Control */
#define ENM1_SLW                    (* (reg8 *) ENM1__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define ENM1_PRTDSI__CAPS_SEL       (* (reg8 *) ENM1__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define ENM1_PRTDSI__DBL_SYNC_IN    (* (reg8 *) ENM1__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define ENM1_PRTDSI__OE_SEL0        (* (reg8 *) ENM1__PRTDSI__OE_SEL0) 
#define ENM1_PRTDSI__OE_SEL1        (* (reg8 *) ENM1__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define ENM1_PRTDSI__OUT_SEL0       (* (reg8 *) ENM1__PRTDSI__OUT_SEL0) 
#define ENM1_PRTDSI__OUT_SEL1       (* (reg8 *) ENM1__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define ENM1_PRTDSI__SYNC_OUT       (* (reg8 *) ENM1__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(ENM1__SIO_CFG)
    #define ENM1_SIO_HYST_EN        (* (reg8 *) ENM1__SIO_HYST_EN)
    #define ENM1_SIO_REG_HIFREQ     (* (reg8 *) ENM1__SIO_REG_HIFREQ)
    #define ENM1_SIO_CFG            (* (reg8 *) ENM1__SIO_CFG)
    #define ENM1_SIO_DIFF           (* (reg8 *) ENM1__SIO_DIFF)
#endif /* (ENM1__SIO_CFG) */

/* Interrupt Registers */
#if defined(ENM1__INTSTAT)
    #define ENM1_INTSTAT            (* (reg8 *) ENM1__INTSTAT)
    #define ENM1_SNAP               (* (reg8 *) ENM1__SNAP)
    
	#define ENM1_0_INTTYPE_REG 		(* (reg8 *) ENM1__0__INTTYPE)
#endif /* (ENM1__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_ENM1_H */


/* [] END OF FILE */
