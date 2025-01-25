import os
import cbor2

# Clase ImageSnapshot
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
            print("Data:")
            print(self.byte_sequence)
            self.deserialized_data = cbor2.loads(self.byte_sequence)
            
            # Extraer los campos
            self.msg_id = self.deserialized_data.get('msgId', None)
            self.img_size = self.deserialized_data.get('imgSize', None)
            self.img_buffer = self.deserialized_data.get('imgBuffer', None)
            self.timestamp = self.deserialized_data.get('timestamp', None)

        except Exception as e:
            print(f"Error deserializando la secuencia de bytes: {e}")

    def print_fields(self):
        """Devuelve los campos deserializados como un string."""
        fields = []
        if self.deserialized_data:
            fields.append(f"msgId: {self.msg_id}")
            fields.append(f"imgSize: {self.img_size}")
            fields.append(f"timestamp: {self.timestamp}")

            if self.img_buffer:
                # Mostrar todos los bytes del buffer de imagen
                img_buffer_hex = ' '.join(f'{byte:02X}' for byte in self.img_buffer)
                fields.append(f"imgBuffer (completo en formato hexadecimal):\n{img_buffer_hex}")
        else:
            fields.append("No se ha deserializado la secuencia de bytes todavía.")
        
        return '\n'.join(fields)

# Ruta de los archivos CBOR (debe estar en el mismo directorio que este script)
directory_path = "."  # Directorio actual

# Iterar sobre todos los archivos que empiezan con "CborFrame" y terminan en ".txt"
for filename in os.listdir(directory_path):
    if filename.startswith("CborFrame"): #and filename.endswith(".txt"):
        cbor_file_path = os.path.join(directory_path, filename)

        # Leer el contenido del archivo
        if os.path.exists(cbor_file_path):
            with open(cbor_file_path, "r") as file:
                # Leer el contenido del archivo
                cbor_hex_data = file.read().replace(" ", "").strip()  # Eliminar espacios y saltos de línea

            try:
                # Convertir datos hexadecimales a bytes
                cbor_byte_data = bytes.fromhex(cbor_hex_data)

                # Crear una instancia de ImageSnapshot
                snapshot = ImageSnapshot(cbor_byte_data)
                
                # Deserializar los datos
                snapshot.deserialize()

                # Crear nombre del archivo de salida
                output_filename = filename.replace(".txt", "Deserialized.txt")
                output_file_path = os.path.join(directory_path, output_filename)

                # Escribir los campos deserializados en el archivo de salida
                with open(output_file_path, "w") as output_file:
                    fields = snapshot.print_fields()
                    output_file.write(fields)
                    print(f"Campos deserializados escritos en: {output_file_path}")

            except ValueError as e:
                print(f"Error al procesar los datos hexadecimales en {filename}: {e}")
        else:
            print(f"Archivo {cbor_file_path} no encontrado.")
