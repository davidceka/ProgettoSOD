const express = require('express');
const router = express.Router()
const session = require("../controllers/session");

router.get('/', (req, res) => { 
    res.render('home', {
        layout:'index'
    })
})
router.get('/about', (req, res) => { 
    res.render('about', {
        layout:'index'
    })
})


router.get('/sensordata', (req, res) => {
    const sensorData = session.getLatestSensorData(req);
    console.log(sensorData)
    if (sensorData) {
        res.render('stanze', {
            layout: 'index',
            sensorData:sensorData
        })
    } else {
      res.status(404).json({ error: 'Nessun dato disponibile per l\'ultimo sensore.' });
    }
  });

module.exports = router;