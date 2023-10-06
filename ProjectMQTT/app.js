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

app.get('/api/sensordata', (req, res) => {
  // Esegui la query per ottenere l'ultimo dato del sensore dalla tabella sensor_data
  const querySensorData = `
    SELECT temperature, pressure, luminosity, ledstate, peoplecount, timestamp
    FROM sensor_data
    ORDER BY id DESC
    LIMIT 1
  `;

  // Esegui la query per ottenere i dati degli utenti presenti nella stanza
  const queryUsersInside = `
    SELECT user_tag, timestamp 
    FROM user_data AS enter_data 
    WHERE action = 'enter' 
    AND NOT EXISTS ( 
      SELECT 1 
      FROM user_data AS exit_data 
      WHERE exit_data.action = 'exit' 
      AND exit_data.user_tag = enter_data.user_tag 
      AND exit_data.timestamp > enter_data.timestamp 
    ) 
    GROUP BY user_tag 
    ORDER BY enter_data.timestamp DESC
  `;

  connection.query(querySensorData, (errorSensorData, resultsSensorData) => {
    if (errorSensorData) {
      console.error('Errore durante la query del sensore:', errorSensorData);
      res.status(500).json({ error: 'Errore durante il recupero dei dati del sensore' });
    } else {
      // Se la query del sensore ha avuto successo, esegui la query per gli utenti
      connection.query(queryUsersInside, (errorUsersInside, resultsUsersInside) => {
        if (errorUsersInside) {
          console.error('Errore durante la query degli utenti:', errorUsersInside);
          res.status(500).json({ error: 'Errore durante il recupero dei dati degli utenti' });
        } else {
          // Se la query degli utenti ha avuto successo, invia i dati in formato JSON
          const sensorData = resultsSensorData.length > 0 ? resultsSensorData[0] : null;
          const usersInside = resultsUsersInside;
          res.json({ sensorData, usersInside });
        }
      });
    }
  });
});



app.use(express.urlencoded({
    extended: true
  }));
app.use(express.json());



// Configura i dettagli di connessione al broker MQTT
const brokerUrl = 'mqtt://192.168.1.47:1883';  // Indirizzo del broker MQTT
const topic_sensor = 'sensor_data';  // Il topic a cui sottoscriversi
const topic_user = 'user_data';  // Il topic a cui sottoscriversi


// Crea un client MQTT
const client = mqtt.connect(brokerUrl);

// Gestione degli eventi di connessione
client.on('connect', () => {
  console.log('Connesso al broker MQTT');

  // Sottoscrizione al topic "sensor_data"
  client.subscribe(topic_sensor, (err) => {
    if (err) {
      console.error('Errore durante la sottoscrizione al topic "sensor_data"', err);
    } else {
      console.log('Sottoscrizione al topic "sensor_data" avvenuta con successo');
    }
  });

  // Sottoscrizione al nuovo topic "user_data"
  client.subscribe(topic_user, (err) => {
    if (err) {
      console.error('Errore durante la sottoscrizione al topic "user_data"', err);
    } else {
      console.log('Sottoscrizione al topic "user_data" avvenuta con successo');
    }
  });
});


client.on('message', (topic, message) => {
  console.log(`Messaggio ricevuto sul topic '${topic}': ${message.toString()}`);
  
  try {
    const jsonMessage = message.toString().replace(/'/g, '"'); // Sostituisci gli apici singoli con doppi apici
    const data = JSON.parse(jsonMessage);
    
    if (topic === topic_sensor) {
      // Dato ricevuto dal topic "sensor_data"
      // Inserisci i dati nel database
      const query = `INSERT INTO sensor_data (temperature, pressure, luminosity, ledstate, peoplecount, timestamp) VALUES (?, ?, ?, ?, ?, ?)`;
      connection.query(query, [data.Temperature, data.Pressure, data.Luminosity, data['LED State'], data['People Count'], data.Timestamp], (err, results) => {
        if (err) {
          console.error('Errore durante l\'inserimento dei dati nel database:', err);
        } else {
          console.log('Dati sensori inseriti correttamente nel database.');
        }
      });
    } else if (topic === topic_user) {
      const query = `INSERT INTO user_data (user_tag, giorno, mese, anno, ora, minuto, secondo, action, timestamp) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)`;
      connection.query(query, [data.id_user, data.giorno, data.mese, data.anno, data.ora, data.minuto, data.secondo,data.action,data.timestamp], (err, results) => {
        if (err) {
          console.error('Errore durante l\'inserimento dei dati dell\'utente nel database:', err);
        } else {
          console.log('Dati dell\'utente inseriti correttamente nel database.');
        }
      });

    } else {
      // Nessuna azione definita per altri topic
      console.log('Messaggio ricevuto da un topic non gestito:', topic);
    }
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
