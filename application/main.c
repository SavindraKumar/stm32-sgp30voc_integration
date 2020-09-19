//! @addtogroup Main
//! @brief main file
//! @{
//!
//****************************************************************************/
//! @file main.c
//! @brief Initialisation file
//! @author Savindra Kumar(savindran1989@gmail.com)
//! @bug No known bugs.
//!
//****************************************************************************/
//****************************************************************************/
//                           Includes
//****************************************************************************/
//standard header files
#include <stdint.h>
#include <stdlib.h>
//user defined header files
#include "stm32f4xx_hal.h"

//****************************************************************************/
//                           Defines and typedefs
//****************************************************************************/


//****************************************************************************/
//                           Private Functions
//****************************************************************************/
//
//! @brief Init MCU
//! @param[in]    None
//! @param[out]   None
//! @return       None
//
static void SystemInit(void);

//
//! @brief Set MCU clock to 100Mhz
//! @param[in]    None
//! @param[out]   None
//! @return       None
//
static void SetClock(void);


//****************************************************************************/
//                           external variables
//****************************************************************************/

//****************************************************************************/
//                           Private variables
//****************************************************************************/


//****************************************************************************/
//                    G L O B A L  F U N C T I O N S
//****************************************************************************/
int main()
{
    SystemInit();
    
    while (1)
    {
        //Do something;
    }

}//end main


/******************************************************************************
 *                           L O C A L  F U N C T I O N S
 *****************************************************************************/
static void SystemInit(void)
{  
    HAL_Init();
    SetClock();
    SystemCoreClockUpdate();
}//end SystemInit


static void SetClock(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct         = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct         = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

    //Configure the main internal regulator output voltage
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    // Initializes the CPU, AHB and APB busses clocks
    RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
    RCC_OscInitStruct.HSIState            = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.LSIState            = RCC_LSI_ON; 
    RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource       = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM            = 8;
    RCC_OscInitStruct.PLL.PLLN            = 100;
    RCC_OscInitStruct.PLL.PLLP            = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ            = 4;

    if ( HAL_OK != HAL_RCC_OscConfig(&RCC_OscInitStruct) )
    {
       //TODO:Write error handler here
    }

    // Initializes the CPU, AHB and APB busses clocks
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                 | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;

    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if ( HAL_OK != HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) )
    {
       //TODO:Write error handler here
    }

    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    PeriphClkInitStruct.RTCClockSelection    = RCC_RTCCLKSOURCE_LSI;

    if (HAL_OK != HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct))
    {
      //TODO:Write error handler here
    }
    
}//end SetClock


/******************************************************************************
 *                             End of file
 ******************************************************************************/
/** @}*/