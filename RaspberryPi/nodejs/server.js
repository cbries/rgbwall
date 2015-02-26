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
var sprintfjs = require("sprintf"),
    sprintf = sprintfjs.sprintf,
    vsprintf = sprintfjs.vsprintf;
var exec = require('child_process').exec;
var webSocketsServerPort = 1337;

var connections = [ ];

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
	connections.push(newConnection);

    	newConnection.on('message', function(message) {
    		console.log(message); 
		try {
			var json = JSON.parse(message.utf8Data);
		} catch (e) {
			console.log('This doesn\'t look like a valid JSON: ', message.utf8Data);
			return ;
		} 

		if(json.type === 'single')
		{
			var osingle = json.data;
			var cmd = sprintf("../transmitrgb /dev/ttyACM0 -x %d -y %d -r %d -g %d -b %d -ts 0 -tms 500"
					, parseInt(osingle.x)
					, parseInt(osingle.y)
					, parseInt(osingle.red)
					, parseInt(osingle.green)
					, parseInt(osingle.blue));
					
			var child = exec(cmd,
				function (error, stdout, stderr) {
					console.log(' Exec::Sdout: ' + stdout);
    					console.log(' Exec::Stderr: ' + stderr);
    					if (error !== null) {
      						console.log(' Exec failure: ' + error);
					}
				}
			);
		}
		else if(json.type === 'grid')
		{
			var ogrid = json.data;
			for(var i=0; i < ogrid.length; ++i)
			{
				var data = ogrid[i];
				if(data !== null && typeof(data) !== 'undefined')
				{
					var cmd = sprintf("../transmitrgb /dev/ttyACM0 -x %d -y %d -r %d -g %d -b %d -ts 0 -tms 500"
						, parseInt(data.x)
                                        	, parseInt(data.y)
                                        	, parseInt(data.red)
                                        	, parseInt(data.green)
                                        	, parseInt(data.blue));

					var child = exec(cmd,
                                		function (error, stdout, stderr) {
                                        		console.log(' Exec::Sdout: ' + stdout);
                                        		console.log(' Exec::Stderr: ' + stderr);
                                        		if (error !== null) {
                                        		        console.log(' Exec failure: ' + error);
                                       			}
                                		}
                        		);
				}
			}
		}
		else
		{
			// unknown
		}
	});

    	newConnection.on('close', function(connection) {
    		var p = this;
		for(var i=0; i < connections.length; ++i)
		{
			if(connections[i] == p)
			{
				connections.splice(i, 1);	
			}
		}
	});
});

