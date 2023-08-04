const mysql = require("mysql");
const session = require("./session");

function defineConn() {
    var conn = mysql.createConnection({
      host: process.env.DB_HOST,
      user: process.env.DB_USER,
      password: process.env.DB_PASSWORD,
      database: process.env.DB_NAME,
    });
    return conn;
  }

var conn = defineConn();

async function executeQuery(query, params, callback) {
    conn.query(query, params, callback);
  }


  exports.getLatestSensorData = async (req, res) => {
    executeQuery(
      "SELECT temperature, pressure, luminosity, ledstate, peoplecount, timestamp FROM sensor_data ORDER BY id DESC LIMIT 1",
      [],
      async function (error, results) {
        if (error) throw error;
        
  
        var latestSensorData = {
          temperature: results[0].temperature,
          pressure: results[0].pressure,
          luminosity: results[0].luminosity,
          ledstate: results[0].ledstate,
          peoplecount: results[0].peoplecount,
          timestamp: results[0].timestamp
        };
        
        // Ora puoi fare qualsiasi cosa con l'oggetto latestSensorData
        // Per esempio, salvare i dati nella sessione o passarli a una vista
        session.setLatestSensorData(req, latestSensorData);
        
        res.redirect("/sensordata"); // Cambia "/dashboard" con la tua route di destinazione
      }
    );
  };