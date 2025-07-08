import can
import time
import threading
from collections import defaultdict
import re
import cbor2
from typing import List, Dict, Tuple
import os
from PIL import Image
import cv2
import struct
# Definición de clases y tipos
ImageID = int
CborID = int
FrameIndex = int
DataPayload = bytes
MessageKey = Tuple[ImageID, CborID]
updateGlobalClkPeriod = 3
channel = 'PCAN_USBBUS1'
output_file = 'image.jpeg' 
bus = can.interface.Bus(interface='pcan', channel=channel, bitrate=250000)

class ImageSnapshot:
    def __init__(self, byte_sequence):
        """Inicializa la clase con la secuencia de bytes CBOR."""
        self.byte_sequence = byte_sequence
        self.msg_id = None
        self.img_size = None
        self.img_buffer = None
        self.timestamp = None
        self.deserialized_data = None

    def deserialize(self):
        """Deserializa la secuencia de bytes CBOR y extrae los campos relevantes."""
        try:
            # Deserializar la secuencia de bytes usando cbor2
            self.deserialized_data = cbor2.loads(self.byte_sequence)

            # Extraer los campos
            self.msg_id = self.deserialized_data.get('msgId', None)
            self.img_size = self.deserialized_data.get('imgSize', None)
            self.img_buffer = self.deserialized_data.get('imgBuffer', None)
            self.timestamp = self.deserialized_data.get('timestamp', None)

        except Exception as e:
            print(f"Error deserializando la secuencia de bytes: {e}")

    def print_fields(self):
        """Imprime los campos deserializados."""
        if self.deserialized_data:
            print(f"msgId: {self.msg_id}")
            print(f"imgSize: {self.img_size}")
            print(f"timestamp: {self.timestamp}")

            if self.img_buffer:
                print(f"imgBuffer (primeros 10 bytes): {self.img_buffer[:10]}")
        else:
            print("No se ha deserializado la secuencia de bytes")
            
class CANMessage:
    """Clase para representar un mensaje CAN."""
    def __init__(self, message_id: int, data: bytes):
        self.message_id = message_id  # ID del mensaje CAN
        self.data = data  # Datos del mensaje CAN (8 bytes)

        self.image_id = (self.data[0] >> 6) & 0b11  # Bits 7 y 6
        self.cbor_id = self.data[0] & 0b00111111  # Bits 5 a 0
        self.frame_index = self.data[1]  # Byte[1]

    def __repr__(self):
        return (f"CANMessage(ID: {self.message_id}, Data: {' '.join(f'{b:02X}' for b in self.data)}, "
                f"ImageID: {self.image_id}, CborID: {self.cbor_id}, FrameIndex: {self.frame_index})")

class CBORAssembler:
    """Clase para ensamblar tramas CBOR a partir de mensajes CAN."""
    def __init__(self):
        # Diccionario para almacenar las tramas agrupadas por (image_id, cbor_id)
        self.tramas: Dict[MessageKey, Dict[FrameIndex, DataPayload]] = defaultdict(dict)

    def add_message(self, can_message: CANMessage):
        """Agrega un mensaje CAN al ensamblador."""
        key = (can_message.image_id, can_message.cbor_id)
        index = can_message.frame_index
        data = can_message.data[2:8]  # Bytes 2 a 7 contienen la carga útil

        if index in self.tramas[key]:
            print(f"Advertencia: Índice de trama duplicado {index} para {key}. Sobrescribiendo la trama existente.")

        self.tramas[key][index] = data
        #print(f"Mensaje agregado: {can_message}")

    def get_ordered_cbor_tramas(self, image_id: ImageID, cbor_id: CborID) -> List[DataPayload]:
    
        """Retorna las tramas CBOR ordenadas para una imagen y cbor_id específicos."""
        key = (image_id, cbor_id)
        if key not in self.tramas:
            print(f"No se encontraron tramas para ImageID: {image_id}, CborID: {cbor_id}")
            return []

        # Ordenar las tramas por FrameIndex
        ordered_indices = sorted(self.tramas[key].keys())
        ordered_tramas = [self.tramas[key][index] for index in ordered_indices]
        return ordered_tramas

    def get_all_ordered_tramas(self) -> Dict[MessageKey, List[DataPayload]]:
        """Retorna todas las tramas CBOR ordenadas agrupadas por (image_id, cbor_id)."""
        ordered_tramas_dict = {}
        for key in self.tramas:
            ordered_tramas_dict[key] = self.get_ordered_cbor_tramas(*key)
        return ordered_tramas_dict

    def get_all_received_tramas(self) -> Dict[MessageKey, List[DataPayload]]:
        """Retorna todas las tramas CBOR en el orden en que fueron recibidas agrupadas por (image_id, cbor_id)."""
        received_tramas_dict = {}
        for key in self.tramas:
            received_tramas_dict[key] = self.get_received_cbor_tramas(*key)
        return received_tramas_dict
    def clear(self):
        """Limpia todas las tramas almacenadas."""
        self.tramas.clear()
        print("Todas las tramas han sido eliminadas.")

def reconstruct_and_view_jpeg(output_directory, final_image_filename="FullJpegImage.jpg"):
    """
    Lee fragmentos de imágenes JPEG almacenados en archivos, los concatena y guarda la imagen completa.
    Luego, la visualiza usando PIL (Pillow) y OpenCV.
    
    Parámetros:
        output_directory (str): Ruta donde están los fragmentos.
        final_image_filename (str): Nombre del archivo final.
    """
    # Obtener todos los fragmentos JPEG en orden
    fragment_files = sorted(
        [f for f in os.listdir(output_directory) if f.startswith("JpegFrame") and f.endswith(".txt")],
        key=lambda x: int(x.replace("JpegFrame", "").replace(".txt", ""))
    )

    # Nombre del archivo final
    final_image_fullpath = os.path.join(output_directory, final_image_filename)

    # Concatenar los fragmentos y escribir la imagen reconstruida en binario
    with open(final_image_fullpath, 'wb') as full_image:
        for fragment in fragment_files:
            fragment_path = os.path.join(output_directory, fragment)

            # Leer el contenido en hexadecimal y convertirlo a binario
            with open(fragment_path, 'r') as file:
                hex_data = file.read().replace(" ", "")  # Eliminar espacios
                binary_data = bytes.fromhex(hex_data)
                full_image.write(binary_data)  # Escribir en el archivo final

    print(f"Imagen reconstruida guardada en: {final_image_fullpath}")

    img = Image.open(final_image_fullpath)
    img.show()

    img_cv2 = cv2.imread(final_image_fullpath)
    cv2.imshow("Imagen Completa", img_cv2)
    cv2.waitKey(0)
    cv2.destroyAllWindows()
    
def read_can_messages():
    """Lee mensajes CAN desde el canal especificado y los almacena en un archivo."""

    assembler = CBORAssembler()  # Instancia del ensamblador CBOR

    print(f"Listening for CAN messages on {channel}...")

    # Abrir el archivo en modo append
    with open(output_file, 'a') as file:
        try:
            while True:
                message = bus.recv(timeout=1)  # Leer mensajes con timeout de 1 segundo
                if message:
                    data_hex = ' '.join(f"{byte:02x}" for byte in message.data)
                    print(f"Message received: ID: {message.arbitration_id}, Data: {data_hex}, DLC: {message.dlc}", flush=True)
                    
                    assembler.add_message(CANMessage(message.arbitration_id, message.data))

                    #file.write(f"{data_hex}\n")

        except KeyboardInterrupt:
            print("Stopped listening for CAN messages.")
            ordered_tramas = assembler.get_all_ordered_tramas()
            for key, tramas in ordered_tramas.items():
                image_id, cbor_id = key
                print(f"\nImagen ID: {image_id}, Cbor ID: {cbor_id}")
                concatenated_tramas = ''.join(trama.hex() for trama in tramas)
                #print(f"  Tramas concatenadas: {concatenated_tramas}")
                
                snapshot = ImageSnapshot(bytes.fromhex(concatenated_tramas))

                snapshot.deserialize()

                #snapshot.print_fields()
                
                output_directory = 'Resources/memory_dump'
                
                os.makedirs(output_directory, exist_ok=True)

                #guardar Cbor frame
                cbor_frame = ' '.join(f"{byte:02X}" for byte in bytes.fromhex(concatenated_tramas))
                print("Cbor Frame: ")
                print(cbor_frame)
                print("Concatenated Tramas: ")
                print(concatenated_tramas)
                cbor_frame_filename = "CborFrame" + str(cbor_id) + ".txt"
                cbor_frame_fullpath_filename = os.path.join(output_directory, cbor_frame_filename)
                
                with open(cbor_frame_fullpath_filename, 'w') as file:
                    file.write(cbor_frame)

                jpeg_frame = ' '.join(f"{byte:02X}" for byte in snapshot.img_buffer)
                jpeg_frame_filename = "JpegFrame" + str(cbor_id) + ".txt"
                jpeg_frame_fullpath_filename = os.path.join(output_directory, jpeg_frame_filename)  
                with open(jpeg_frame_fullpath_filename, 'w') as file:
                    file.write(jpeg_frame)         
                #hex_string = ' '.join(f"{byte:02X}" for byte in snapshot.img_buffer)
                # Escribir en el archivo y añadir una nueva línea
                #file.write(hex_string)
            reconstruct_and_view_jpeg(output_directory)

        finally:
            bus.shutdown()  # Cerrar la conexión al bus CAN

def send_periodic_messages():
    

    while True:
        current_time_ns = time.time_ns()
        seconds = current_time_ns // 1_000_000_000  # Segundos actuales
        nanoseconds = current_time_ns % 1_000_000_000  # Porción de nanosegundos
        print(f"Segundos: {seconds}, Nanosegundos: {nanoseconds}")

        # Serializa segundos y nanosegundos en 4 bytes (big-endian)
        sec_bytes = struct.pack('>I', seconds)  # 4 bytes para los segundos
        ns_bytes = struct.pack('>I', nanoseconds)  # 4 bytes para los nanosegundos
        
        syncMessage = can.Message(
            arbitration_id=0x17,  # ID en formato estándar
            data=[0x20, 0xFE, 0x00, 0x13] + list(sec_bytes),
            is_extended_id=False  # CAN estándar
        )
        followUpMessage = can.Message(
            arbitration_id=0x17,  # ID en formato estándar
            data=[0x28, 0xF1, 0x00, 0x00] + list(ns_bytes), 
            is_extended_id=False  # CAN estándar
        )       
        try:
            bus.send(syncMessage)
            #print("syncMessage")
            time.sleep(0.1)
            bus.send(followUpMessage)
            #print("followUpMessage")
            time.sleep(updateGlobalClkPeriod)
        except can.CanError as e:
            print(f"Error enviando mensaje: {e}")
            
if __name__ == "__main__":
    receive_thread = threading.Thread(target=read_can_messages, daemon=True)
    #send_thread = threading.Thread(target=send_periodic_messages, daemon=True)
    #send_thread.start()
    receive_thread.start()
    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        print("Programa terminado.")
