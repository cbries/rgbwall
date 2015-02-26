/* 
 * Copyright (C) 2015 Christian Benjamin Ries
 * Website: https://github.com/cbries/rgbwall
 * License: MIT
 */

/*
 * Partily based on a web tutorial:
 *  -> http://ahoj.io/nodejs-and-websocket-simple-chat-tutorial
 */

//
// Protocol:
// --------------------------------------------------------------
// { "type" : "single",
//   "data" : {
//       "x" : INT, "y" : INT,
//       "red" : INT, "green" : INT, "blue" : INT
// }}
//
// { "type" : "grid",
//   "data" : [
//  { "x":INT, "y":INT, "red":INT, "green":INT, "blue":INT },
//      { ... }
// ]}
// --------------------------------------------------------------

var WebSocketServer = require('websocket').server;
var http = require('http');
var sprintfjs = require("../src/sprintf.js"),
    sprintf = sprintfjs.sprintf,
    vsprintf = sprintfjs.vsprintf;
var webSocketsServerPort = 1337;

var connection = null;

var server = http.createServer(function(request, response) {
    // process HTTP request. Since we're writing just WebSockets server
    // we don't have to implement anything.
});
server.listen(webSocketsServerPort, function() { 
	console.log((new Date()) + " Server is listening on port " + webSocketsServerPort);
});

wsServer = new WebSocketServer({
	httpServer: server
});

wsServer.on('request', function(request) 
{
	console.log((new Date()) + ' Connection from origin ' + request.origin + '.');

    	var newConnection = request.accept(null, request.origin);
	if(connection != null) {
		newConnection.sendUTF(JSON.stringify({type: 'failure', message: 'Only one request is allowed.'}));
		newConnection.close();
		return ;
	}

	connection = newConnection;

    	connection.on('message', function(message) {
    		 try {
			var json = JSON.parse(message.data);
		} catch (e) {
			console.log('This doesn\'t look like a valid JSON: ', message.data);
			return ;
		} 

		if(json.type === 'single')
		{
			var osingle = json.data;
			var cmd = sprintf("%02d%02d%03d%03d%03d\n"
					, osingle.x
					, osingle.y
					, osingle.red
					, osingle.green
					, osingle.blue);
			console.log("RGB command: ", cmd);
		}
		else if(json.type === 'grid')
		{
			var ogrid = json.data;
			for(var i=0; i < ogrid.length; ++i)
			{
				var data = ogrid[i];
				if(data !== null && typeof(data) !== 'undefined')
				{
					var cmd = sprintf("%02d%02d%03d%03d%03d\n"
						, data.x
                                        	, data.y
                                        	, data.red
                                        	, data.green
                                        	, data.blue);

                        		console.log("RGB command: ", cmd);
				}
			}
		}
		else
		{
			// unknown
		}
	});

    	connection.on('close', function(connection) {
        	connection = null;
    	});
});

