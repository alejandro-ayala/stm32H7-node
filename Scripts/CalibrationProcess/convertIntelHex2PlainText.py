import sys
import os

def hex_to_plain_text(hex_file, output_file):
    with open(hex_file, 'r') as infile, open(output_file, 'w') as outfile:
        for line in infile:
            # Ignorar líneas que no contengan datos
            if line.startswith(':'):
                # Obtener la longitud de los datos y la dirección
                length = int(line[1:3], 16)
                
                # Solo procesar registros de datos
                if int(line[7:9], 16) == 0:
                    # Extraer los datos
                    data = line[9:9 + length * 2]
                    # Convertir datos hex a formato plano
                    for i in range(0, len(data), 2):
                        byte = data[i:i + 2]
                        outfile.write(f"{byte} ")
                    #outfile.write("\n")  # Nueva línea después de cada línea de datos

def convert_files_in_directory(directory):
    for filename in os.listdir(directory):
        if filename.endswith('.hex'):
            hex_file_path = os.path.join(directory, filename)
            output_file_path = os.path.join(directory, filename[:-4] + '.txt')  # Cambiar .hex a .txt
            
            hex_to_plain_text(hex_file_path, output_file_path)
            print(f"Convertido: {hex_file_path} -> {output_file_path}")

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print("Uso: python convert_hex_to_plain.py directorio_con_archivos_hex")
        sys.exit(1)

    input_directory = sys.argv[1]

    if not os.path.isdir(input_directory):
        print(f"El directorio {input_directory} no existe o no es un directorio válido.")
        sys.exit(1)

    convert_files_in_directory(input_directory)
    print("Conversión completada para todos los archivos .hex en el directorio.")
