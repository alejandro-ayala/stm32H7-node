set confirm off

define dump_memory_to_file
	print i
    set $ptrSerializedMsg = ptrSerializedMsg
    set $size = serializedMsgSize 
    set $endExportedMemory = $ptrSerializedMsg + $size   
    if i == 0
        dump ihex memory Scripts/Resources/memory_dump/cbor0.hex $ptrSerializedMsg $endExportedMemory
	end
    if i == 1
        dump ihex memory Scripts/Resources/memory_dump/cbor1.hex $ptrSerializedMsg $endExportedMemory
	end
    if i == 2
        dump ihex memory Scripts/Resources/memory_dump/cbor2.hex $ptrSerializedMsg $endExportedMemory
	end
    if i == 3
        dump ihex memory Scripts/Resources/memory_dump/cbor3.hex $ptrSerializedMsg $endExportedMemory
	end
    if i == 4
        dump ihex memory Scripts/Resources/memory_dump/cbor4.hex $ptrSerializedMsg $endExportedMemory
	end
    if i == 5
        dump ihex memory Scripts/Resources/memory_dump/cbor5.hex $ptrSerializedMsg $endExportedMemory
	end
    if i == 6
        dump ihex memory Scripts/Resources/memory_dump/cbor6.hex $ptrSerializedMsg $endExportedMemory
	end
    if i == 7
        dump ihex memory Scripts/Resources/memory_dump/cbor7.hex $ptrSerializedMsg $endExportedMemory
	end
    if i == 8
        dump ihex memory Scripts/Resources/memory_dump/cbor8.hex $ptrSerializedMsg $endExportedMemory
	end
    if i == 9
        dump ihex memory Scripts/Resources/memory_dump/cbor9.hex $ptrSerializedMsg $endExportedMemory
	end
    if i == 10
        dump ihex memory Scripts/Resources/memory_dump/cbor10.hex $ptrSerializedMsg $endExportedMemory
    end
	continue
end

dump_memory_to_file