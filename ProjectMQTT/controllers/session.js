
function setLatestSensorData(req, latestSensorData) {
    req.session.latestSensorData = latestSensorData;
  }

function getLatestSensorData(req) {
    return req.session.latestSensorData || null;
}

module.exports={
    getLatestSensorData: getLatestSensorData,
    setLatestSensorData: setLatestSensorData
}