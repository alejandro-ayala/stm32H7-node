from PIL import Image
import re
import sys
import os

# Configuración general
input_image = "captura.png"
output_raw = "imagen_320x240_gray.raw"
output_c_array = "imagen_320x240_gray_array.txt"
width = 320
height = 240

# Forzar UTF-8 en Windows (para evitar errores de consola)
if os.name == 'nt':
    sys.stdout.reconfigure(encoding='utf-8')

# Paso 1: Leer imagen y generar RAW + array C en escala de grises
print("➡ Leyendo imagen...")
img = Image.open(input_image)
img_resized = img.resize((width, height), Image.LANCZOS)
img_gray = img_resized.convert("L")  # Convertir a escala de grises
img_bytes = img_gray.tobytes()

print(f"➡ Guardando como RAW ({output_raw})...")
with open(output_raw, "wb") as f_raw:
    f_raw.write(img_bytes)

print(f"➡ Exportando como array C ({output_c_array})...")
with open(output_c_array, "w") as f_array:
    f_array.write(f"const unsigned char imagen_{width}x{height}_gray[] = {{\n")

    for i in range(0, len(img_bytes)):
        if i % 16 == 0:
            f_array.write("    ")
        f_array.write(f"0x{img_bytes[i]:02X}")
        if i < len(img_bytes) - 1:
            f_array.write(", ")
        if (i + 1) % 16 == 0:
            f_array.write("\n")

    f_array.write("\n};\n")

print("✅ Conversión a escala de grises completada correctamente.")

# Paso 2: Verificar leyendo el array C y reconstruyendo la imagen
print("➡ Verificando imagen desde el array C exportado...")

with open(output_c_array, "r") as f_array:
    c_code = f_array.read()

match = re.search(r'\{(.+?)\}', c_code, re.DOTALL)
if match:
    data_block = match.group(1)
    hex_values = re.findall(r'0x([0-9A-Fa-f]{2})', data_block)
    reconstructed_bytes = bytes(int(h, 16) for h in hex_values)

    expected_size = width * height  # Solo un byte por píxel en grises
    actual_size = len(reconstructed_bytes)
    if actual_size != expected_size:
        print(f"❌ Error: tamaño esperado {expected_size}, pero se obtuvieron {actual_size} bytes.")
    else:
        img_verificada = Image.frombytes('L', (width, height), reconstructed_bytes)
        img_verificada.show(title="Verificación desde array C (Grises)")
        print("✅ Imagen en escala de grises verificada correctamente desde array C.")
else:
    print("❌ Error: no se pudo encontrar el bloque de datos entre llaves.")

print("🚀 Proceso finalizado.")
