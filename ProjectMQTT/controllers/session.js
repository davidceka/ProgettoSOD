
function setLatestSensorData(req, latestSensorData) {
    req.session.latestSensorData = latestSensorData;
  }

function getLatestSensorData(req) {
    return req.session.latestSensorData || null;
}

function setPeopleInside(req, peopleInside) {
    req.session.peopleInside = peopleInside;
  }
  
function getPeopleInside(req) {
    return req.session.peopleInside || null;
}
  
  
  
  

  module.exports = {
    getLatestSensorData: getLatestSensorData,
    setLatestSensorData: setLatestSensorData,
    getPeopleInside: getPeopleInside,
    setPeopleInside: setPeopleInside
  };
  