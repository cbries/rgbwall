//
// Copyright (C) 2015 Christian Benjamin Ries
// License: MIT
//
var objTable;
var maxBrightness = 255;
var gridWidth = 20;
var gridHeight = 10;
var tableWHdelta = 40;
var tableWidth = gridWidth * tableWHdelta;
var tableHeight = gridHeight * tableWHdelta;

var wsTargetAddress = "ws://192.168.137.200:1337";
var ws = null;

function to2digit(v) {
	v = v.toString();
	if(v.length == 1) { return "0" + v; }
	return v;
}

function randomColorPart()
{
	return Math.floor((Math.random() * parseInt(mb())) + 0);
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
			content += '<td id="' + tdid + '" width="'+tableWHdelta+'" height="'+tableWHdelta+'">';
			content += '<div id="colorPicker_'+x+'_'+y+'" class="colorPicker">';
			content +=   '<a class="color"><div class="colorInner"></div></a>';
			content +=   '<div class="track"></div>';
		  content +=   '<ul class="dropdown"><li></li></ul>';
			content +=   '<input type="hidden" class="colorInput"/>';
			content += '</div>';
			content += '</td>';
		}
		content += '</tr>';
	}
	content += '</table>'
	objTable.append(content);
	
	// init color picker
	for(var y=0; y < gridHeight; ++y)
	{
		for(var x=0; x < gridWidth; ++x)
		{
			var id = '#colorPicker_' + x + '_' + y;
			$(id).tinycolorpicker();
			var $box = $(id);
						
			$box.on("change", function() {
				var rgb = $(this).data("plugin_tinycolorpicker").colorRGB;
				rgb = rgb.replace("rgb(", "");
				rgb = rgb.replace(")", "");
				var parts = rgb.split(",");
				var red = parseInt(parts[0]);
				var green = parseInt(parts[1]);
				var blue = parseInt(parts[2]);
				
				var senderId = $(this).attr('id');
				senderId = senderId.replace("colorPicker_", "");
				var xy = senderId.split("_");
								
				sendColor(parseInt(xy[0]), parseInt(xy[1]), red, green, blue);
			});
		}
	}
}

function wsOnOpen() {
	console.log("Connection established!");
	checkUi();
}
function wsOnError(er) {
	console.log('WebSocket Error ' + er);
}
function wsOnMessage(ev) {
	console.log('Server: ' + ev.data);
}
function wsOnClose(ev) {
	checkUi();
}

function wsIsOpen() {
	return (ws != null && ws.readyState == 1);
}

function checkUi()
{
	if(wsIsOpen()) {
		$('#gridPart').show();
		$('.connect').hide();
		$('.disconnect').show();
	} else {
		$('#gridPart').hide();
		$('.connect').show();
		$('.disconnect').hide();
	}
}

function initializeUi()
{
	mb();
	objTable = $('#grid20x10');
	createGridTable();
}

function mb() 
{
	maxBrightness = $('#lineMaxBrightness').val();
	return maxBrightness;
}

$( document ).ready(function() 
{
	console.log('Ready!');

	initializeUi();	
	checkUi();
	
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
	
	$('#cmdRandom').click(function(ev) {
		for(var x=0; x < gridWidth; ++x)
		{
      for(var y=0; y < gridHeight; ++y)
		  {
				var step = 255.0 / parseInt(mb());
				var red   = step * parseInt(randomColorPart());
				var green = step * parseInt(randomColorPart());
				var blue  = step * parseInt(randomColorPart());
			  				
				var id = '#colorPicker_' + x + '_' + y;
				var $box = $(id);				

				$box.data("plugin_tinycolorpicker").setColor("rgb("+red+","+green+","+blue+")");				
				sendColor(x, y, red, green, blue);
			}
		}			
	});
	
	$('#cmdConnect').click(function(ev) {
		if(ws != null) {
			alert('There is already a WebSocket connection established!');
		} else {
			wsTargetAddress = $('#lineAddress').val();
			console.debug('Trying to connect to: ' + wsTargetAddress);
			ws = new WebSocket(wsTargetAddress);
			ws.onopen = wsOnOpen;
			ws.onerror = wsOnError;
			ws.onmessage = wsOnMessage;
			ws.onclose = wsOnClose;
		}
	});
	
	$('#cmdDisconnect').click(function(ev) {
		if(ws == null) {
			checkUi();
		} else {
			ws.close();
		}
	});
});