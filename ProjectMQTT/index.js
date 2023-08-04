const mqtt = require('mqtt');

// Configura i dettagli di connessione al broker MQTT
const brokerUrl = 'mqtt://192.168.1.47:1883';  // Indirizzo del broker MQTT
const topic = 'sensor_data';  // Il topic a cui sottoscriversi

// Crea un client MQTT
const client = mqtt.connect(brokerUrl);

// Gestione degli eventi di connessione
client.on('connect', () => {
  console.log('Connesso al broker MQTT');

  // Sottoscrizione al topic
  client.subscribe(topic, (err) => {
    if (err) {
      console.error('Errore durante la sottoscrizione al topic', err);
    } else {
      console.log('Sottoscrizione al topic avvenuta con successo');
    }
  });
});

// Gestione degli eventi di ricezione dei messaggi
client.on('message', (topic, message) => {
  console.log(`Messaggio ricevuto sul topic '${topic}': ${message.toString()}`);
  // Puoi elaborare il messaggio qui o aggiornare il tuo sito web con i dati ricevuti
});

// Gestione dell'evento di disconnessione
client.on('close', () => {
  console.log('Connessione MQTT chiusa');
});

// Gestione dell'errore
client.on('error', (err) => {
  console.error('Errore MQTT:', err);
});
