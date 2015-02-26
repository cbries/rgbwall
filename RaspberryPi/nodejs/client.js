//
// Copyright (C) 2015 Christian Benjamin Ries
// License: MIT
//
var objTable;
var gridWidth = 20;
var gridHeight = 10;
var tableWHdelta = 40;
var tableWidth = gridWidth * tableWHdelta;
var tableHeight = gridHeight * tableWHdelta;

var wsTargetAddress = "ws://192.168.137.200:1337";
var ws = null;

function randomColorPart()
{
	return Math.floor((Math.random() * 13) + 0);
}

function sendColor(x, y, red, green, blue)
{
	if(wsIsOpen())
	{
		var json = JSON.stringify({
			 "type" : "single",
				"data" : {
					"x": x, "y": y,
					"red": red,
					"green": green,
					"blue": blue
				}
			});
		ws.send(json);
	}
}

function createGridTable() 
{
	var ids = [ ];	
	var content = '<table class="grid" width="'+tableWidth+'" height="'+tableHeight+'">'
	for(var y=0; y < gridHeight; ++y)
	{
		content += '<tr>';
		for(var x=0; x < gridWidth; ++x)
		{
			var tdid = 'td_' + x + '_' + y;
			ids.push(tdid);
			content += '<td id="' + tdid + '" width="'+tableWHdelta+'" height="'+tableWHdelta+'">&nbsp;</td>';
		}
		content += '</tr>';
	}
	content += '</table>'
	objTable.append(content);
	
	for(var i=0; i < ids.length; ++i) 
	{
		var id = ids[i];
		$('#' + id).click(function(ev)
		{
			var parts = ev.target.id.split("_");
			var x = parts[1];
			var y = parts[2];
			
			var red = randomColorPart();
			var green = randomColorPart();
			var blue = randomColorPart();
			
			var rgb = "#" + parseInt(red, 16) + parseInt(green, 16) + parseInt(blue, 16);		
			for(var ii=0; ii < 10; ++ii)
			{
				$('#' + ev.target.id).css('background-color', rgb);
				$('#' + ev.target.id).css('backgroundColor', rgb);
			}
			console.debug("Background color changed of: " + ev.target.id);
			sendColor(x, y, red, green, blue);
		});
	}
}

function wsOnOpen() {
	console.log("Connection established!");
	$('#gridPart').show();
}
function wsOnError(er) {
	console.log('WebSocket Error ' + er);
}
function wsOnMessage(ev) {
	console.log('Server: ' + ev.data);
}
function wsOnClose(ev) {
	$('#gridPart').hide();
}

function wsIsOpen() {
	return (ws != null && ws.readyState == 1);
}

$( document ).ready(function() {
	console.log('Ready!');
	$('#gridPart').hide();
	objTable = $('#grid20x10');
	createGridTable();
	
	$('#cmdReset').click(function(ev) {
		if(ws == null) {
			alert('There is no WebSocket connection!');
		} else {
			
			var cmd = { "type" : "grid", "data": [] };			
			for(var x=0; x < gridWidth; ++x) {
				for(var y=0; y < gridHeight; ++y) {
					var n = cmd["data"].length;
					cmd["data"][n] = { "x" : x, "y" : y, "red" : "0", "green" : "0", "blue" : "0" };
					$('#td_' + x + '_' + y).css('background-color', '#ffffff');
				}
			}
			
			ws.send(JSON.stringify(cmd));
		}
	});
	
	$('#cmdConnect').click(function(ev) {
		if(ws != null) {
			alert('There is already a WebSocket connection established!');
		} else {
			ws = new WebSocket(wsTargetAddress);
			ws.onopen = wsOnOpen;
			ws.onerror = wsOnError;
			ws.onmessage = wsOnMessage;
			ws.onclose = wsOnClose;
		}
	});
});