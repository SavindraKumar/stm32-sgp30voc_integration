//! @addtogroup SGPApp
//! @brief Implement SGP App
//! @{
//!
//****************************************************************************/
//! @file sgp_app.c
//! @brief SGP app
//! @author Savindra Kumar(savindran1989@gmail.com)
//! @bug No known bugs.
//!
//****************************************************************************/
//****************************************************************************/
//                           Includes
//****************************************************************************/
//standard header files
#include <stdint.h>
#include <stdio.h>
//user defined header files
#include "stm32f4xx_hal.h"
#include "sgp_app.h"
#include "sgp30.h"
#include "sgp_git_version.h"
#include "uart_app.h"

//****************************************************************************/
//                           Defines and typedefs
//****************************************************************************/
#define PRINT_BUF_LEN   256

//****************************************************************************/
//                           Private Functions
//****************************************************************************/
static void SgpSelfTest(void);
static void GetSgpInfo(void);

//****************************************************************************/
//                           external variables
//****************************************************************************/
const char *SGP_DRV_VERSION_STR = "7.0.0";

//****************************************************************************/
//                           Private variables
//****************************************************************************/
static char msg[PRINT_BUF_LEN] = {0};


//****************************************************************************/
//                    G L O B A L  F U N C T I O N S
//****************************************************************************/
void SgpInit(void)
{  
    const char* driver_version = sgp30_get_driver_version();
    
    if (driver_version) 
    {
        sprintf(msg,  "\nSGP30 driver version %s\r\n", driver_version);
        UARTPrint(msg);
    } 
    else 
    {
        sprintf(msg, "fatal: Getting driver version failed\r\n");
        UARTPrint(msg);
    }

    // Initialize I2C bus
    sensirion_i2c_init();
    //first pass self test 
    SgpSelfTest();
    GetSgpInfo();

}//end Init

void SgpPoll(void)
{
   // Consider the two cases (A) and (B):
    //(A) If no baseline is available or the most recent baseline is more than
    //one week old, it must discarded. A new baseline is found with
    //sgp30_iaq_init()
    
    int16_t err = sgp30_iaq_init();
    
    if (STATUS_OK == err) 
    {
        sprintf(msg, "sgp30_iaq_init done\r\n");
        UARTPrint(msg);
    } 
    else 
    {
        sprintf(msg, "sgp30_iaq_init failed!\n");
        UARTPrint(msg);
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
        
        if (STATUS_OK == err) 
        {
            sprintf(msg, "tVOC  Concentration: %dppb\r\n", tvoc_ppb);
            UARTPrint(msg);
            sprintf(msg, "CO2eq Concentration: %dppm\r\n", co2_eq_ppm);
            UARTPrint(msg);
        } 
        else 
        {
            sprintf(msg, "error reading IAQ values\r\n");
            UARTPrint(msg);
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
}//end SgpPoll

/******************************************************************************
 *                           L O C A L  F U N C T I O N S
 *****************************************************************************/
static void SgpSelfTest(void)
{
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
            sprintf(msg, "Sensor need at least feature set version 1.0 (0x20)\n");
            UARTPrint(msg);
        }
                
        sprintf(msg, "SGP sensor probing failed\r\n");
        UARTPrint(msg);
        
        sensirion_sleep_usec(1000000);
    } 
    
    sprintf(msg, "SGP sensor probing successful\r\n");
    UARTPrint(msg);

}

static void GetSgpInfo(void)
{
    uint16_t feature_set_version;
    uint8_t product_type;
    
    int16_t err = sgp30_get_feature_set_version(&feature_set_version, &product_type);
    
    if (STATUS_OK == err) 
    {
        sprintf(msg, "Feature set version: %u\r\n", feature_set_version);
        UARTPrint(msg);
        sprintf(msg, "Product type: %u\r\n", product_type);
        UARTPrint(msg);
    } 
    else 
    {
        sprintf(msg, "sgp30_get_feature_set_version failed!\n");
        UARTPrint(msg);
    }
    
    uint64_t serial_id;
    
    err = sgp30_get_serial_id(&serial_id);
    
    if (STATUS_OK == err)
    {
        sprintf(msg, "SerialID: %u\r\n", serial_id);
        UARTPrint(msg);
    } 
    else 
    {
        sprintf(msg, "sgp30_get_serial_id failed!\n");
        UARTPrint(msg);
    }    
    
}

/******************************************************************************
 *                             End of file
 ******************************************************************************/
/** @}*/