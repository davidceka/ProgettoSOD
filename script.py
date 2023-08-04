import serial
import struct
import os

# Configura la porta seriale
serial_port = '/dev/ttyACM0' # Specifica la porta seriale corretta
baud_rate = 115200
ser = serial.Serial(serial_port, baud_rate, timeout=1)

# Legge i dati inviati dalla scheda Arduino
while True:
    # # Legge esattamente 18 byte
    # data_bytes = ser.read(18)

    # # Verifica che siano stati letti 18 byte
    # if len(data_bytes) == 18:
    #     os.system('clear')
    #     for byte in data_bytes:
    #         print(hex(byte))
        
    #     # Interpreta i dati come specificato nella struttura dataPacket in Arduino
    #     data = struct.unpack('>fffbBI', data_bytes)  # Aggiornato il formato per big-endian

    #     # Estrae i dati dalla tupla
    #     temperature, pressure, luminosity, ledState, peopleCount, timestamp = data
        data_string = ser.readline().decode().strip()
        if data_string:
            data_values = data_string.split(";")

        
            if len(data_values) == 6:
                temperature, pressure, luminosity, ledState, peopleCount, timestamp = data_values
                # Stampa i dati
                print(f'Temperature: {temperature}°C')
                print(f'Pressure: {pressure} hPa')
                print(f'Luminosity: {luminosity}')
                print(f'LED State: {"ON" if ledState else "OFF"}')
                print(f'People Count: {peopleCount}')
                print(f'Timestamp: {timestamp}')

        

# Chiude la connessione seriale
ser.close()
