#include "CanController.h"
#include <cstring>
#include "services/Logger/LoggerMacros.h"

namespace hardware_abstraction
{
namespace Controllers
{

CanController::CanController(FDCAN_HandleTypeDef& hfdcan1) : IController("CanController"), m_hfdcan1(hfdcan1)
{
	canMutex = xSemaphoreCreateMutex();
	if (canMutex == nullptr) {
		LOG_INFO("ERROR creating canMutex");

	}
	initialize();
    HAL_FDCAN_DeInit(&m_hfdcan1);
    initialize();
}

CanController::~CanController()
{
    if (canMutex != nullptr) {
    	LOG_INFO("~CanControlle vSemaphoreDelete");
        vSemaphoreDelete(canMutex);
        canMutex = nullptr;
    }
}

void CanController::initialize()
{


	  /* USER CODE BEGIN FDCAN1_Init 0 */
	 // HAL_FDCAN_DeInit(&m_hfdcan1);
	  m_hfdcan1.Instance = FDCAN1;
	  m_hfdcan1.Init.FrameFormat = FDCAN_CLASSIC_CAN;
	  m_hfdcan1.Init.Mode = FDCAN_MODE_NORMAL;
	  m_hfdcan1.Init.AutoRetransmission = ENABLE;
	  m_hfdcan1.Init.TransmitPause = ENABLE;
	  m_hfdcan1.Init.ProtocolException = DISABLE;

	  m_hfdcan1.Init.NominalPrescaler = 25;
	  m_hfdcan1.Init.NominalSyncJumpWidth = 50;
	  m_hfdcan1.Init.NominalTimeSeg1 = 15;
	  m_hfdcan1.Init.NominalTimeSeg2 = 6;
	  m_hfdcan1.Init.DataPrescaler = 4;
	  m_hfdcan1.Init.DataSyncJumpWidth = 5;
	  m_hfdcan1.Init.DataTimeSeg1 = 19;
	  m_hfdcan1.Init.DataTimeSeg2 = 5;

	  m_hfdcan1.Init.MessageRAMOffset = 0;
	  m_hfdcan1.Init.StdFiltersNbr = 1;
	  m_hfdcan1.Init.ExtFiltersNbr = 0;
	  m_hfdcan1.Init.RxFifo0ElmtsNbr = 2;
	  m_hfdcan1.Init.RxFifo0ElmtSize = FDCAN_DATA_BYTES_8;
	  m_hfdcan1.Init.RxFifo1ElmtsNbr = 0;
	  m_hfdcan1.Init.RxBuffersNbr = 0;
	  m_hfdcan1.Init.TxEventsNbr = 0;
	  m_hfdcan1.Init.TxBuffersNbr = 0;
	  m_hfdcan1.Init.TxFifoQueueElmtsNbr = 32;
	  m_hfdcan1.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
	  m_hfdcan1.Init.TxElmtSize = FDCAN_DATA_BYTES_8;
	  HAL_FDCAN_Init(&m_hfdcan1);

	  /* Configure Rx filter */
	  FDCAN_FilterTypeDef        sFilterConfig;
	  sFilterConfig.IdType = FDCAN_STANDARD_ID;
	  sFilterConfig.FilterIndex = 0;
	  sFilterConfig.FilterType = FDCAN_FILTER_MASK;
	  sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
	  sFilterConfig.FilterID1 = 0x000;  // Aceptar desde ID 0x000
	  sFilterConfig.FilterID2 = 0x000;  // Máscara en 0x000 acepta todo
	  HAL_FDCAN_ConfigFilter(&m_hfdcan1, &sFilterConfig);

	  /* Configure global filter to reject all non-matching frames */
	  //HAL_FDCAN_ConfigGlobalFilter(&m_hfdcan1, FDCAN_REJECT, FDCAN_REJECT, FDCAN_REJECT_REMOTE, FDCAN_REJECT_REMOTE);
	  HAL_FDCAN_ConfigGlobalFilter(&m_hfdcan1, FDCAN_ACCEPT_IN_RX_FIFO0, FDCAN_ACCEPT_IN_RX_FIFO0, FDCAN_ACCEPT_IN_RX_FIFO0, FDCAN_ACCEPT_IN_RX_FIFO0);

	  //HAL_FDCAN_RegisterCallback(&m_hfdcan1, FDCAN_IT_TX_COMPLETE, HAL_FDCAN_TxBufferCompleteCallback);
	  //HAL_FDCAN_ActivateNotification(&m_hfdcan1, FDCAN_IT_TX_COMPLETE, 0);
	  if (HAL_FDCAN_ActivateNotification(&m_hfdcan1, FDCAN_IT_TX_COMPLETE | FDCAN_IT_RAM_ACCESS_FAILURE |FDCAN_IT_ERROR_LOGGING_OVERFLOW |FDCAN_IT_RAM_WATCHDOG   |FDCAN_IT_ARB_PROTOCOL_ERROR |FDCAN_IT_DATA_PROTOCOL_ERROR  |FDCAN_IT_RESERVED_ADDRESS_ACCESS |FDCAN_IT_ERROR_PASSIVE  |FDCAN_IT_ERROR_WARNING|FDCAN_IT_BUS_OFF, FDCAN_TX_BUFFER0 | FDCAN_TX_BUFFER1 | FDCAN_TX_BUFFER2) != HAL_OK)
	  {
		  LOG_WARNING("HAL_FDCAN_ActivateNotification ERROR");
	  }
	  /* Start the FDCAN module */
	  HAL_FDCAN_Start(&m_hfdcan1);
	  /* USER CODE END FDCAN1_Init 0*/
}

int CanController::transmitMsg(uint8_t idMsg, const uint8_t *txMsg, uint8_t dataSize)
{
//    HAL_FDCAN_DeInit(&m_hfdcan1);
//    initialize();
	FDCAN_TxHeaderTypeDef l_txHeader;
	l_txHeader.Identifier = idMsg;
	l_txHeader.IdType = FDCAN_STANDARD_ID;
	l_txHeader.TxFrameType = FDCAN_DATA_FRAME;
	//TODO review DataLenght size control!
	l_txHeader.DataLength = dataSize;//FDCAN_DLC_BYTES_64;
	l_txHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
	l_txHeader.BitRateSwitch = FDCAN_BRS_OFF;
	l_txHeader.FDFormat = FDCAN_CLASSIC_CAN;//FDCAN_CLASSIC_CAN;
	l_txHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
	l_txHeader.MessageMarker = 0x0;
	//LOG_INFO("TAKE2");
	xSemaphoreTake(canMutex, portMAX_DELAY);
	//LOG_INFO("TAKEN2");
	auto fifoSpace = HAL_FDCAN_GetTxFifoFreeLevel(&m_hfdcan1);
	static uint32_t txFailed = 0;
	if(fifoSpace < 30)txFailed++;
//	if(fifoSpace < 2)
//	{
//		LOG_WARNING("CanController::transmitMsg fifoSpace is almost full: ", std::to_string(fifoSpace));
//	}
//	else LOG_TRACE("CanController::transmitMsg fifoSpace: ", std::to_string(fifoSpace));
	uint8_t waitForSpace = 0;

	while(fifoSpace < 10)
	{
		if(waitForSpace > 5)
			LOG_ERROR("CanController::transmitMsg fifoSpace is full: availableSpace: ", std::to_string(fifoSpace) , " Retry: ", std::to_string(waitForSpace));
		else if(waitForSpace > 50)
		{
			LOG_ERROR("CanController::transmitMsg fifoSpace is full: availableSpace: ", std::to_string(fifoSpace), " Going ahead");
		}
		vTaskDelay(10 / portTICK_PERIOD_MS);
		fifoSpace = HAL_FDCAN_GetTxFifoFreeLevel(&m_hfdcan1);
		waitForSpace++;
	}

	if (HAL_FDCAN_AddMessageToTxFifoQ(&m_hfdcan1, &l_txHeader, txMsg) != HAL_OK)
	{
		LOG_ERROR("CanController::HAL_FDCAN_AddMessageToTxFifoQ NOT ADDED ");
		xSemaphoreGive(canMutex);
		return HAL_ERROR;
	}
	//LOG_INFO("GIVE2");
	xSemaphoreGive(canMutex);
	//LOG_INFO("GIVEN2");
}

business_logic::Communication::CanFrame CanController::receiveMsg()
{

	business_logic::Communication::CanFrame rxMsg;
	uint8_t rxBuffer[16];
	//LOG_INFO("TAKE");
	xSemaphoreTake(canMutex, portMAX_DELAY);
	//LOG_INFO("TAKEN");
	const auto pendingMsg = HAL_FDCAN_GetRxFifoFillLevel(&m_hfdcan1, FDCAN_RX_FIFO0);
	if (pendingMsg >0)
	{
		HAL_FDCAN_GetRxMessage(&m_hfdcan1, FDCAN_RX_FIFO0, &rxHeader, rxBuffer);
		rxMsg.id  = rxHeader.Identifier;
		rxMsg.dlc = rxHeader.DataLength;
		memcpy( rxMsg.data , rxBuffer , rxHeader.DataLength);
	}
	else
		rxMsg.dlc = 0;
	//LOG_INFO("GIVE");
	xSemaphoreGive(canMutex);
	//LOG_INFO("GIVEN");
	return rxMsg;
}

void CanController::integrationTest()
{
  HAL_FDCAN_DeInit(&m_hfdcan1);
  uint8_t rxData[16U];
  FDCAN_TxHeaderTypeDef txHeader;
  m_hfdcan1.Instance = FDCAN1;
  m_hfdcan1.Init.FrameFormat = FDCAN_FRAME_FD_BRS;
  m_hfdcan1.Init.Mode = FDCAN_MODE_NORMAL;
  m_hfdcan1.Init.AutoRetransmission = ENABLE;
  m_hfdcan1.Init.TransmitPause = DISABLE;
  m_hfdcan1.Init.ProtocolException = ENABLE;

  m_hfdcan1.Init.NominalPrescaler = 50;
  m_hfdcan1.Init.NominalSyncJumpWidth = 50;
  m_hfdcan1.Init.NominalTimeSeg1 = 15;
  m_hfdcan1.Init.NominalTimeSeg2 = 6;
  m_hfdcan1.Init.DataPrescaler = 4;
  m_hfdcan1.Init.DataSyncJumpWidth = 5;
  m_hfdcan1.Init.DataTimeSeg1 = 19;
  m_hfdcan1.Init.DataTimeSeg2 = 5;

  m_hfdcan1.Init.MessageRAMOffset = 0;
  m_hfdcan1.Init.StdFiltersNbr = 1;
  m_hfdcan1.Init.ExtFiltersNbr = 0;
  m_hfdcan1.Init.RxFifo0ElmtsNbr = 2;
  m_hfdcan1.Init.RxFifo0ElmtSize = FDCAN_DATA_BYTES_8;
  m_hfdcan1.Init.RxFifo1ElmtsNbr = 0;
  m_hfdcan1.Init.RxBuffersNbr = 0;
  m_hfdcan1.Init.TxEventsNbr = 0;
  m_hfdcan1.Init.TxBuffersNbr = 0;
  m_hfdcan1.Init.TxFifoQueueElmtsNbr = 2;
  m_hfdcan1.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
  m_hfdcan1.Init.TxElmtSize = FDCAN_DATA_BYTES_8;
  HAL_FDCAN_Init(&m_hfdcan1);

  /* Configure Rx filter */
  FDCAN_FilterTypeDef        sFilterConfig;
  sFilterConfig.IdType = FDCAN_STANDARD_ID;
  sFilterConfig.FilterIndex = 0;
  sFilterConfig.FilterType = FDCAN_FILTER_MASK;
  sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
  sFilterConfig.FilterID1 = 0x111;
  sFilterConfig.FilterID2 = 0x7FF; /* For acceptance, MessageID and FilterID1 must match exactly */
  HAL_FDCAN_ConfigFilter(&m_hfdcan1, &sFilterConfig);

  /* Configure global filter to reject all non-matching frames */
  HAL_FDCAN_ConfigGlobalFilter(&m_hfdcan1, FDCAN_REJECT, FDCAN_REJECT, FDCAN_REJECT_REMOTE, FDCAN_REJECT_REMOTE);

  /* Start the FDCAN module */
  HAL_FDCAN_Start(&m_hfdcan1);

  /* Infinite loop */
  auto enable_tx = 1;
  static const uint8_t txData[] = {0x10, 0x32, 0x54, 0x76, 0x98, 0x00, 0x11, 0x22,
                                   0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0x00
                                  };
  while(1)
  {

	if(enable_tx)
	{

		  txHeader.Identifier = 0x122;
		  txHeader.IdType = FDCAN_STANDARD_ID;
		  txHeader.TxFrameType = FDCAN_DATA_FRAME;
		  txHeader.DataLength = FDCAN_DLC_BYTES_8;
		  txHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
		  txHeader.BitRateSwitch = FDCAN_BRS_OFF;
		  txHeader.FDFormat = FDCAN_CLASSIC_CAN;
		  txHeader.TxEventFifoControl = FDCAN_STORE_TX_EVENTS;
		  txHeader.MessageMarker = 0xCC;

	    HAL_FDCAN_AddMessageToTxFifoQ(&m_hfdcan1, &txHeader, &txData[0]);
	    //HAL_FDCAN_AddMessageToTxFifoQ(&m_hfdcan1, &txHeader, &TxData[8]);
	}
	else
	{
		/* Wait for two messages received */
		while (HAL_FDCAN_GetRxFifoFillLevel(&m_hfdcan1, FDCAN_RX_FIFO0) < 2) {}
	/* Retrieve Rx messages from RX FIFO0 */
		HAL_FDCAN_GetRxMessage(&m_hfdcan1, FDCAN_RX_FIFO0, &rxHeader, rxData);
		HAL_FDCAN_GetRxMessage(&m_hfdcan1, FDCAN_RX_FIFO0, &rxHeader, &rxData[8]);
	}
  }
}

void CanController::externalLoopbackTest()
{
	uint8_t rxData[16U];
	HAL_FDCAN_DeInit(&m_hfdcan1);
	FDCAN_TxHeaderTypeDef txHeader;
	static const uint8_t  txData0[] = {0x10, 0x32, 0x54, 0x76, 0x98, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
	static const uint8_t  txData1[] = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};
	static const uint8_t  txData2[] = {0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00};
	  /* Initializes the FDCAN peripheral in loopback mode */
	  m_hfdcan1.Instance = FDCAN1;
	  m_hfdcan1.Init.FrameFormat = FDCAN_FRAME_FD_BRS;
	  m_hfdcan1.Init.Mode = FDCAN_MODE_EXTERNAL_LOOPBACK;
	  m_hfdcan1.Init.AutoRetransmission = ENABLE;
	  m_hfdcan1.Init.TransmitPause = DISABLE;
	  m_hfdcan1.Init.ProtocolException = ENABLE;
	  /* Bit time configuration:
	    ************************
	    Bit time parameter         | Nominal      |  Data
	    ---------------------------|--------------|----------------
	    fdcan_ker_ck               | 20 MHz       | 20 MHz
	    Time_quantum (tq)          | 50 ns        | 50 ns
	    Synchronization_segment    | 1 tq         | 1 tq
	    Propagation_segment        | 23 tq        | 1 tq
	    Phase_segment_1            | 8 tq         | 4 tq
	    Phase_segment_2            | 8 tq         | 4 tq
	    Synchronization_Jump_width | 8 tq         | 4 tq
	    Bit_length                 | 40 tq = 2 µs | 10 tq = 0.5 µs
	    Bit_rate                   | 0.5 MBit/s   | 2 MBit/s
	  */
//	  m_hfdcan1.Init.NominalPrescaler = 0x1; /* tq = NominalPrescaler x (1/fdcan_ker_ck) */
//	  m_hfdcan1.Init.NominalSyncJumpWidth = 0x8;
//	  m_hfdcan1.Init.NominalTimeSeg1 = 0x1F; /* NominalTimeSeg1 = Propagation_segment + Phase_segment_1 */
//	  m_hfdcan1.Init.NominalTimeSeg2 = 0x8;
//	  m_hfdcan1.Init.DataPrescaler = 0x1;
//	  m_hfdcan1.Init.DataSyncJumpWidth = 0x4;
//	  m_hfdcan1.Init.DataTimeSeg1 = 0x5; /* DataTimeSeg1 = Propagation_segment + Phase_segment_1 */
//	  m_hfdcan1.Init.DataTimeSeg2 = 0x4;

	  m_hfdcan1.Init.NominalPrescaler = 50;
	  m_hfdcan1.Init.NominalSyncJumpWidth = 50;
	  m_hfdcan1.Init.NominalTimeSeg1 = 15;
	  m_hfdcan1.Init.NominalTimeSeg2 = 6;
	  m_hfdcan1.Init.DataPrescaler = 4;
	  m_hfdcan1.Init.DataSyncJumpWidth = 5;
	  m_hfdcan1.Init.DataTimeSeg1 = 19;
	  m_hfdcan1.Init.DataTimeSeg2 = 5;

	  m_hfdcan1.Init.MessageRAMOffset = 0;
	  m_hfdcan1.Init.StdFiltersNbr = 1;
	  m_hfdcan1.Init.ExtFiltersNbr = 1;
	  m_hfdcan1.Init.RxFifo0ElmtsNbr = 1;
	  m_hfdcan1.Init.RxFifo0ElmtSize = FDCAN_DATA_BYTES_12;
	  m_hfdcan1.Init.RxFifo1ElmtsNbr = 2;
	  m_hfdcan1.Init.RxFifo1ElmtSize = FDCAN_DATA_BYTES_12;
	  m_hfdcan1.Init.RxBuffersNbr = 1;
	  m_hfdcan1.Init.RxBufferSize = FDCAN_DATA_BYTES_12;
	  m_hfdcan1.Init.TxEventsNbr = 2;
	  m_hfdcan1.Init.TxBuffersNbr = 1;
	  m_hfdcan1.Init.TxFifoQueueElmtsNbr = 2;
	  m_hfdcan1.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
	  m_hfdcan1.Init.TxElmtSize = FDCAN_DATA_BYTES_12;
	  HAL_FDCAN_Init(&m_hfdcan1);
	  FDCAN_FilterTypeDef        sFilterConfig;
	  /* Configure standard ID reception filter to Rx buffer 0 */
	  sFilterConfig.IdType = FDCAN_STANDARD_ID;
	  sFilterConfig.FilterIndex = 0;
	  sFilterConfig.FilterType = FDCAN_FILTER_DUAL;
	  sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXBUFFER;
	  sFilterConfig.FilterID1 = 0x111;
	  sFilterConfig.FilterID2 = 0x555;
	  sFilterConfig.RxBufferIndex = 0;
	  HAL_FDCAN_ConfigFilter(&m_hfdcan1, &sFilterConfig);

	  /* Configure extended ID reception filter to Rx FIFO 1 */
	  sFilterConfig.IdType = FDCAN_EXTENDED_ID;
	  sFilterConfig.FilterIndex = 0;
	  sFilterConfig.FilterType = FDCAN_FILTER_RANGE_NO_EIDM;
	  sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO1;
	  sFilterConfig.FilterID1 = 0x1111111;
	  sFilterConfig.FilterID2 = 0x2222222;
	  HAL_FDCAN_ConfigFilter(&m_hfdcan1, &sFilterConfig);

	  /* Configure Tx buffer message */
	  txHeader.Identifier = 0x111;
	  txHeader.IdType = FDCAN_STANDARD_ID;
	  txHeader.TxFrameType = FDCAN_DATA_FRAME;
	  txHeader.DataLength = FDCAN_DLC_BYTES_12;
	  txHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
	  txHeader.BitRateSwitch = FDCAN_BRS_ON;
	  txHeader.FDFormat = FDCAN_FD_CAN;
	  txHeader.TxEventFifoControl = FDCAN_STORE_TX_EVENTS;
	  txHeader.MessageMarker = 0x52;
	  HAL_FDCAN_AddMessageToTxBuffer(&m_hfdcan1, &txHeader, txData0, FDCAN_TX_BUFFER0);

	  /* Start the FDCAN module */
	  HAL_FDCAN_Start(&m_hfdcan1);

	  /* Send Tx buffer message */
	  HAL_FDCAN_EnableTxBufferRequest(&m_hfdcan1, FDCAN_TX_BUFFER0);

	  /* Add message to Tx FIFO */
	  txHeader.Identifier = 0x1111112;
	  txHeader.IdType = FDCAN_EXTENDED_ID;
	  txHeader.TxFrameType = FDCAN_DATA_FRAME;
	  txHeader.DataLength = FDCAN_DLC_BYTES_12;
	  txHeader.ErrorStateIndicator = FDCAN_ESI_PASSIVE;
	  txHeader.BitRateSwitch = FDCAN_BRS_ON;
	  txHeader.FDFormat = FDCAN_FD_CAN;
	  txHeader.TxEventFifoControl = FDCAN_STORE_TX_EVENTS;
	  txHeader.MessageMarker = 0xCC;
	  HAL_FDCAN_AddMessageToTxFifoQ(&m_hfdcan1, &txHeader, txData1);

	  /* Add second message to Tx FIFO */
	  txHeader.Identifier = 0x2222222;
	  txHeader.IdType = FDCAN_EXTENDED_ID;
	  txHeader.TxFrameType = FDCAN_DATA_FRAME;
	  txHeader.DataLength = FDCAN_DLC_BYTES_12;
	  txHeader.ErrorStateIndicator = FDCAN_ESI_PASSIVE;
	  txHeader.BitRateSwitch = FDCAN_BRS_OFF;
	  txHeader.FDFormat = FDCAN_FD_CAN;
	  txHeader.TxEventFifoControl = FDCAN_STORE_TX_EVENTS;
	  txHeader.MessageMarker = 0xDD;
	  HAL_FDCAN_AddMessageToTxFifoQ(&m_hfdcan1, &txHeader, txData2);

	  /* Retrieve message from Rx buffer 0 */
	  HAL_FDCAN_GetRxMessage(&m_hfdcan1, FDCAN_RX_BUFFER0, &rxHeader, rxData);

	  /* Retrieve message from Rx FIFO 1 */
	  HAL_FDCAN_GetRxMessage(&m_hfdcan1, FDCAN_RX_FIFO1, &rxHeader, rxData);


	  /* Retrieve next message from Rx FIFO 1 */
	  HAL_FDCAN_GetRxMessage(&m_hfdcan1, FDCAN_RX_FIFO1, &rxHeader, rxData);


}
bool CanController::selfTest()
{
	return true;
}
}
}
