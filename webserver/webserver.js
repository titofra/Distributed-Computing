/*
    @brief Basic webserver
    @author Titouan ABADIE - http://github.com/titofra - 04/23
*/

const port = 1818;
const root_path = './webserver_content';

const fs = require('fs');
const http = require('http');

var server = http.createServer(function (req, res) {   //create web server
    if (req.url == '/') {
        fs.readFile(root_path + '/index', {encoding: 'utf-8'}, (err, data) => {
            
            if (err) {
                console.log ('Invalid Request: ' + err);
                res.end ('Invalid Request: ' + err);
            } else {
                res.writeHead(200, { 'Content-Type': 'text/html; charset=utf-8'});
                res.write(data);
                res.end();
            }
        });
    } else {
        fs.readFile(root_path + req.url, {encoding: 'utf-8'}, (err, data) => {
            
            if (err) {
                console.log ('Invalid Request: ' + err);
                res.end ('Invalid Request: ' + err);
            } else {
                res.writeHead(200, { 'Content-Type': 'text/html; charset=utf-8'}); 
                res.write(data);
                res.end();
            }
        });
    }
});

server.listen(port);
console.log('WebServer running port ' + port + ' ...')