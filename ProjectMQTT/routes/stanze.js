const express = require('express');
const router = express.Router()


router.get('/cucina', (req, res) => { 
    res.render('stanze', {
        layout: 'index',
        cucina:true,
        salotto:false
    })
})

router.get('/salotto', (req, res) => { 
    res.render('stanze', {
        layout: 'index',
        cucina:false,
        salotto:true
    })
})



module.exports = router;
