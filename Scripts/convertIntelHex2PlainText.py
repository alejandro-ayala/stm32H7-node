import sys

def hex_to_plain_text(hex_file, output_file):
    with open(hex_file, 'r') as infile, open(output_file, 'w') as outfile:
        for line in infile:
            # Ignorar líneas que no contengan datos
            if line.startswith(':'):
                # Obtener la longitud de los datos y la dirección
                length = int(line[1:3], 16)
                # type = int(line[7:9], 16)  # No necesitamos el tipo aquí

                # Solo procesar registros de datos
                if int(line[7:9], 16) == 0:
                    # Extraer los datos
                    data = line[9:9 + length * 2]
                    # Convertir datos hex a formato plano
                    for i in range(0, len(data), 2):
                        byte = data[i:i + 2]
                        outfile.write(f"{byte} ")
                    #outfile.write("\n")  # Nueva línea después de cada línea de datos

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print("Uso: python convert_hex_to_plain.py archivo_hex archivo_salida")
        sys.exit(1)

    input_hex_file = sys.argv[1]
    output_plain_file = sys.argv[2]

    try:
        hex_to_plain_text(input_hex_file, output_plain_file)
        print(f"Conversión completada. Salida en: {output_plain_file}")
    except FileNotFoundError:
        print(f"El archivo {input_hex_file} no existe.")
        sys.exit(1)
