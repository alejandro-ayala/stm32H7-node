from PIL import Image
import re
import os
import sys

# Forzar UTF-8 en Windows (evitar UnicodeEncodeError)
if os.name == 'nt':
    sys.stdout.reconfigure(encoding='utf-8')

# Configuración general
output_c_array = "imagen_escalada_gray.h"
width = 96
height = 96

print("➡ Verificando imagen desde el array C exportado...")

try:
    with open(output_c_array, "r") as f_array:
        c_code = f_array.read()

    match = re.search(r'\{([^}]+)\}', c_code, re.DOTALL)
    if not match:
        raise ValueError("No se pudo encontrar el bloque de datos entre llaves.")

    data_block = match.group(1)
    hex_values = re.findall(r'0x([0-9A-Fa-f]{2})', data_block)
    if not hex_values:
        raise ValueError("No se encontraron valores hexadecimales en el bloque de datos.")

    reconstructed_bytes = bytes(int(h, 16) for h in hex_values)

    expected_size = width * height
    actual_size = len(reconstructed_bytes)
    if actual_size != expected_size:
        raise ValueError(f"Tamaño incorrecto: esperado {expected_size} bytes, obtenido {actual_size} bytes.")

    img_verificada = Image.frombytes('L', (width, height), reconstructed_bytes)
    img_verificada.show(title="Verificación desde array C (Grises)")
    print("✅ Imagen en escala de grises verificada correctamente desde array C.")

except Exception as e:
    print(f"❌ Error: {e}")

print("🚀 Proceso finalizado.")
