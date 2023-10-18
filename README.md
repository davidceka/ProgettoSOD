# Progetto SOD

Questo progetto è un sistema IoT che utilizza Raspberry Pi e Arduino per monitorare dati ambientali, comunicare tramite MQTT e visualizzare i dati attraverso un'applicazione web. Segui le istruzioni di seguito per configurare e avviare il sistema.

## Materiale Occorrente

- Raspberry Pi
- Arduino Uno
- Shield NFC PN532
- Sensore di temperatura/pressione BMP280
- Sensore di luminosità BH1750
- Modulo RTC

## Collegamenti Hardware
- Consulta il PDF generale per il pinout

## Setup Arduino

1. Utilizza l'Arduino IDE o un tool simile per caricare il firmware (sketch_rtos_2task.ino) sul tuo Arduino Uno.

2. Collega l'Arduino Uno al Raspberry Pi tramite la porta USB.

## Setup Raspberry Pi

1. Assicurati che la Raspberry Pi sia aggiornata eseguendo i seguenti comandi:
```sudo apt update&&sudo apt upgrade```

2. Installa e configura il broker MQTT Mosquitto sulla Raspberry Pi. Modifica il file di configurazione 
`/etc/mosquitto/mosquitto.conf` 
con il seguente contenuto:
`listener 1883 0.0.0.0`
`allow_anonymous true`

3. Installa il servizio di comunicazione fornito dalla cartella `servizi_demoni` sul tuo sistema Linux e avvialo usando il tuo service manager (ad esempio, SYSTEMCTL).

4. Assicurati che il servizio sia attivo.

## Configurazione del Client

2. Nella macchina che fungerà da client, spostati nella cartella chiamata `ProjectMQTT` e scarica le dipendenze NPM con il comando:
```npm install```

3. Utilizzando un software per la gestione di database relazionali (ad esempio MySQL), importa il dump del database situato nella repository. Il file si chiama `backup.sql`.

4. Esegui il comando `npm start` per avviare il backend e l'applicazione frontend per la consultazione dei dati.

5. Recarsi in http://localhost:3000 (se non si è cambiato il codice, il sito sarà localizzato qui).

