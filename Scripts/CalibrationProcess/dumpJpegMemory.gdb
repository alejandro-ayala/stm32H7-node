set confirm off

define dump_memory_to_file
	print i
    set $rawImgBuffer = rawImgBuffer
    set $size = bufferSize 
    set $endExportedMemory = $rawImgBuffer + $size   
    if i == 0
        dump ihex memory Scripts/CalibrationProcess/resources/jpeg0.hex $rawImgBuffer $endExportedMemory
	end
    if i == 1
        dump ihex memory Scripts/CalibrationProcess/resources/jpeg1.hex $rawImgBuffer $endExportedMemory
	end
    if i == 2
        dump ihex memory Scripts/CalibrationProcess/resources/jpeg2.hex $rawImgBuffer $endExportedMemory
	end
    if i == 3
        dump ihex memory Scripts/CalibrationProcess/resources/jpeg3.hex $rawImgBuffer $endExportedMemory
	end
    if i == 4
        dump ihex memory Scripts/CalibrationProcess/resources/jpeg4.hex $rawImgBuffer $endExportedMemory
	end
    if i == 5
        dump ihex memory Scripts/CalibrationProcess/resources/jpeg5.hex $rawImgBuffer $endExportedMemory
	end
    if i == 6
        dump ihex memory Scripts/CalibrationProcess/resources/jpeg6.hex $rawImgBuffer $endExportedMemory
	end
    if i == 7
        dump ihex memory Scripts/CalibrationProcess/resources/jpeg7.hex $rawImgBuffer $endExportedMemory
	end
    if i == 8
        dump ihex memory Scripts/CalibrationProcess/resources/jpeg8.hex $rawImgBuffer $endExportedMemory
	end
    if i == 9
        dump ihex memory Scripts/CalibrationProcess/resources/jpeg9.hex $rawImgBuffer $endExportedMemory
	end
    if i == 10
        dump ihex memory Scripts/CalibrationProcess/resources/jpeg10.hex $rawImgBuffer $endExportedMemory
    end
    if i == 11
        dump ihex memory Scripts/CalibrationProcess/resources/jpeg11.hex $rawImgBuffer $endExportedMemory
	end
    if i == 12
        dump ihex memory Scripts/CalibrationProcess/resources/jpeg12.hex $rawImgBuffer $endExportedMemory
	end
    if i == 13
        dump ihex memory Scripts/CalibrationProcess/resources/jpeg13.hex $rawImgBuffer $endExportedMemory
	end
    if i == 14
        dump ihex memory Scripts/CalibrationProcess/resources/jpeg14.hex $rawImgBuffer $endExportedMemory
	end
    if i == 15
        dump ihex memory Scripts/CalibrationProcess/resources/jpeg15.hex $rawImgBuffer $endExportedMemory
	end
    if i == 16
        dump ihex memory Scripts/CalibrationProcess/resources/jpeg16.hex $rawImgBuffer $endExportedMemory
	end
    if i == 17
        dump ihex memory Scripts/CalibrationProcess/resources/jpeg17.hex $rawImgBuffer $endExportedMemory
	end
    if i == 18
        dump ihex memory Scripts/CalibrationProcess/resources/jpeg18.hex $rawImgBuffer $endExportedMemory
	end
    if i == 19
        dump ihex memory Scripts/CalibrationProcess/resources/jpeg19.hex $rawImgBuffer $endExportedMemory
	end
    if i == 20
        dump ihex memory Scripts/CalibrationProcess/resources/jpeg20.hex $rawImgBuffer $endExportedMemory
	end
	continue
end

dump_memory_to_file