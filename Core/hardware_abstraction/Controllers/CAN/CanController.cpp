#include "CanController.h"

namespace hardware_abstraction
{
namespace Controllers
{

CanController::CanController() : IController("CanController")
{

}

CanController::~CanController()
{

}

void CanController::initialize()
{
	  /* USER CODE BEGIN FDCAN1_Init 0 */

	  /* USER CODE END FDCAN1_Init 0 */

	  /* USER CODE BEGIN FDCAN1_Init 1 */

	  /* USER CODE END FDCAN1_Init 1 */
	  hfdcan1.Instance = FDCAN1;
	  hfdcan1.Init.FrameFormat = FDCAN_FRAME_CLASSIC;
	  hfdcan1.Init.Mode = FDCAN_MODE_NORMAL;
	  hfdcan1.Init.AutoRetransmission = DISABLE;
	  hfdcan1.Init.TransmitPause = DISABLE;
	  hfdcan1.Init.ProtocolException = DISABLE;
	  hfdcan1.Init.NominalPrescaler = 16;
	  hfdcan1.Init.NominalSyncJumpWidth = 1;
	  hfdcan1.Init.NominalTimeSeg1 = 2;
	  hfdcan1.Init.NominalTimeSeg2 = 2;
	  hfdcan1.Init.DataPrescaler = 1;
	  hfdcan1.Init.DataSyncJumpWidth = 1;
	  hfdcan1.Init.DataTimeSeg1 = 1;
	  hfdcan1.Init.DataTimeSeg2 = 1;
	  hfdcan1.Init.MessageRAMOffset = 0;
	  hfdcan1.Init.StdFiltersNbr = 0;
	  hfdcan1.Init.ExtFiltersNbr = 0;
	  hfdcan1.Init.RxFifo0ElmtsNbr = 0;
	  hfdcan1.Init.RxFifo0ElmtSize = FDCAN_DATA_BYTES_8;
	  hfdcan1.Init.RxFifo1ElmtsNbr = 0;
	  hfdcan1.Init.RxFifo1ElmtSize = FDCAN_DATA_BYTES_8;
	  hfdcan1.Init.RxBuffersNbr = 0;
	  hfdcan1.Init.RxBufferSize = FDCAN_DATA_BYTES_8;
	  hfdcan1.Init.TxEventsNbr = 0;
	  hfdcan1.Init.TxBuffersNbr = 0;
	  hfdcan1.Init.TxFifoQueueElmtsNbr = 0;
	  hfdcan1.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
	  hfdcan1.Init.TxElmtSize = FDCAN_DATA_BYTES_8;
	  if (HAL_FDCAN_Init(&hfdcan1) != HAL_OK)
	  {
	    //Error_Handler();
	  }
	  /* USER CODE BEGIN FDCAN1_Init 2 */

	  /* USER CODE END FDCAN1_Init 2 */
}

int CanController::transmitMsg(uint8_t idMsg, uint8_t *TxMsg, uint8_t msgLength)
{

	//HAL_CAN_AddTxMessage(&_hcan1, &pHeader, TxMsg, &TxMailbox);
}

int CanController::receiveMsg(uint8_t *rxBuffer)
{
	return 1;
}

bool CanController::selfTest()
{
	return true;
}
}
}
