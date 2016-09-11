var http = require('http');

function onRequest(request, response) {
	response.writeHead(200, {'Content-Type' : 'text/plain'});
	response.write('ScanYou demo web site');
	response.end();
}

http.createServer(onRequest).listen(8080);
console.log('server is running on locahost:8080');
