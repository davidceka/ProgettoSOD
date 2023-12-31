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

1. Assicurati che la Raspberry Pi sia aggiornata eseguendo i seguenti comandi: <br>
```sudo apt update&&sudo apt upgrade```
`
2. Installa Mosquitto <br>
- ```sudo apt install mosquitto``` <br>
- ```sudo apt install mosquitto-clients```

3. Configura il broker MQTT Mosquitto sulla Raspberry Pi. Modifica il file di configurazione 
`/etc/mosquitto/mosquitto.conf` 
con il seguente contenuto:<br>
- `listener 1883 0.0.0.0`<br>
- `allow_anonymous true`<br>

4. Avvia il servizio di mosquitto e verificane il corretto caricamento. <br>
```sudo systemctl start mosquitto``` <br>
```sudo systemctl enable mosquitto``` <br>

5. Installa il servizio di comunicazione `communication.service` fornito dalla cartella `servizi_demoni` sul tuo sistema Linux e avvialo usando il tuo service manager (ad esempio, SYSTEMCTL).

6. Assicurati che il servizio sia attivo. <br>
```sudo systemctl status communication```

7. In alternativa, avvia manualmente il processo python `python comm.py`

## Configurazione del Client

1. Nella macchina che fungerà da client, spostati nella cartella chiamata `ProjectMQTT` e scarica le dipendenze NPM con il comando:
```npm install```
Prima di eseguire questo comando, assicurati che Node.js sia installato sulla macchina client. Ecco come puoi installare Node.js: <br>
```sudo pacman -S nodejs```<br>
```sudo pacman -S npm```


2. Utilizzando un software per la gestione di database relazionali (ad esempio MySQL), importa il dump `backup.sql` del database situato nella repository. 
Riportiamo la procedura di import per MySQL Workbench al link https://dev.mysql.com/doc/workbench/en/wb-admin-export-import-management.html
Per l'installazione di mysql, riportiamo quella per sistemi ArchLinux <br>
```sudo pacman -S mariadb```<br>
```sudo systemctl start mariadb```

3. Esegui il comando `npm start` per avviare il backend e l'applicazione frontend per la consultazione dei dati.

4. Recarsi in http://localhost:3000 (se non si è cambiato il codice, il sito sarà localizzato qui).

