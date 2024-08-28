#include "../../Controllers/I2C/I2CController.h"
#include "services/Exception/SystemExceptions.h"
#include <string.h>

namespace hardware_abstraction
{
namespace Controllers
{

I2CController::I2CController(const I2C_HandleTypeDef* i2cCfg) : m_hi2c2(*i2cCfg)
{

}

void I2CController::initialize()
{
	  /* USER CODE BEGIN I2C1_Init 0 */

	  /* USER CODE END I2C1_Init 0 */

	  /* USER CODE BEGIN I2C1_Init 1 */

	  /* USER CODE END I2C1_Init 1 */
	  m_hi2c2.Instance = I2C1;
	  m_hi2c2.Init.Timing = 0x60404E72;
	  m_hi2c2.Init.OwnAddress1 = 0;
	  m_hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	  m_hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	  m_hi2c2.Init.OwnAddress2 = 0;
	  m_hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
	  m_hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	  m_hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	  if (HAL_I2C_Init(&m_hi2c2) != HAL_OK)
	  {
	    //Error_Handler();
	  }

	  /** Configure Analogue filter
	  */
	  if (HAL_I2CEx_ConfigAnalogFilter(&m_hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
	  {
	    //Error_Handler();
	  }

	  /** Configure Digital filter
	  */
	  if (HAL_I2CEx_ConfigDigitalFilter(&m_hi2c2, 0) != HAL_OK)
	  {
	    //Error_Handler();
	  }
	  /* USER CODE BEGIN I2C1_Init 2 */

	  /* USER CODE END I2C1_Init 2 */
}

short I2CController::receive(uint8_t reg_addr, uint8_t *pdata)
{
	short opertionStatus = 0;
	HAL_StatusTypeDef connectionStatus;
	__disable_irq();
	connectionStatus = HAL_I2C_Master_Transmit(&m_hi2c2, (uint16_t) 0x60,
			&reg_addr, 1, 100);
	if (connectionStatus == HAL_OK) {
		connectionStatus = HAL_I2C_Master_Receive(&m_hi2c2, (uint16_t) 0x61, pdata,
				1, 100);
		if (connectionStatus == HAL_OK) {
			opertionStatus = 0;
		} else {
			opertionStatus = 1;
		}
	} else {
		opertionStatus = 2;
	}
	__enable_irq();
	return opertionStatus;
}

short I2CController::send(uint8_t reg_addr, uint8_t data)
{
	short opertionStatus = 0;
	uint8_t buffer[2] = { 0 };
	HAL_StatusTypeDef connectionStatus;
	buffer[0] = reg_addr;
	buffer[1] = data;
	__disable_irq();
	connectionStatus = HAL_I2C_Master_Transmit(&m_hi2c2, (uint16_t) 0x60, buffer,
			2, 100);
	if (connectionStatus == HAL_OK) {
		opertionStatus = 1;
	} else {
		opertionStatus = 0;
	}
	__enable_irq();
	return opertionStatus;
}
}
}
