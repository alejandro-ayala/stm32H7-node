# Release Notes
## Version 1.0.0 - 2025-06-13

### New features
- Componente para medición de tiempos de ejecución implementado usando periférico Timer.
- Funcionalidad de estadísticas del sistema añadida.

### Improvements
- La tarea `SendData` cambia de usar `sleep` a una ejecución asíncrona, mejorando la eficiencia.
- `SystemTasks::sendData` eliminó el `delayUntil`, ahora espera directamente la llegada de un nuevo `ImageSnapshot` en la cola de capturas.

### Fixes
- Corrección en `CanController::receiveMsg` para evitar retorno de frames CAN no inicializados cuando no se recibe mensaje.


## Version 0.1.0 - 2025-06-05

### Feature
- Increased CAN bitrate from 125kbit/s to 250kbit/s.
- Implemented `STM32Timer` for clock synchronization.
- Added `SendOfFrame` and `SendOfImage` synchronization logic.
- Assembled frame sync with master node.
- Improved image capture timing and frame rate.
- Enabled FDCAN IRQs and callbacks.
- `ImageSnapshot` now uses `unique_ptr` to manage buffer memory.
- Inference pipeline integrated and running.
- Refactored logger to avoid dynamic memory usage.

### Performance
- DCIM DMA buffer capture optimized.
- Improved capture delay logic to reduce latency.

### Fix
- Fixed memory leaks in `ImageSnapshot` and `JpegCompressor`.
- Corrected memory synchronization for `GlobalClock` during initialization.
- Fixed out-of-bound index in `CommunicationManager::sendData`.
- Fixed payload index miscalculation in CAN frame.
- Added mutex to `CanController` to protect TX/RX mutual exclusion.

---

## Version 0.0.1 - 2025-02-04

### Feature
- Release v0.0.1 includes:
  - Full SW architecture including application, business_logic, hardware_abstraction and services layer. 
  - Working node with whole process: capturing image, procesing edge and sending to master node. 
  - Clock sync component as slave. 

## Version 0.0.0 - 2025-02-04

### Feature
- Release v0.0.0 includes:
  - Master branch creation
  - `release-notes.md`, `README.md` and `version.txt` files
  - Basic CI/CD pipeline
