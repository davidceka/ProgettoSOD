const mqtt = require('mqtt');
const path = require('path');
const express = require('express');
const app = express();
const sessions=require('express-session')
const crypto=require('crypto')
const mysql=require('mysql')



var secret = crypto.randomBytes(32).toString('hex');

app.use(sessions({
  secret: secret,
  saveUninitialized: true,
  resave: false
}));



const dotenv=require('dotenv').config({
  path:'./configs/.env'
})

const connection = mysql.createConnection({
  host: process.env.DB_HOST,
  user: process.env.DB_USER,
  password: process.env.DB_PASSWORD,
  database: process.env.DB_NAME,
});

const pagesRouter=require('./routes/pages')
const stanzeRouter=require('./routes/stanze')
const dbdataRouter=require('./routes/dbdata')



const handlebars = require('express-handlebars').create({
    layoutsDir: path.join(__dirname, "views/layouts"),
    partialsDir: path.join(__dirname, "views/partials"),
    defaultLayout: 'layout',
    extname: 'hbs'
  });




app.use(express.static(path.join(__dirname, 'views')));
app.engine('hbs', handlebars.engine)
app.set('view engine', 'hbs')
app.set('views', path.join(__dirname, "views"))

// Definisci le tue route qui
app.use('/',pagesRouter)
app.use('/stanze',stanzeRouter)
app.use('/dbdataRouter', dbdataRouter)

app.get('/api/sensordata', (req, res) => {
  // Esegui la query per ottenere l'ultimo dato del sensore dalla tabella sensor_data
  const query = `
    SELECT temperature, pressure, luminosity, ledstate, peoplecount, timestamp
    FROM sensor_data
    ORDER BY id DESC
    LIMIT 1
  `;

  connection.query(query, (error, results) => {
    if (error) {
      console.error('Errore durante la query:', error);
      res.status(500).json({ error: 'Errore durante il recupero dei dati del sensore' });
    } else {
      // Se la query ha avuto successo, invia i dati del sensore in formato JSON
      if (results.length > 0) {
        res.json(results[0]);
      } else {
        res.status(404).json({ message: 'Nessun dato del sensore trovato' });
      }
    }
  });
});


app.use(express.urlencoded({
    extended: true
  }));
app.use(express.json());



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

client.on('message', (topic, message) => {
  console.log(`Messaggio ricevuto sul topic '${topic}': ${message.toString()}`);
  
  try {
    const jsonMessage = message.toString().replace(/'/g, '"'); // Sostituisci gli apici singoli con doppi apici
    const data = JSON.parse(jsonMessage);
    
    // Ora 'data' sarà un oggetto JavaScript con le chiavi e i valori del messaggio
    // Possiamo utilizzare la funzione per inserire i dati nel database
    

    // Creiamo una query SQL di inserimento dei dati
    const query = `INSERT INTO sensor_data (temperature, pressure, luminosity, ledstate, peoplecount, timestamp) VALUES (?, ?, ?, ?, ?, ?)`;

    // Eseguiamo la query utilizzando i dati ricevuti dal messaggio MQTT
    connection.query(query, [data.Temperature, data.Pressure, data.Luminosity, data['LED State'], data['People Count'], data.Timestamp], (err, results) => {
      if (err) {
        console.error('Errore durante l\'inserimento dei dati nel database:', err);
      } else {
        console.log('Dati inseriti correttamente nel database.');
      }
      
      // Chiudi la connessione dopo aver inserito i dati
      connection.end();
    });
    
  } catch (error) {
    console.error('Errore durante il parsing del messaggio JSON:', error);
  }
});


// Gestione dell'evento di disconnessione
client.on('close', () => {
  console.log('Connessione MQTT chiusa');
});

// Gestione dell'errore
client.on('error', (err) => {
  console.error('Errore MQTT:', err);
});

const port = 3000;  // Porta su cui verrà avviato il server
app.listen(port, () => {
  console.log(`Server avviato su http://localhost:${port}`);
});
