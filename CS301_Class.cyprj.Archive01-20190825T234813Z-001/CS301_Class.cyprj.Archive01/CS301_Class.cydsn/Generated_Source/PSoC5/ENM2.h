/*******************************************************************************
* File Name: ENM2.h  
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

#if !defined(CY_PINS_ENM2_H) /* Pins ENM2_H */
#define CY_PINS_ENM2_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "ENM2_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 ENM2__PORT == 15 && ((ENM2__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    ENM2_Write(uint8 value);
void    ENM2_SetDriveMode(uint8 mode);
uint8   ENM2_ReadDataReg(void);
uint8   ENM2_Read(void);
void    ENM2_SetInterruptMode(uint16 position, uint16 mode);
uint8   ENM2_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the ENM2_SetDriveMode() function.
     *  @{
     */
        #define ENM2_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define ENM2_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define ENM2_DM_RES_UP          PIN_DM_RES_UP
        #define ENM2_DM_RES_DWN         PIN_DM_RES_DWN
        #define ENM2_DM_OD_LO           PIN_DM_OD_LO
        #define ENM2_DM_OD_HI           PIN_DM_OD_HI
        #define ENM2_DM_STRONG          PIN_DM_STRONG
        #define ENM2_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define ENM2_MASK               ENM2__MASK
#define ENM2_SHIFT              ENM2__SHIFT
#define ENM2_WIDTH              1u

/* Interrupt constants */
#if defined(ENM2__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in ENM2_SetInterruptMode() function.
     *  @{
     */
        #define ENM2_INTR_NONE      (uint16)(0x0000u)
        #define ENM2_INTR_RISING    (uint16)(0x0001u)
        #define ENM2_INTR_FALLING   (uint16)(0x0002u)
        #define ENM2_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define ENM2_INTR_MASK      (0x01u) 
#endif /* (ENM2__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define ENM2_PS                     (* (reg8 *) ENM2__PS)
/* Data Register */
#define ENM2_DR                     (* (reg8 *) ENM2__DR)
/* Port Number */
#define ENM2_PRT_NUM                (* (reg8 *) ENM2__PRT) 
/* Connect to Analog Globals */                                                  
#define ENM2_AG                     (* (reg8 *) ENM2__AG)                       
/* Analog MUX bux enable */
#define ENM2_AMUX                   (* (reg8 *) ENM2__AMUX) 
/* Bidirectional Enable */                                                        
#define ENM2_BIE                    (* (reg8 *) ENM2__BIE)
/* Bit-mask for Aliased Register Access */
#define ENM2_BIT_MASK               (* (reg8 *) ENM2__BIT_MASK)
/* Bypass Enable */
#define ENM2_BYP                    (* (reg8 *) ENM2__BYP)
/* Port wide control signals */                                                   
#define ENM2_CTL                    (* (reg8 *) ENM2__CTL)
/* Drive Modes */
#define ENM2_DM0                    (* (reg8 *) ENM2__DM0) 
#define ENM2_DM1                    (* (reg8 *) ENM2__DM1)
#define ENM2_DM2                    (* (reg8 *) ENM2__DM2) 
/* Input Buffer Disable Override */
#define ENM2_INP_DIS                (* (reg8 *) ENM2__INP_DIS)
/* LCD Common or Segment Drive */
#define ENM2_LCD_COM_SEG            (* (reg8 *) ENM2__LCD_COM_SEG)
/* Enable Segment LCD */
#define ENM2_LCD_EN                 (* (reg8 *) ENM2__LCD_EN)
/* Slew Rate Control */
#define ENM2_SLW                    (* (reg8 *) ENM2__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define ENM2_PRTDSI__CAPS_SEL       (* (reg8 *) ENM2__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define ENM2_PRTDSI__DBL_SYNC_IN    (* (reg8 *) ENM2__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define ENM2_PRTDSI__OE_SEL0        (* (reg8 *) ENM2__PRTDSI__OE_SEL0) 
#define ENM2_PRTDSI__OE_SEL1        (* (reg8 *) ENM2__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define ENM2_PRTDSI__OUT_SEL0       (* (reg8 *) ENM2__PRTDSI__OUT_SEL0) 
#define ENM2_PRTDSI__OUT_SEL1       (* (reg8 *) ENM2__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define ENM2_PRTDSI__SYNC_OUT       (* (reg8 *) ENM2__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(ENM2__SIO_CFG)
    #define ENM2_SIO_HYST_EN        (* (reg8 *) ENM2__SIO_HYST_EN)
    #define ENM2_SIO_REG_HIFREQ     (* (reg8 *) ENM2__SIO_REG_HIFREQ)
    #define ENM2_SIO_CFG            (* (reg8 *) ENM2__SIO_CFG)
    #define ENM2_SIO_DIFF           (* (reg8 *) ENM2__SIO_DIFF)
#endif /* (ENM2__SIO_CFG) */

/* Interrupt Registers */
#if defined(ENM2__INTSTAT)
    #define ENM2_INTSTAT            (* (reg8 *) ENM2__INTSTAT)
    #define ENM2_SNAP               (* (reg8 *) ENM2__SNAP)
    
	#define ENM2_0_INTTYPE_REG 		(* (reg8 *) ENM2__0__INTTYPE)
#endif /* (ENM2__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_ENM2_H */


/* [] END OF FILE */
