
from PIL import Image
import io

# Leer la cadena hexadecimal desde el archivo 'jpeg.txt'
with open('edges4', 'r') as file:
    hex_data = file.read().strip()

# Convertir la cadena hexadecimal en bytes
image_data = bytes.fromhex(hex_data)

# Crear un objeto de bytes en memoria
image_stream = io.BytesIO(image_data)

# Abrir la imagen usando Pillow
image = Image.open(image_stream)

# Mostrar la imagen
image.show()
