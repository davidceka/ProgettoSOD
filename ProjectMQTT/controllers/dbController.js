const mysql = require("mysql");
const session = require("./session");
const moment = require('moment');

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


  
  // Altre importazioni necessarie...
  
  exports.getLatestSensorData = async (req, res) => {
    // Esegui la query per ottenere i dati più recenti dal database
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
  
        // Esegui la query per ottenere i dati relativi alle persone presenti nella stanza
        executeQuery(
          "SELECT user_tag, timestamp " +
          "FROM user_data AS enter_data " +
          "WHERE action = 'enter' " +
          "AND NOT EXISTS ( " +
          "  SELECT 1 " +
          "  FROM user_data AS exit_data " +
          "  WHERE exit_data.action = 'exit' " +
          "  AND exit_data.user_tag = enter_data.user_tag " +
          "  AND exit_data.timestamp > enter_data.timestamp " +
          ") " +
          "GROUP BY user_tag " +
          "ORDER BY enter_data.timestamp DESC",
          [],
          async function (error, results) {
            if (error) throw error;
  
            var peopleInside = [];
            results.forEach((row) => {
              const timestamp = row.timestamp;
              const formattedTimestamp = moment.unix(timestamp).format("DD/MM/YYYY HH:mm:ss");
              peopleInside.push({
                user_tag: row.user_tag,
                timestamp: formattedTimestamp
              });
            });
            console.log(peopleInside)
            // Ora puoi fare qualsiasi cosa con i dati latestSensorData e peopleInside
            // Per esempio, salvare i dati nella sessione
            session.setLatestSensorData(req,latestSensorData)
            session.setPeopleInside(req,peopleInside)
  
            res.redirect("/sensordata"); // Cambia "/dashboard" con la tua route di destinazione
          }
        );
      }
    );
  };
  