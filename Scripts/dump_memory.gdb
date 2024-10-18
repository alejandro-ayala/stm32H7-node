set confirm off

define dump_memory_to_file
	print i
    set $ptrImgChunkBuffer = ptrImgChunkBuffer
    set $size = 128 
    set $endExportedMemory = $ptrImgChunkBuffer + $size   
    if i == 0
        dump ihex memory Scripts/Resources/memory_dump/memory0.hex $ptrImgChunkBuffer $endExportedMemory
	end
    if i == 1
        dump ihex memory Scripts/Resources/memory_dump/memory1.hex $ptrImgChunkBuffer $endExportedMemory
	end
    if i == 2
        dump ihex memory Scripts/Resources/memory_dump/memory2.hex $ptrImgChunkBuffer $endExportedMemory
	end
    if i == 3
        dump ihex memory Scripts/Resources/memory_dump/memory3.hex $ptrImgChunkBuffer $endExportedMemory
	end
    if i == 4
        dump ihex memory Scripts/Resources/memory_dump/memory4.hex $ptrImgChunkBuffer $endExportedMemory
	end
    if i == 5
        dump ihex memory Scripts/Resources/memory_dump/memory5.hex $ptrImgChunkBuffer $endExportedMemory
	end
    if i == 6
        dump ihex memory Scripts/Resources/memory_dump/memory6.hex $ptrImgChunkBuffer $endExportedMemory
	end
    if i == 7
        dump ihex memory Scripts/Resources/memory_dump/memory7.hex $ptrImgChunkBuffer $endExportedMemory
	end
    if i == 8
        dump ihex memory Scripts/Resources/memory_dump/memory8.hex $ptrImgChunkBuffer $endExportedMemory
	end
    if i == 9
        dump ihex memory Scripts/Resources/memory_dump/memory9.hex $ptrImgChunkBuffer $endExportedMemory
	end
    if i == 10
        dump ihex memory Scripts/Resources/memory_dump/memory10.hex $ptrImgChunkBuffer $endExportedMemory
    end
	continue
end

dump_memory_to_file