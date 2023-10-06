const express = require('express');
const router = express.Router()

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
    const userData = session.getPeopleInside(req);
    console.log(sensorData)
    if (sensorData) {
        res.render('stanze', {
            layout: 'index',
            sensorData:sensorData,
            user_data:userData
        })
    } else {
      res.status(404).json({ error: 'Nessun dato disponibile per l\'ultimo sensore.' });
    }
  });

module.exports = router;