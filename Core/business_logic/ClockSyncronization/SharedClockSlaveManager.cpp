#include <iostream>

#include "../ClockSyncronization/SharedClockSlaveManager.h"
#include "../Communication/CanIDs.h"
#include "CanSyncMessages.h"
#include "services/Logger/LoggerMacros.h"
#include <sstream>

namespace business_logic
{
using namespace Communication;
namespace ClockSyncronization
{
SharedClockSlaveManager::SharedClockSlaveManager(std::shared_ptr<TimeController> timecontroller, std::shared_ptr<hardware_abstraction::Controllers::CanController> cancontroller) : timeController(timecontroller), canController(cancontroller)
{

}

SharedClockSlaveManager::~SharedClockSlaveManager() {}

void SharedClockSlaveManager::initialization()
{
	timeController->initialize();
	canController->initialize();
	timeController->startTimer();
}


bool SharedClockSlaveManager::synqGlobalTime()
{
	uint8_t cnt = 0;
	uint8_t lenght = 800;
	bool updatedTime;
	CanSyncMessage globalTimeSynMsg;
	CanFUPMessage  globalTimeFupMsg;
	const auto rxMsg = canController->receiveMsg();
	if(rxMsg.dlc != 0 && rxMsg.id == static_cast<uint8_t>(CAN_IDs::CLOCK_SYNC))
	{
		if(rxMsg.data[0] == static_cast<uint8_t>(SYNC_MSG_TYPES::SYNC_MSG))
		{
			globalTimeSynMsg.type        = rxMsg.data[0];
			globalTimeSynMsg.crc         = rxMsg.data[1];
			globalTimeSynMsg.secCounter  = rxMsg.data[2];
			globalTimeSynMsg.userByte    = rxMsg.data[3];
			globalTimeSynMsg.syncTimeSec = rxMsg.data[4] << 24 | rxMsg.data[5] << 16 | rxMsg.data[6] << 8 | rxMsg.data[7];

			globalTimeStamp.seconds = globalTimeSynMsg.syncTimeSec;
			globalTimeStamp.t2vlt   = timeController->getCurrentNsecTime();
			LOG_TRACE("globalTimeSynMsg --> type: ", globalTimeSynMsg.type, " CRC: ", globalTimeSynMsg.crc, " secCounter: ", globalTimeSynMsg.secCounter );
			LOG_TRACE("globalTimeStamp.seconds: ", globalTimeStamp.seconds);
			updatedTime = false;
		}
		else if(rxMsg.data[0] == static_cast<uint8_t>(SYNC_MSG_TYPES::FUP_MSG))
		{
			globalTimeFupMsg.type        = rxMsg.data[0];
			globalTimeFupMsg.crc         = rxMsg.data[1];
			globalTimeFupMsg.secCounter  = rxMsg.data[2];
			globalTimeFupMsg.overflowSeconds    = rxMsg.data[3];
			globalTimeFupMsg.syncTimeNSec = rxMsg.data[4] << 24 | rxMsg.data[5] << 16 | rxMsg.data[6] << 8 | rxMsg.data[7];

			globalTimeStamp.nanoseconds=globalTimeFupMsg.syncTimeNSec;
			globalTimeStamp.t5vlt   = timeController->getGlobalTime();

/****************************/
			// 1. Reconstruir el tiempo de envío exacto del maestro.
			uint64_t masterSendTime = (globalTimeStamp.seconds * 1e9) + globalTimeStamp.nanoseconds;

			// 2. Obtener el tiempo de recepción local del esclavo (guardado al recibir el SYNC).
			uint64_t slaveReceptionTime = globalTimeStamp.t2vlt;

			// 3. Calcular el desfase. Esta es la fórmula clave.
			//    Offset = (Tiempo del Maestro cuando envió) - (Tiempo del Esclavo cuando recibió)
			int64_t newOffset = masterSendTime - slaveReceptionTime;
			std::string strOffset = "Clock offset = (masterSendTime - slaveReceptionTime): " + std::to_string(newOffset) + " = " + std::to_string(masterSendTime) + " - " + std::to_string(slaveReceptionTime);
			LOG_INFO(strOffset);
			timeController->setClockOffset(newOffset);
			updatedTime = true;
/****************************/

		}
		else
		{
			LOG_WARNING("Wrong CAN_IDs::CLOCK_SYNC: size = " , std::to_string(rxMsg.dlc));
			for(int i=0;i<rxMsg.dlc;i++)
			{
				LOG_WARNING(rxMsg.data[i], ",");
			}
			updatedTime = false;
		}
	}
	else
	{
		updatedTime = false;
	}
	return updatedTime;
}

double SharedClockSlaveManager::getGlobalTime() const
{
	return timeController->getGlobalTime();
}

void SharedClockSlaveManager::localClockTest() const
{
    const uint32_t testIntervals[] = {
        1000,     // 1s
        5000,     // 5s
        10000,    // 10s
        30000,    // 30s
        60000,    // 1min
        600000,   // 10min
        900000,   // 15min
    };
    constexpr size_t numIntervals = sizeof(testIntervals) / sizeof(testIntervals[0]);
    size_t currentIntervalIndex = 0;
    std::ostringstream oss;
    oss << "********** [SharedClockSlaveManager::localClockTest] ********** ";
    oss << "  InitialAbsoluteTime:   "     << getLocalTimeReference() << " ns";
    while (true)
    {
        uint32_t currentInterval = testIntervals[currentIntervalIndex];

        auto localTimeBefore = getLocalTimeReference();

        vTaskDelay(pdMS_TO_TICKS(currentInterval));

        auto localTimeAfter = getLocalTimeReference();

        auto localTimeDiff = localTimeAfter - localTimeBefore;
        auto elapsedTimeCnt = timeController->getElapsedTimeCounter();

        std::ostringstream oss;
        oss << "  SleepTask: "        << (currentInterval*1e6) << " ns";
        oss << "  elapsedTimeCnt: "   << elapsedTimeCnt;
        oss << "  localTime: "        << localTimeAfter << " ns";
        oss << "  localTimeDiff: "    << localTimeDiff << " ns";
        LOG_INFO(oss.str());

        currentIntervalIndex = (currentIntervalIndex + 1) % numIntervals;
    }
}

}
}
