import can
from collections import defaultdict
import re
import cbor2
from typing import List, Dict, Tuple

# Definición de clases y tipos
ImageID = int
CborID = int
FrameIndex = int
DataPayload = bytes
MessageKey = Tuple[ImageID, CborID]

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
                # Mostrar algunos bytes del buffer de imagen para verificar
                print(f"imgBuffer (primeros 10 bytes): {self.img_buffer[:10]}")
        else:
            print("No se ha deserializado la secuencia de bytes todavía.")
            
class CANMessage:
    """Clase para representar un mensaje CAN."""
    def __init__(self, message_id: int, data: bytes):
        self.message_id = message_id  # ID del mensaje CAN
        self.data = data  # Datos del mensaje CAN (8 bytes)

        # Parsing de los bytes según la especificación
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

def read_can_messages(channel, output_file):
    """Lee mensajes CAN desde el canal especificado y los almacena en un archivo."""
    # Configurar la conexión con la PCAN-USB a 125 kbit/s usando 'interface'
    bus = can.interface.Bus(interface='pcan', channel=channel, bitrate=125000)
    assembler = CBORAssembler()  # Instancia del ensamblador CBOR

    print(f"Listening for CAN messages on {channel}...")

    # Abrir el archivo en modo append
    with open(output_file, 'a') as file:
        try:
            while True:
                message = bus.recv(timeout=1)  # Leer mensajes con timeout de 1 segundo
                if message:
                    # Convertir los datos del mensaje a una cadena de bytes hexadecimales separados por espacios
                    data_hex = ' '.join(f"{byte:02x}" for byte in message.data)
                    # Imprimir mensaje en la consola
                    print(f"Message received: ID: {message.arbitration_id}, Data: {data_hex}, DLC: {message.dlc}", flush=True)
                    
                    # Procesar el mensaje y agregarlo al ensamblador
                    assembler.add_message(CANMessage(message.arbitration_id, message.data))

                    # Almacenar el mensaje en el archivo
                    #file.write(f"{data_hex}\n")

                # Revisar si tenemos tramas CBOR completas (opcional, puedes definir cuándo es el momento adecuado)
                #ordered_tramas = assembler.get_all_ordered_tramas()
                #for key, tramas in ordered_tramas.items():
                #    image_id, cbor_id = key
                #    print(f"\nImagen ID: {image_id}, Cbor ID: {cbor_id}")
                #    for idx, trama in enumerate(tramas):
                #        print(f"  Trama {idx}: {trama.hex()}")

        except KeyboardInterrupt:
            print("Stopped listening for CAN messages.")
            ordered_tramas = assembler.get_all_ordered_tramas()
            for key, tramas in ordered_tramas.items():
                image_id, cbor_id = key
                print(f"\nImagen ID: {image_id}, Cbor ID: {cbor_id}")
                
                concatenated_tramas = ''.join(trama.hex() for trama in tramas)
                #print(f"  Tramas concatenadas: {concatenated_tramas}")
                
                # Crear instancia de ImageSnapshot
                snapshot = ImageSnapshot(bytes.fromhex(concatenated_tramas))

                # Deserializar la secuencia de bytes
                snapshot.deserialize()

                # Imprimir los campos
                #snapshot.print_fields()
                print(' '.join(f"{byte:02X}" for byte in snapshot.img_buffer))
                
                
                #guardar la imagen 
                
                #hex_string = ' '.join(f"{byte:02X}" for byte in snapshot.img_buffer)
                # Escribir en el archivo y añadir una nueva línea
                #file.write(hex_string)

        finally:
            bus.shutdown()  # Cerrar la conexión al bus CAN

if __name__ == "__main__":
    # Aquí puedes especificar el canal, como 'PCAN_USBBUS1'
    channel = 'PCAN_USBBUS1'  # Asegúrate de cambiar esto si tu bus es diferente
    output_file = 'image.jpeg'  # Nombre del archivo donde almacenar los mensajes
    read_can_messages(channel, output_file)
