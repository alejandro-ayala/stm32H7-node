import can

def read_can_messages(channel, output_file):
    """Lee mensajes CAN desde el canal especificado y los almacena en un archivo."""
    # Configurar la conexión con la PCAN-USB a 125 kbit/s usando 'interface'
    bus = can.interface.Bus(interface='pcan', channel=channel, bitrate=125000)

    print(f"Listening for CAN messages on {channel}...")

    # Abrir el archivo en modo append
    with open(output_file, 'a') as file:
        try:
            while True:
                message = bus.recv(timeout=1)  # Leer mensajes con timeout de 1 segundo
                if message:
                    # Convertir los datos del mensaje a una cadena de bytes hexadecimales separados por espacios
                    data_hex = ' '.join(f"{byte:02x}" for byte in message.data)
                    # Imprimir mensaje en la consola
                    print(f"Message received: ID: {message.arbitration_id}, Data: {data_hex}, DLC: {message.dlc}", flush=True)
                    # Almacenar en el archivo
                    #file.write(f"Message received: ID: {message.arbitration_id}, Data: {data_hex}, DLC: {message.dlc}\n")
                    file.write(f"{data_hex}\n")

                # else:
                #     print("No message received...", flush=True)
        except KeyboardInterrupt:
            print("Stopped listening for CAN messages.")
        finally:
            bus.shutdown()  # Cerrar la conexión al bus CAN

if __name__ == "__main__":
    # Aquí puedes especificar el canal, como 'PCAN_USBBUS1'
    channel = 'PCAN_USBBUS1'  # Asegúrate de cambiar esto si tu bus es diferente
    output_file = 'can_messages.txt'  # Nombre del archivo donde almacenar los mensajes
    read_can_messages(channel, output_file)
