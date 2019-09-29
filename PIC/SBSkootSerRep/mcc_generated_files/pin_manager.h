/**
  @Generated Pin Manager Header File

  @Company:
    Microchip Technology Inc.

  @File Name:
    pin_manager.h

  @Summary:
    This is the Pin Manager file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  @Description
    This header file provides APIs for driver for .
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.77
        Device            :  PIC16F18326
        Driver Version    :  2.11
    The generated drivers are tested against the following:
        Compiler          :  XC8 2.05 and above
        MPLAB 	          :  MPLAB X 5.20	
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

#ifndef PIN_MANAGER_H
#define PIN_MANAGER_H

/**
  Section: Included Files
*/

#include <xc.h>

#define INPUT   1
#define OUTPUT  0

#define HIGH    1
#define LOW     0

#define ANALOG      1
#define DIGITAL     0

#define PULL_UP_ENABLED      1
#define PULL_UP_DISABLED     0

// get/set IO_SDATA aliases
#define IO_SDATA_TRIS                 TRISCbits.TRISC1
#define IO_SDATA_LAT                  LATCbits.LATC1
#define IO_SDATA_PORT                 PORTCbits.RC1
#define IO_SDATA_WPU                  WPUCbits.WPUC1
#define IO_SDATA_OD                   ODCONCbits.ODCC1
#define IO_SDATA_ANS                  ANSELCbits.ANSC1
#define IO_SDATA_SetHigh()            do { LATCbits.LATC1 = 1; } while(0)
#define IO_SDATA_SetLow()             do { LATCbits.LATC1 = 0; } while(0)
#define IO_SDATA_Toggle()             do { LATCbits.LATC1 = ~LATCbits.LATC1; } while(0)
#define IO_SDATA_GetValue()           PORTCbits.RC1
#define IO_SDATA_SetDigitalInput()    do { TRISCbits.TRISC1 = 1; } while(0)
#define IO_SDATA_SetDigitalOutput()   do { TRISCbits.TRISC1 = 0; } while(0)
#define IO_SDATA_SetPullup()          do { WPUCbits.WPUC1 = 1; } while(0)
#define IO_SDATA_ResetPullup()        do { WPUCbits.WPUC1 = 0; } while(0)
#define IO_SDATA_SetPushPull()        do { ODCONCbits.ODCC1 = 0; } while(0)
#define IO_SDATA_SetOpenDrain()       do { ODCONCbits.ODCC1 = 1; } while(0)
#define IO_SDATA_SetAnalogMode()      do { ANSELCbits.ANSC1 = 1; } while(0)
#define IO_SDATA_SetDigitalMode()     do { ANSELCbits.ANSC1 = 0; } while(0)

// get/set IO_LED aliases
#define IO_LED_TRIS                 TRISCbits.TRISC2
#define IO_LED_LAT                  LATCbits.LATC2
#define IO_LED_PORT                 PORTCbits.RC2
#define IO_LED_WPU                  WPUCbits.WPUC2
#define IO_LED_OD                   ODCONCbits.ODCC2
#define IO_LED_ANS                  ANSELCbits.ANSC2
#define IO_LED_SetHigh()            do { LATCbits.LATC2 = 1; } while(0)
#define IO_LED_SetLow()             do { LATCbits.LATC2 = 0; } while(0)
#define IO_LED_Toggle()             do { LATCbits.LATC2 = ~LATCbits.LATC2; } while(0)
#define IO_LED_GetValue()           PORTCbits.RC2
#define IO_LED_SetDigitalInput()    do { TRISCbits.TRISC2 = 1; } while(0)
#define IO_LED_SetDigitalOutput()   do { TRISCbits.TRISC2 = 0; } while(0)
#define IO_LED_SetPullup()          do { WPUCbits.WPUC2 = 1; } while(0)
#define IO_LED_ResetPullup()        do { WPUCbits.WPUC2 = 0; } while(0)
#define IO_LED_SetPushPull()        do { ODCONCbits.ODCC2 = 0; } while(0)
#define IO_LED_SetOpenDrain()       do { ODCONCbits.ODCC2 = 1; } while(0)
#define IO_LED_SetAnalogMode()      do { ANSELCbits.ANSC2 = 1; } while(0)
#define IO_LED_SetDigitalMode()     do { ANSELCbits.ANSC2 = 0; } while(0)

// get/set IO_DRDY aliases
#define IO_DRDY_TRIS                 TRISCbits.TRISC3
#define IO_DRDY_LAT                  LATCbits.LATC3
#define IO_DRDY_PORT                 PORTCbits.RC3
#define IO_DRDY_WPU                  WPUCbits.WPUC3
#define IO_DRDY_OD                   ODCONCbits.ODCC3
#define IO_DRDY_ANS                  ANSELCbits.ANSC3
#define IO_DRDY_SetHigh()            do { LATCbits.LATC3 = 1; } while(0)
#define IO_DRDY_SetLow()             do { LATCbits.LATC3 = 0; } while(0)
#define IO_DRDY_Toggle()             do { LATCbits.LATC3 = ~LATCbits.LATC3; } while(0)
#define IO_DRDY_GetValue()           PORTCbits.RC3
#define IO_DRDY_SetDigitalInput()    do { TRISCbits.TRISC3 = 1; } while(0)
#define IO_DRDY_SetDigitalOutput()   do { TRISCbits.TRISC3 = 0; } while(0)
#define IO_DRDY_SetPullup()          do { WPUCbits.WPUC3 = 1; } while(0)
#define IO_DRDY_ResetPullup()        do { WPUCbits.WPUC3 = 0; } while(0)
#define IO_DRDY_SetPushPull()        do { ODCONCbits.ODCC3 = 0; } while(0)
#define IO_DRDY_SetOpenDrain()       do { ODCONCbits.ODCC3 = 1; } while(0)
#define IO_DRDY_SetAnalogMode()      do { ANSELCbits.ANSC3 = 1; } while(0)
#define IO_DRDY_SetDigitalMode()     do { ANSELCbits.ANSC3 = 0; } while(0)

// get/set RC4 procedures
#define RC4_SetHigh()            do { LATCbits.LATC4 = 1; } while(0)
#define RC4_SetLow()             do { LATCbits.LATC4 = 0; } while(0)
#define RC4_Toggle()             do { LATCbits.LATC4 = ~LATCbits.LATC4; } while(0)
#define RC4_GetValue()              PORTCbits.RC4
#define RC4_SetDigitalInput()    do { TRISCbits.TRISC4 = 1; } while(0)
#define RC4_SetDigitalOutput()   do { TRISCbits.TRISC4 = 0; } while(0)
#define RC4_SetPullup()             do { WPUCbits.WPUC4 = 1; } while(0)
#define RC4_ResetPullup()           do { WPUCbits.WPUC4 = 0; } while(0)
#define RC4_SetAnalogMode()         do { ANSELCbits.ANSC4 = 1; } while(0)
#define RC4_SetDigitalMode()        do { ANSELCbits.ANSC4 = 0; } while(0)

// get/set RC5 procedures
#define RC5_SetHigh()            do { LATCbits.LATC5 = 1; } while(0)
#define RC5_SetLow()             do { LATCbits.LATC5 = 0; } while(0)
#define RC5_Toggle()             do { LATCbits.LATC5 = ~LATCbits.LATC5; } while(0)
#define RC5_GetValue()              PORTCbits.RC5
#define RC5_SetDigitalInput()    do { TRISCbits.TRISC5 = 1; } while(0)
#define RC5_SetDigitalOutput()   do { TRISCbits.TRISC5 = 0; } while(0)
#define RC5_SetPullup()             do { WPUCbits.WPUC5 = 1; } while(0)
#define RC5_ResetPullup()           do { WPUCbits.WPUC5 = 0; } while(0)
#define RC5_SetAnalogMode()         do { ANSELCbits.ANSC5 = 1; } while(0)
#define RC5_SetDigitalMode()        do { ANSELCbits.ANSC5 = 0; } while(0)

/**
   @Param
    none
   @Returns
    none
   @Description
    GPIO and peripheral I/O initialization
   @Example
    PIN_MANAGER_Initialize();
 */
void PIN_MANAGER_Initialize (void);

/**
 * @Param
    none
 * @Returns
    none
 * @Description
    Interrupt on Change Handling routine
 * @Example
    PIN_MANAGER_IOC();
 */
void PIN_MANAGER_IOC(void);



#endif // PIN_MANAGER_H
/**
 End of File
*/