const express = require("express");
const dbController = require("../controllers/dbController");


const router = express.Router();

router.get('/getsensordata',dbController.getLatestSensorData)

module.exports = router;