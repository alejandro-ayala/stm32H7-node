
import numpy as np
from PIL import Image

# Dimensiones de la imagen RAW en escala de grises
width = 320  # Ancho de la imagen
height = 240  # Alto de la imagen

# Leer la cadena hexadecimal desde el archivo 'raw.txt'
with open('rawImageAssembler', 'r') as file:
    hex_data = file.read().strip()

# Convertir la cadena hexadecimal en bytes
image_data = bytes.fromhex(hex_data)

# Asegurarse de que la longitud de los datos coincide con las dimensiones de la imagen
if len(image_data) != width * height:
    raise ValueError("La longitud de los datos de la imagen no coincide con las dimensiones especificadas para una imagen en escala de grises")

# Convertir los bytes en un array numpy
image_array = np.frombuffer(image_data, dtype=np.uint8).reshape((height, width))

# Convertir el array numpy a una imagen PIL en modo 'L' (escala de grises)
image = Image.fromarray(image_array, mode='L')

# Mostrar la imagen
image.show()
