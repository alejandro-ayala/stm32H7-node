from PIL import Image, ImageFile
import io

ImageFile.LOAD_TRUNCATED_IMAGES = True

with open('AssembledPtr', 'r') as file:
    hex_data = file.read().strip()

image_data = bytes.fromhex(hex_data)
image_stream = io.BytesIO(image_data)

image = Image.open(image_stream)
image.load()  # Forzar la carga completa
image.show()

