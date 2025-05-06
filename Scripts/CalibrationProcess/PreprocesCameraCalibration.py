import os
import io
import sys
import subprocess
from PIL import Image

def run_preprocessing_script():
    script = 'convertIntelHex2PlainText.py'
    folder = 'resources/'

    script_path = os.path.join(os.path.dirname(__file__), script)
    folder_path = os.path.join(os.path.dirname(__file__), folder)

    try:
        print(f"[INFO] Ejecutando script de preparación: {script_path} {folder_path}")
        subprocess.run(['python3.exe', script_path, folder_path], check=True)
        print("[OK] Script de preparación finalizado.")
    except subprocess.CalledProcessError as e:
        print(f"[ERROR] Falló el script de preparación (código {e.returncode})")
        sys.exit(1)

def process_hex_txt_file(filepath):
    with open(filepath, 'r') as file:
        hex_data = file.read().strip()

    try:
        image_data = bytes.fromhex(hex_data)
    except ValueError as e:
        print(f"[ERROR] Archivo inválido {filepath}: {e}")
        return

    try:
        image = Image.open(io.BytesIO(image_data))
    except Exception as e:
        print(f"[ERROR] No se pudo abrir la imagen en {filepath}: {e}")
        return

    #image.show()

    output_path = os.path.splitext(filepath)[0] + '.jpeg'
    try:
        image.save(output_path, format='JPEG')
        print(f"[OK] Imagen guardada: {output_path}")
    except Exception as e:
        print(f"[ERROR] No se pudo guardar {output_path}: {e}")

def main(directory):
    if not os.path.isdir(directory):
        print(f"[ERROR] Carpeta no encontrada: {directory}")
        return

    for filename in os.listdir(directory):
        if filename.lower().endswith('.txt'):
            full_path = os.path.join(directory, filename)
            process_hex_txt_file(full_path)

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Uso: python convert_hex_images.py <carpeta_con_txt>")
        sys.exit(1)

    run_preprocessing_script()
    main(sys.argv[1])
