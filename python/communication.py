import serial
import struct
import os
import paho.mqtt.client as mqtt
import datetime

# Configura la porta seriale
serial_port = '/dev/ttyACM0' # Specifica la porta seriale corretta
baud_rate = 115200
ser = serial.Serial(serial_port, baud_rate, timeout=1)

# Configura i parametri del broker MQTT
broker = "0.0.0.0"  # Inserisci l'indirizzo IP o il nome del tuo broker MQTT
port = 1883  # Inserisci la porta del tuo broker MQTT
topic_sensor = "sensor_data"  # Specifica il topic MQTT su cui pubblicare i dati
topic_user="user_data"

# Funzione di callback chiamata quando il client MQTT è connesso al broker
def on_connect(client, userdata, flags, rc,qos=1):
    print("Connected to MQTT broker with result code " + str(rc))

# Crea un client MQTT
client = mqtt.Client()
client.on_connect = on_connect

# Connetti il client al broker MQTT
client.connect(broker, port)

# Legge i dati inviati dalla scheda Arduino e li pubblica sul topic MQTT
while True:
    data_string = ser.readline().decode().strip()
    if data_string:
        data_values = data_string.split(";")

        if len(data_values) == 3:
            os.system('clear')
            readTag, timestamp, action = data_values
            dt_object = datetime.datetime.fromtimestamp(int(timestamp))
            anno = dt_object.year
            mese = dt_object.month
            giorno = dt_object.day
            ora = dt_object.hour
            minuti = dt_object.minute
            secondi = dt_object.second

            # Qui puoi fare ciò che desideri con i valori di readTag e timestamp
            # Per esempio, convertire il timestamp in data e ora come mostrato prima

            print(f"Read Tag: {readTag}")
            print(f"Anno: {anno}")
            print(f"Mese: {mese}")
            print(f"Giorno: {giorno}")
            print(f"Ora: {ora}")
            print(f"Minuti: {minuti}")
            print(f"Secondi: {secondi}")
            print(f'Action: {"Enter" if action=="1" else "Exit"}')
            print(f'Timestamp: {timestamp}')

            # Creazione del payload per l'invio MQTT
            payload = {
                "id_user": readTag,
                "anno": anno,
                "mese": mese,
                "giorno": giorno,
                "ora": ora,
                "minuto": minuti,
                "secondo": secondi,
                "action": "enter" if action=="1" else "exit",
                "timestamp":timestamp
            }
            

            # Invio del payload MQTT
            client.publish(topic_user, str(payload),qos=1)

        if len(data_values) == 6:
            os.system('clear')
            temperature, pressure, luminosity, ledState, peopleCount, timestamp = data_values

            # Qui puoi fare ciò che desideri con i dati ricevuti dall'Arduino

            # Pubblica i dati sul topic MQTT
            payload = {
                "Temperature": temperature,
                "Pressure": pressure,
                "Luminosity": luminosity,
                "LED State": "ON" if ledState=="1" else "OFF",
                "People Count": peopleCount,
                "Timestamp": timestamp
            }

            client.publish(topic_sensor, str(payload))

            # Stampa i dati
            print(f'Temperature: {temperature}°C')
            print(f'Pressure: {pressure} hPa')
            print(f'Luminosity: {luminosity} lx')
            print(f'LED State: {"ON" if ledState=="1" else "OFF"}')
            print(f'People Count: {peopleCount}')
            print(f'Timestamp: {timestamp}')

# Chiude la connessione seriale
ser.close()
