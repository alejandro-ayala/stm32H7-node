
import numpy as np
from PIL import Image

# Dimensiones de la imagen RAW en escala de grises
width = 96#320  # Ancho de la imagen
height = 96#240  # Alto de la imagen
channels = 1
# Leer la cadena hexadecimal desde el archivo 'raw.txt'
hex_data = ""
if channels == 1:
    with open('ImageClassifierToBeInference', 'r') as file:
        hex_data = file.read().strip()
elif channels == 3:
    with open('rawImage_3ch', 'r') as file:
        hex_data = file.read().strip()
    


# Convertir la cadena hexadecimal en bytes
image_data = bytes.fromhex(hex_data)

# Asegurarse de que la longitud de los datos coincide con las dimensiones de la imagen
if len(image_data) != width * height * channels:
    print(f"[DEBUG] len(image_data): {len(image_data)}, width * height: {width * height}")
    raise ValueError("La longitud de los datos de la imagen no coincide con las dimensiones especificadas para una imagen en escala de grises")

# Convertir los bytes en un array numpy
# Convertir el array numpy a una imagen PIL en modo 'L' (escala de grises)
if channels == 1:
    # Escala de grises
    image_array = np.frombuffer(image_data, dtype=np.uint8).reshape((height, width))
    image = Image.fromarray(image_array, mode='L')
    image.show()
elif channels == 3:
    # Imagen RGB
    image_array = np.frombuffer(image_data, dtype=np.uint8).reshape((height, width, 3))
    image = Image.fromarray(image_array, mode='RGB')
    image.show()
#image_array = np.frombuffer(image_data, dtype=np.uint8).reshape((240, 320, 3))
#image = Image.fromarray(image_array, mode='RGB')

# Mostrar la imagen

