set confirm off

define dump_memory_to_file
	print i
    set $ptrImgDeque = ptrImgDeque
    set $size = nextSnapshot->m_imgSize 
    set $endExportedMemory = $ptrImgDeque + $size   
    if i == 0
        dump ihex memory C:/Users/Alex/Desktop/ExportedJpeg/stm32Cube/memory0.hex $ptrImgDeque $endExportedMemory
	end
    if i == 1
        dump ihex memory C:/Users/Alex/Desktop/ExportedJpeg/stm32Cube/memory1.hex $ptrImgDeque $endExportedMemory
	end
    if i == 2
        dump ihex memory C:/Users/Alex/Desktop/ExportedJpeg/stm32Cube/memory2.hex $ptrImgDeque $endExportedMemory
	end
    if i == 3
        dump ihex memory C:/Users/Alex/Desktop/ExportedJpeg/stm32Cube/memory3.hex $ptrImgDeque $endExportedMemory
	end
    if i == 4
        dump ihex memory C:/Users/Alex/Desktop/ExportedJpeg/stm32Cube/memory4.hex $ptrImgDeque $endExportedMemory
	end
    if i == 5
        dump ihex memory C:/Users/Alex/Desktop/ExportedJpeg/stm32Cube/memory5.hex $ptrImgDeque $endExportedMemory
	end
    if i == 6
        dump ihex memory C:/Users/Alex/Desktop/ExportedJpeg/stm32Cube/memory6.hex $ptrImgDeque $endExportedMemory
	end
    if i == 7
        dump ihex memory C:/Users/Alex/Desktop/ExportedJpeg/stm32Cube/memory7.hex $ptrImgDeque $endExportedMemory
	end
    if i == 8
        dump ihex memory C:/Users/Alex/Desktop/ExportedJpeg/stm32Cube/memory8.hex $ptrImgDeque $endExportedMemory
	end
    if i == 9
        dump ihex memory C:/Users/Alex/Desktop/ExportedJpeg/stm32Cube/memory9.hex $ptrImgDeque $endExportedMemory
	end
    if i == 10
        dump ihex memory C:/Users/Alex/Desktop/ExportedJpeg/stm32Cube/memory10.hex $ptrImgDeque $endExportedMemory
    end
end

dump_memory_to_file