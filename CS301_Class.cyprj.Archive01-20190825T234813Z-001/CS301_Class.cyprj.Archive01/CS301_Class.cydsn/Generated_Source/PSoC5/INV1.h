/*******************************************************************************
* File Name: INV1.h  
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

#if !defined(CY_PINS_INV1_H) /* Pins INV1_H */
#define CY_PINS_INV1_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "INV1_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 INV1__PORT == 15 && ((INV1__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    INV1_Write(uint8 value);
void    INV1_SetDriveMode(uint8 mode);
uint8   INV1_ReadDataReg(void);
uint8   INV1_Read(void);
void    INV1_SetInterruptMode(uint16 position, uint16 mode);
uint8   INV1_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the INV1_SetDriveMode() function.
     *  @{
     */
        #define INV1_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define INV1_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define INV1_DM_RES_UP          PIN_DM_RES_UP
        #define INV1_DM_RES_DWN         PIN_DM_RES_DWN
        #define INV1_DM_OD_LO           PIN_DM_OD_LO
        #define INV1_DM_OD_HI           PIN_DM_OD_HI
        #define INV1_DM_STRONG          PIN_DM_STRONG
        #define INV1_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define INV1_MASK               INV1__MASK
#define INV1_SHIFT              INV1__SHIFT
#define INV1_WIDTH              1u

/* Interrupt constants */
#if defined(INV1__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in INV1_SetInterruptMode() function.
     *  @{
     */
        #define INV1_INTR_NONE      (uint16)(0x0000u)
        #define INV1_INTR_RISING    (uint16)(0x0001u)
        #define INV1_INTR_FALLING   (uint16)(0x0002u)
        #define INV1_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define INV1_INTR_MASK      (0x01u) 
#endif /* (INV1__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define INV1_PS                     (* (reg8 *) INV1__PS)
/* Data Register */
#define INV1_DR                     (* (reg8 *) INV1__DR)
/* Port Number */
#define INV1_PRT_NUM                (* (reg8 *) INV1__PRT) 
/* Connect to Analog Globals */                                                  
#define INV1_AG                     (* (reg8 *) INV1__AG)                       
/* Analog MUX bux enable */
#define INV1_AMUX                   (* (reg8 *) INV1__AMUX) 
/* Bidirectional Enable */                                                        
#define INV1_BIE                    (* (reg8 *) INV1__BIE)
/* Bit-mask for Aliased Register Access */
#define INV1_BIT_MASK               (* (reg8 *) INV1__BIT_MASK)
/* Bypass Enable */
#define INV1_BYP                    (* (reg8 *) INV1__BYP)
/* Port wide control signals */                                                   
#define INV1_CTL                    (* (reg8 *) INV1__CTL)
/* Drive Modes */
#define INV1_DM0                    (* (reg8 *) INV1__DM0) 
#define INV1_DM1                    (* (reg8 *) INV1__DM1)
#define INV1_DM2                    (* (reg8 *) INV1__DM2) 
/* Input Buffer Disable Override */
#define INV1_INP_DIS                (* (reg8 *) INV1__INP_DIS)
/* LCD Common or Segment Drive */
#define INV1_LCD_COM_SEG            (* (reg8 *) INV1__LCD_COM_SEG)
/* Enable Segment LCD */
#define INV1_LCD_EN                 (* (reg8 *) INV1__LCD_EN)
/* Slew Rate Control */
#define INV1_SLW                    (* (reg8 *) INV1__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define INV1_PRTDSI__CAPS_SEL       (* (reg8 *) INV1__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define INV1_PRTDSI__DBL_SYNC_IN    (* (reg8 *) INV1__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define INV1_PRTDSI__OE_SEL0        (* (reg8 *) INV1__PRTDSI__OE_SEL0) 
#define INV1_PRTDSI__OE_SEL1        (* (reg8 *) INV1__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define INV1_PRTDSI__OUT_SEL0       (* (reg8 *) INV1__PRTDSI__OUT_SEL0) 
#define INV1_PRTDSI__OUT_SEL1       (* (reg8 *) INV1__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define INV1_PRTDSI__SYNC_OUT       (* (reg8 *) INV1__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(INV1__SIO_CFG)
    #define INV1_SIO_HYST_EN        (* (reg8 *) INV1__SIO_HYST_EN)
    #define INV1_SIO_REG_HIFREQ     (* (reg8 *) INV1__SIO_REG_HIFREQ)
    #define INV1_SIO_CFG            (* (reg8 *) INV1__SIO_CFG)
    #define INV1_SIO_DIFF           (* (reg8 *) INV1__SIO_DIFF)
#endif /* (INV1__SIO_CFG) */

/* Interrupt Registers */
#if defined(INV1__INTSTAT)
    #define INV1_INTSTAT            (* (reg8 *) INV1__INTSTAT)
    #define INV1_SNAP               (* (reg8 *) INV1__SNAP)
    
	#define INV1_0_INTTYPE_REG 		(* (reg8 *) INV1__0__INTTYPE)
#endif /* (INV1__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_INV1_H */


/* [] END OF FILE */
