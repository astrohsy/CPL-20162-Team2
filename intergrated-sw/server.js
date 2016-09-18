var http 	= require('http'),
	fs   	= require('fs'),
	express = require('express'),
	path 	= require('path');

app = express();
app.use(express.static(path.join(__dirname, 'public')));
app.listen(8080);
console.log('server is running on port 8080');

app.get('/', function(req, res) {
	res.render('index.html');
});
