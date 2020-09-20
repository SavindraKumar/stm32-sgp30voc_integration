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
#include <stdio.h>
//user defined header files
#include "stm32f4xx_hal.h"
#include "init.h"
#include "uart_app.h"
#include "sgp30.h"
#include "sgp_git_version.h"

const char *SGP_DRV_VERSION_STR = "7.0.0";

//****************************************************************************/
//                           Defines and typedefs
//****************************************************************************/

//****************************************************************************/
//                           Private Functions
//****************************************************************************/

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
    Init();
    UARTInit();

    const char* driver_version = sgp30_get_driver_version();
    
    if (driver_version) 
    {
        printf("SGP30 driver version %s\n", driver_version);
    } 
    else 
    {
        printf("fatal: Getting driver version failed\n");
        return -1;
    }

    // Initialize I2C bus
    sensirion_i2c_init();

    
    while (1) 
    {
        int16_t probe;
        probe = sgp30_probe();

        if (STATUS_OK == probe)
        {
            break;
        }

        if (SGP30_ERR_UNSUPPORTED_FEATURE_SET == probe)
        {
            printf("Sensor need at least feature set version 1.0 (0x20)\n");
        }
                
        printf("SGP sensor probing failed\n");
        
        sensirion_sleep_usec(1000000);
    }

    printf("SGP sensor probing successful\n");

    uint16_t feature_set_version;
    uint8_t product_type;
    
    int16_t err = sgp30_get_feature_set_version(&feature_set_version, &product_type);
    
    if (STATUS_OK == err) 
    {
        printf("Feature set version: %u\n", feature_set_version);
        printf("Product type: %u\n", product_type);
    } 
    else 
    {
        printf("sgp30_get_feature_set_version failed!\n");
    }
    
    uint64_t serial_id;
    
    err = sgp30_get_serial_id(&serial_id);
    
    if (STATUS_OK == err)
    {
        printf("SerialID: %u\n", serial_id);
    } 
    else 
    {
        printf("sgp30_get_serial_id failed!\n");
    }


    // Consider the two cases (A) and (B):
    //(A) If no baseline is available or the most recent baseline is more than
    //one week old, it must discarded. A new baseline is found with
    //sgp30_iaq_init()
    
    err = sgp30_iaq_init();
    
    if (STATUS_OK == err) 
    {
        printf("sgp30_iaq_init done\n");
    } 
    else 
    {
        printf("sgp30_iaq_init failed!\n");
    }
    
    //(B) If a recent baseline is available, set it after sgp30_iaq_init() for
    //faster start-up */
    //IMPLEMENT: retrieve iaq_baseline from presistent storage;
    //err = sgp30_set_iaq_baseline(iaq_baseline);

    while (1) 
    {
        uint16_t tvoc_ppb     = 0;
        uint16_t co2_eq_ppm   = 0;
        uint32_t iaq_baseline = 0;  
        uint16_t count        = 0;
    
        err = sgp30_measure_iaq_blocking_read(&tvoc_ppb, &co2_eq_ppm);
        
        if (err == STATUS_OK) 
        {
            printf("tVOC  Concentration: %dppb\n", tvoc_ppb);
            printf("CO2eq Concentration: %dppm\n", co2_eq_ppm);
        } 
        else 
        {
            printf("error reading IAQ values\n");
        }

        // Persist the current baseline every hour
        if ( (++count % 3600) == 3599) 
        {
            err = sgp30_get_iaq_baseline(&iaq_baseline);
            
            if (err == STATUS_OK) 
            {
                // IMPLEMENT: store baseline to presistent storage
            }
        }

        HAL_Delay(1000);
    }

}//end main


/******************************************************************************
 *                           L O C A L  F U N C T I O N S
 *****************************************************************************/

/******************************************************************************
 *                             End of file
 ******************************************************************************/
/** @}*/