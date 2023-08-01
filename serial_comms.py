import serial

# Imposta la porta seriale e la velocità di comunicazione
port = "COM3"  # Inserisci la porta seriale corretta
baudrate = 9600

# Apre la connessione seriale
ser = serial.Serial(port, baudrate)

# Legge i dati in arrivo da Arduino
def read_data():
    while ser.inWaiting() > 0:
        data = ser.readline().decode().rstrip()
        print("Dato ricevuto da Arduino:", data)

# Esempio di utilizzo
while True:
    read_data()  # Leggi i dati in arrivo da Arduino

# Chiude la connessione seriale
ser.close()
