/*******************************************************************************
* File Name: INV2.h  
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

#if !defined(CY_PINS_INV2_H) /* Pins INV2_H */
#define CY_PINS_INV2_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "INV2_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 INV2__PORT == 15 && ((INV2__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    INV2_Write(uint8 value);
void    INV2_SetDriveMode(uint8 mode);
uint8   INV2_ReadDataReg(void);
uint8   INV2_Read(void);
void    INV2_SetInterruptMode(uint16 position, uint16 mode);
uint8   INV2_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the INV2_SetDriveMode() function.
     *  @{
     */
        #define INV2_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define INV2_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define INV2_DM_RES_UP          PIN_DM_RES_UP
        #define INV2_DM_RES_DWN         PIN_DM_RES_DWN
        #define INV2_DM_OD_LO           PIN_DM_OD_LO
        #define INV2_DM_OD_HI           PIN_DM_OD_HI
        #define INV2_DM_STRONG          PIN_DM_STRONG
        #define INV2_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define INV2_MASK               INV2__MASK
#define INV2_SHIFT              INV2__SHIFT
#define INV2_WIDTH              1u

/* Interrupt constants */
#if defined(INV2__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in INV2_SetInterruptMode() function.
     *  @{
     */
        #define INV2_INTR_NONE      (uint16)(0x0000u)
        #define INV2_INTR_RISING    (uint16)(0x0001u)
        #define INV2_INTR_FALLING   (uint16)(0x0002u)
        #define INV2_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define INV2_INTR_MASK      (0x01u) 
#endif /* (INV2__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define INV2_PS                     (* (reg8 *) INV2__PS)
/* Data Register */
#define INV2_DR                     (* (reg8 *) INV2__DR)
/* Port Number */
#define INV2_PRT_NUM                (* (reg8 *) INV2__PRT) 
/* Connect to Analog Globals */                                                  
#define INV2_AG                     (* (reg8 *) INV2__AG)                       
/* Analog MUX bux enable */
#define INV2_AMUX                   (* (reg8 *) INV2__AMUX) 
/* Bidirectional Enable */                                                        
#define INV2_BIE                    (* (reg8 *) INV2__BIE)
/* Bit-mask for Aliased Register Access */
#define INV2_BIT_MASK               (* (reg8 *) INV2__BIT_MASK)
/* Bypass Enable */
#define INV2_BYP                    (* (reg8 *) INV2__BYP)
/* Port wide control signals */                                                   
#define INV2_CTL                    (* (reg8 *) INV2__CTL)
/* Drive Modes */
#define INV2_DM0                    (* (reg8 *) INV2__DM0) 
#define INV2_DM1                    (* (reg8 *) INV2__DM1)
#define INV2_DM2                    (* (reg8 *) INV2__DM2) 
/* Input Buffer Disable Override */
#define INV2_INP_DIS                (* (reg8 *) INV2__INP_DIS)
/* LCD Common or Segment Drive */
#define INV2_LCD_COM_SEG            (* (reg8 *) INV2__LCD_COM_SEG)
/* Enable Segment LCD */
#define INV2_LCD_EN                 (* (reg8 *) INV2__LCD_EN)
/* Slew Rate Control */
#define INV2_SLW                    (* (reg8 *) INV2__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define INV2_PRTDSI__CAPS_SEL       (* (reg8 *) INV2__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define INV2_PRTDSI__DBL_SYNC_IN    (* (reg8 *) INV2__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define INV2_PRTDSI__OE_SEL0        (* (reg8 *) INV2__PRTDSI__OE_SEL0) 
#define INV2_PRTDSI__OE_SEL1        (* (reg8 *) INV2__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define INV2_PRTDSI__OUT_SEL0       (* (reg8 *) INV2__PRTDSI__OUT_SEL0) 
#define INV2_PRTDSI__OUT_SEL1       (* (reg8 *) INV2__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define INV2_PRTDSI__SYNC_OUT       (* (reg8 *) INV2__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(INV2__SIO_CFG)
    #define INV2_SIO_HYST_EN        (* (reg8 *) INV2__SIO_HYST_EN)
    #define INV2_SIO_REG_HIFREQ     (* (reg8 *) INV2__SIO_REG_HIFREQ)
    #define INV2_SIO_CFG            (* (reg8 *) INV2__SIO_CFG)
    #define INV2_SIO_DIFF           (* (reg8 *) INV2__SIO_DIFF)
#endif /* (INV2__SIO_CFG) */

/* Interrupt Registers */
#if defined(INV2__INTSTAT)
    #define INV2_INTSTAT            (* (reg8 *) INV2__INTSTAT)
    #define INV2_SNAP               (* (reg8 *) INV2__SNAP)
    
	#define INV2_0_INTTYPE_REG 		(* (reg8 *) INV2__0__INTTYPE)
#endif /* (INV2__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_INV2_H */


/* [] END OF FILE */
