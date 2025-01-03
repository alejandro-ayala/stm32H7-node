#include "CanController.h"

namespace hardware_abstraction
{
namespace Controllers
{

CanController::CanController() : IController("CanController")
{
	initialize();
    HAL_FDCAN_DeInit(&hfdcan1);
    initialize();
}

CanController::~CanController()
{

}

void CanController::initialize()
{


	  /* USER CODE BEGIN FDCAN1_Init 0 */
	 // HAL_FDCAN_DeInit(&hfdcan1);
	  hfdcan1.Instance = FDCAN1;
	  hfdcan1.Init.FrameFormat = FDCAN_FRAME_FD_BRS;
	  hfdcan1.Init.Mode = FDCAN_MODE_NORMAL;
	  hfdcan1.Init.AutoRetransmission = ENABLE;
	  hfdcan1.Init.TransmitPause = DISABLE;
	  hfdcan1.Init.ProtocolException = ENABLE;

	  hfdcan1.Init.NominalPrescaler = 50;
	  hfdcan1.Init.NominalSyncJumpWidth = 50;
	  hfdcan1.Init.NominalTimeSeg1 = 15;
	  hfdcan1.Init.NominalTimeSeg2 = 6;
	  hfdcan1.Init.DataPrescaler = 4;
	  hfdcan1.Init.DataSyncJumpWidth = 5;
	  hfdcan1.Init.DataTimeSeg1 = 19;
	  hfdcan1.Init.DataTimeSeg2 = 5;

	  hfdcan1.Init.MessageRAMOffset = 0;
	  hfdcan1.Init.StdFiltersNbr = 1;
	  hfdcan1.Init.ExtFiltersNbr = 0;
	  hfdcan1.Init.RxFifo0ElmtsNbr = 2;
	  hfdcan1.Init.RxFifo0ElmtSize = FDCAN_DATA_BYTES_8;
	  hfdcan1.Init.RxFifo1ElmtsNbr = 0;
	  hfdcan1.Init.RxBuffersNbr = 0;
	  hfdcan1.Init.TxEventsNbr = 0;
	  hfdcan1.Init.TxBuffersNbr = 0;
	  hfdcan1.Init.TxFifoQueueElmtsNbr = 32;
	  hfdcan1.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
	  hfdcan1.Init.TxElmtSize = FDCAN_DATA_BYTES_8;
	  HAL_FDCAN_Init(&hfdcan1);

	  /* Configure Rx filter */
	  FDCAN_FilterTypeDef        sFilterConfig;
	  sFilterConfig.IdType = FDCAN_STANDARD_ID;
	  sFilterConfig.FilterIndex = 0;
	  sFilterConfig.FilterType = FDCAN_FILTER_MASK;
	  sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
	  sFilterConfig.FilterID1 = 0x111;
	  sFilterConfig.FilterID2 = 0x7FF; /* For acceptance, MessageID and FilterID1 must match exactly */
	  HAL_FDCAN_ConfigFilter(&hfdcan1, &sFilterConfig);

	  /* Configure global filter to reject all non-matching frames */
	  HAL_FDCAN_ConfigGlobalFilter(&hfdcan1, FDCAN_REJECT, FDCAN_REJECT, FDCAN_REJECT_REMOTE, FDCAN_REJECT_REMOTE);

	  /* Start the FDCAN module */
	  HAL_FDCAN_Start(&hfdcan1);
	  /* USER CODE END FDCAN1_Init 0*/
}

int CanController::transmitMsg(uint8_t idMsg, const uint8_t *txMsg, uint8_t dataSize)
{
//    HAL_FDCAN_DeInit(&hfdcan1);
//    initialize();
	txHeader.Identifier = idMsg;
	txHeader.IdType = FDCAN_STANDARD_ID;
	txHeader.TxFrameType = FDCAN_DATA_FRAME;
	//TODO review DataLenght size control!
	txHeader.DataLength = dataSize;//FDCAN_DLC_BYTES_64;
	txHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
	txHeader.BitRateSwitch = FDCAN_BRS_OFF;
	txHeader.FDFormat = FDCAN_CLASSIC_CAN;
	txHeader.TxEventFifoControl = FDCAN_STORE_TX_EVENTS;
	txHeader.MessageMarker = 0xCC;
	auto fifoSpace = HAL_FDCAN_GetTxFifoFreeLevel(&hfdcan1);
	while(fifoSpace == 0)
	{
		HAL_Delay(300);
		fifoSpace = HAL_FDCAN_GetTxFifoFreeLevel(&hfdcan1);
	}

	if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &txHeader, txMsg) != HAL_OK)
	{
	  return HAL_ERROR;
	}

}

int CanController::receiveMsg(uint8_t *rxBuffer)
{
	return 1;
}

void CanController::integrationTest()
{
  HAL_FDCAN_DeInit(&hfdcan1);
  uint8_t rxData[16U];
  hfdcan1.Instance = FDCAN1;
  hfdcan1.Init.FrameFormat = FDCAN_FRAME_FD_BRS;
  hfdcan1.Init.Mode = FDCAN_MODE_NORMAL;
  hfdcan1.Init.AutoRetransmission = ENABLE;
  hfdcan1.Init.TransmitPause = DISABLE;
  hfdcan1.Init.ProtocolException = ENABLE;

  hfdcan1.Init.NominalPrescaler = 50;
  hfdcan1.Init.NominalSyncJumpWidth = 50;
  hfdcan1.Init.NominalTimeSeg1 = 15;
  hfdcan1.Init.NominalTimeSeg2 = 6;
  hfdcan1.Init.DataPrescaler = 4;
  hfdcan1.Init.DataSyncJumpWidth = 5;
  hfdcan1.Init.DataTimeSeg1 = 19;
  hfdcan1.Init.DataTimeSeg2 = 5;

  hfdcan1.Init.MessageRAMOffset = 0;
  hfdcan1.Init.StdFiltersNbr = 1;
  hfdcan1.Init.ExtFiltersNbr = 0;
  hfdcan1.Init.RxFifo0ElmtsNbr = 2;
  hfdcan1.Init.RxFifo0ElmtSize = FDCAN_DATA_BYTES_8;
  hfdcan1.Init.RxFifo1ElmtsNbr = 0;
  hfdcan1.Init.RxBuffersNbr = 0;
  hfdcan1.Init.TxEventsNbr = 0;
  hfdcan1.Init.TxBuffersNbr = 0;
  hfdcan1.Init.TxFifoQueueElmtsNbr = 2;
  hfdcan1.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
  hfdcan1.Init.TxElmtSize = FDCAN_DATA_BYTES_8;
  HAL_FDCAN_Init(&hfdcan1);

  /* Configure Rx filter */
  FDCAN_FilterTypeDef        sFilterConfig;
  sFilterConfig.IdType = FDCAN_STANDARD_ID;
  sFilterConfig.FilterIndex = 0;
  sFilterConfig.FilterType = FDCAN_FILTER_MASK;
  sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
  sFilterConfig.FilterID1 = 0x111;
  sFilterConfig.FilterID2 = 0x7FF; /* For acceptance, MessageID and FilterID1 must match exactly */
  HAL_FDCAN_ConfigFilter(&hfdcan1, &sFilterConfig);

  /* Configure global filter to reject all non-matching frames */
  HAL_FDCAN_ConfigGlobalFilter(&hfdcan1, FDCAN_REJECT, FDCAN_REJECT, FDCAN_REJECT_REMOTE, FDCAN_REJECT_REMOTE);

  /* Start the FDCAN module */
  HAL_FDCAN_Start(&hfdcan1);

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

	    HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &txHeader, &txData[0]);
	    //HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &txHeader, &TxData[8]);
	}
	else
	{
		/* Wait for two messages received */
		while (HAL_FDCAN_GetRxFifoFillLevel(&hfdcan1, FDCAN_RX_FIFO0) < 2) {}
	/* Retrieve Rx messages from RX FIFO0 */
		HAL_FDCAN_GetRxMessage(&hfdcan1, FDCAN_RX_FIFO0, &rxHeader, rxData);
		HAL_FDCAN_GetRxMessage(&hfdcan1, FDCAN_RX_FIFO0, &rxHeader, &rxData[8]);
	}
  }
}

void CanController::externalLoopbackTest()
{
	uint8_t rxData[16U];
	HAL_FDCAN_DeInit(&hfdcan1);
	static const uint8_t  txData0[] = {0x10, 0x32, 0x54, 0x76, 0x98, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
	static const uint8_t  txData1[] = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};
	static const uint8_t  txData2[] = {0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00};
	  /* Initializes the FDCAN peripheral in loopback mode */
	  hfdcan1.Instance = FDCAN1;
	  hfdcan1.Init.FrameFormat = FDCAN_FRAME_FD_BRS;
	  hfdcan1.Init.Mode = FDCAN_MODE_EXTERNAL_LOOPBACK;
	  hfdcan1.Init.AutoRetransmission = ENABLE;
	  hfdcan1.Init.TransmitPause = DISABLE;
	  hfdcan1.Init.ProtocolException = ENABLE;
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
//	  hfdcan1.Init.NominalPrescaler = 0x1; /* tq = NominalPrescaler x (1/fdcan_ker_ck) */
//	  hfdcan1.Init.NominalSyncJumpWidth = 0x8;
//	  hfdcan1.Init.NominalTimeSeg1 = 0x1F; /* NominalTimeSeg1 = Propagation_segment + Phase_segment_1 */
//	  hfdcan1.Init.NominalTimeSeg2 = 0x8;
//	  hfdcan1.Init.DataPrescaler = 0x1;
//	  hfdcan1.Init.DataSyncJumpWidth = 0x4;
//	  hfdcan1.Init.DataTimeSeg1 = 0x5; /* DataTimeSeg1 = Propagation_segment + Phase_segment_1 */
//	  hfdcan1.Init.DataTimeSeg2 = 0x4;

	  hfdcan1.Init.NominalPrescaler = 50;
	  hfdcan1.Init.NominalSyncJumpWidth = 50;
	  hfdcan1.Init.NominalTimeSeg1 = 15;
	  hfdcan1.Init.NominalTimeSeg2 = 6;
	  hfdcan1.Init.DataPrescaler = 4;
	  hfdcan1.Init.DataSyncJumpWidth = 5;
	  hfdcan1.Init.DataTimeSeg1 = 19;
	  hfdcan1.Init.DataTimeSeg2 = 5;

	  hfdcan1.Init.MessageRAMOffset = 0;
	  hfdcan1.Init.StdFiltersNbr = 1;
	  hfdcan1.Init.ExtFiltersNbr = 1;
	  hfdcan1.Init.RxFifo0ElmtsNbr = 1;
	  hfdcan1.Init.RxFifo0ElmtSize = FDCAN_DATA_BYTES_12;
	  hfdcan1.Init.RxFifo1ElmtsNbr = 2;
	  hfdcan1.Init.RxFifo1ElmtSize = FDCAN_DATA_BYTES_12;
	  hfdcan1.Init.RxBuffersNbr = 1;
	  hfdcan1.Init.RxBufferSize = FDCAN_DATA_BYTES_12;
	  hfdcan1.Init.TxEventsNbr = 2;
	  hfdcan1.Init.TxBuffersNbr = 1;
	  hfdcan1.Init.TxFifoQueueElmtsNbr = 2;
	  hfdcan1.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
	  hfdcan1.Init.TxElmtSize = FDCAN_DATA_BYTES_12;
	  HAL_FDCAN_Init(&hfdcan1);
	  FDCAN_FilterTypeDef        sFilterConfig;
	  /* Configure standard ID reception filter to Rx buffer 0 */
	  sFilterConfig.IdType = FDCAN_STANDARD_ID;
	  sFilterConfig.FilterIndex = 0;
	  sFilterConfig.FilterType = FDCAN_FILTER_DUAL;
	  sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXBUFFER;
	  sFilterConfig.FilterID1 = 0x111;
	  sFilterConfig.FilterID2 = 0x555;
	  sFilterConfig.RxBufferIndex = 0;
	  HAL_FDCAN_ConfigFilter(&hfdcan1, &sFilterConfig);

	  /* Configure extended ID reception filter to Rx FIFO 1 */
	  sFilterConfig.IdType = FDCAN_EXTENDED_ID;
	  sFilterConfig.FilterIndex = 0;
	  sFilterConfig.FilterType = FDCAN_FILTER_RANGE_NO_EIDM;
	  sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO1;
	  sFilterConfig.FilterID1 = 0x1111111;
	  sFilterConfig.FilterID2 = 0x2222222;
	  HAL_FDCAN_ConfigFilter(&hfdcan1, &sFilterConfig);

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
	  HAL_FDCAN_AddMessageToTxBuffer(&hfdcan1, &txHeader, txData0, FDCAN_TX_BUFFER0);

	  /* Start the FDCAN module */
	  HAL_FDCAN_Start(&hfdcan1);

	  /* Send Tx buffer message */
	  HAL_FDCAN_EnableTxBufferRequest(&hfdcan1, FDCAN_TX_BUFFER0);

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
	  HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &txHeader, txData1);

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
	  HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &txHeader, txData2);

	  /* Retrieve message from Rx buffer 0 */
	  HAL_FDCAN_GetRxMessage(&hfdcan1, FDCAN_RX_BUFFER0, &rxHeader, rxData);

	  /* Retrieve message from Rx FIFO 1 */
	  HAL_FDCAN_GetRxMessage(&hfdcan1, FDCAN_RX_FIFO1, &rxHeader, rxData);


	  /* Retrieve next message from Rx FIFO 1 */
	  HAL_FDCAN_GetRxMessage(&hfdcan1, FDCAN_RX_FIFO1, &rxHeader, rxData);


}
bool CanController::selfTest()
{
	return true;
}
}
}
