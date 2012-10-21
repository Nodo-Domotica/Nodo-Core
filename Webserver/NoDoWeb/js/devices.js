var DeviceTimer; 
var DeviceIndex;

var ArrDeviceID=new Array();
var ArrDeviceName=new Array();
var ArrLabelOn=new Array();
var ArrLabelOff=new Array();

$('#devices_page').on('pageinit', function(event) {
    checkSession();
	getDevices();
});


$('#devices_page').on('pageshow', function(event) {
	
	$("div.ui-input-search").hide(); //zoekveld verbergen
	
	pagetitle = 'Devices';
	
	
	//Device status loop opstarten indien de pagina word weergegeven
	checkSession();
	Device_State();
    DeviceTimer=setInterval(function() {Device_State()},5000);
	$('#header_devices').append('<div id="nodostate">'+pagetitle+'</div>');
	Nodo_State();
});


$('#devices_page').on('pagehide', function(event) {
	//Device status loop stoppen indien een andere pagina word geopend
    clearInterval(DeviceTimer);
	$('#header_devices').empty();
});


$( "#device_popup" ).bind({
   popupbeforeposition: function(event, ui) { 
   
   $('#divdevice').empty();
   $('#divdevice_header').empty();
   
   if (varType == 1) { //aan uit
        $('#divdevice').append('<li data-theme="'+themeheader+'" data-role="divider">' + ArrDeviceName[DeviceIndex] + '</li>' +
		'<li data-icon="check"><a href="javascript:send_event(&quot;&action=on&device_id=' + ArrDeviceID[DeviceIndex]  + '&quot;,&quot;device&quot;)" data-ajax="false">'+ ArrLabelOn[DeviceIndex] +'</a></li>' +
	    '<li data-icon="delete"><a href="javascript:send_event(&quot;&action=off&device_id=' + ArrDeviceID[DeviceIndex]  + '&quot;,&quot;device&quot;)" data-ajax="false">'+ ArrLabelOff[DeviceIndex] +'</a></li>');
    }

    if (varType == 2) { //dim buttons
        $('#divdevice').append('<li data-theme="'+themeheader+'" data-role="divider">Dim: ' + ArrDeviceName[DeviceIndex] + '</li>' +
        '<li data-icon="false"><a href="javascript:send_event(&quot;&action=dim&device_id=' + ArrDeviceID[DeviceIndex]  + '&dim=2&quot;,&quot;device&quot;)" data-ajax="false">10%</a></li>' +
        '<li data-icon="false"><a href="javascript:send_event(&quot;&action=dim&device_id=' + ArrDeviceID[DeviceIndex]  + '&dim=3&quot;,&quot;device&quot;)" data-ajax="false">20%</a></li>' +
        '<li data-icon="false"><a href="javascript:send_event(&quot;&action=dim&device_id=' + ArrDeviceID[DeviceIndex]  + '&dim=5&quot;,&quot;device&quot;)" data-ajax="false">30%</a></li>' +
        '<li data-icon="false"><a href="javascript:send_event(&quot;&action=dim&device_id=' + ArrDeviceID[DeviceIndex]  + '&dim=6&quot;,&quot;device&quot;)" data-ajax="false">40%</a></li>' +
        '<li data-icon="false"><a href="javascript:send_event(&quot;&action=dim&device_id=' + ArrDeviceID[DeviceIndex]  + '&dim=8&quot;,&quot;device&quot;)" data-ajax="false">50%</a></li>' +
        '<li data-icon="false"><a href="javascript:send_event(&quot;&action=dim&device_id=' + ArrDeviceID[DeviceIndex]  + '&dim=10&quot;,&quot;device&quot;)" data-ajax="false">60%</a></li>' +
        '<li data-icon="false"><a href="javascript:send_event(&quot;&action=dim&device_id=' + ArrDeviceID[DeviceIndex]  + '&dim=11&quot;,&quot;device&quot;)" data-ajax="false">70%</a></li>' +
        '<li data-icon="false"><a href="javascript:send_event(&quot;&action=dim&device_id=' + ArrDeviceID[DeviceIndex]  + '&dim=13&quot;,&quot;device&quot;)" data-ajax="false">80%</a></li>' +
        '<li data-icon="false"><a href="javascript:send_event(&quot;&action=dim&device_id=' + ArrDeviceID[DeviceIndex]  + '&dim=14&quot;,&quot;device&quot;)" data-ajax="false">90%</a></li>' +
        '<li data-icon="false"><a href="javascript:send_event(&quot;&action=dim&device_id=' + ArrDeviceID[DeviceIndex]  + '&dim=16&quot;,&quot;device&quot;)" data-ajax="false">100%</a></li>');
		
	}
	
	if (varType == 3) { //dim slider
         $('#divdevice').append('<li data-theme="'+themeheader+'" class="ui-li-static ui-corner-top" data-role="divider">Dim: ' + ArrDeviceName[DeviceIndex] + '</li>' +
        '<div style="min-width:275px; padding:20px 20px;">'+
		'<input  name="distSlider" id="distSlider'+ ArrDeviceID[DeviceIndex] +'" value="' + ArrayDeviceDimvalue[ArrDeviceID[DeviceIndex]] + '" min="1" max="16" step="1" data-type="range" onChange="update_distance_device_timer('+ ArrDeviceID[DeviceIndex] +')"></div>').trigger("create");
	}
    
	$('#divdevice').listview('refresh');
	}
	
});
	

$( "#device_popup" ).bind({
   popupafterclose: function(event, ui) {  
  
	$('#divdevice').empty();
    $('#divdevice_header').empty();

	}
});


function OpenDevicePopup(index,type) {
	DeviceIndex = index;
	varType = type;
   	$( "#device_popup" ).popup("open");
};


//Device slider functie
function update_distance_device_timer(index) {
	SliderIndex = index;
	clearTimeout(SliderTimer);
	SliderTimer=setTimeout("update_distance_device(SliderIndex)",200);
}
function update_distance_device(index) {
	var val = $('#distSlider'+index).val();
	send_event('&action=dim&device_id=' + ArrDeviceID[DeviceIndex]  + '&dim='+ val,'device')
}

function getDevices() {
	
    $.getJSON('webservice/json_devices.php', function(data) {
        
		devices = data.devices;
		
        $('#devices li').remove();
        $.each(devices, function(index, device) {

            ArrDeviceID[index]=device.id;
            ArrDeviceName[index]=device.naam;
			if (device.labelon != '') {ArrLabelOn[index]=device.labelon;} else {ArrLabelOn[index]='On';}
			if (device.labeloff != '') {ArrLabelOff[index]=device.labeloff;} else {ArrLabelOff[index]='Off';}
			
          switch (device.type) {

            case "1": //KAKU
            case "3": //WiredOut
            case "4": //SendUserEvents
            case "5": //UserEvents
                			
				//Aan uit
				if (device.toggle == 0) {
					$('#devices').append('<li data-icon="arrow-u"><a href="javascript:OpenDevicePopup('+ index +',1)" data-ajax="false"><img class="ui-li-icon" id=switch_'+ device.id+' src="media/off.png">'+ device.naam +'</a></li>');
				}
				
				//Toggle
				if (device.toggle == 1) {
					$('#devices').append('<li data-icon="toggle"><a href="javascript:send_event(&quot;&action=toggle&device_id=' + device.id  + '&quot;,&quot;device&quot;)" data-ajax="false"><img class="ui-li-icon" id=switch_'+ device.id+' src="media/off.png">'+ device.naam +'</a></li>');
                }
				break;
           
		   //newKAKU
			case "2":  
                
				//Toggle
				if (device.toggle == 1) { 
					
					//Zonder dim function
					if (device.dim == 0) {
						$('#devices').append('<li data-icon="toggle"><a href="javascript:send_event(&quot;&action=toggle&device_id=' + device.id  + '&quot;,&quot;device&quot;)" data-ajax="false"><img class="ui-li-icon" id=switch_'+ device.id+' src="media/off.png">'+ device.naam +'</a></li>');
					}
					
					//Met dim functie
					//Buttons
					if (device.dim == 1 ) {
						$('#devices').append('<li data-icon="toggle"><a href="javascript:send_event(&quot;&action=toggle&device_id=' + device.id  + '&quot;,&quot;device&quot;)" data-ajax="false"><img class="ui-li-icon" id=switch_'+ device.id+' src="media/off.png">'+ device.naam +'</a><a href="javascript:OpenDevicePopup('+ index +',2)"> Dim</a></li>');
					}
					//Slider
					if (device.dim == 2 ) {
						$('#devices').append('<li data-icon="toggle"><a href="javascript:send_event(&quot;&action=toggle&device_id=' + device.id  + '&quot;,&quot;device&quot;)" data-ajax="false"><img class="ui-li-icon" id=switch_'+ device.id+' src="media/off.png">'+ device.naam +'</a><a href="javascript:OpenDevicePopup('+ index +',3)"> Dim</a></li>');
					}
				}
				
				//Aan/uit/dim
				if (device.toggle == 0) { 
					
					//Zonder dim function
					if (device.dim == 0) {
						$('#devices').append('<li data-icon="arrow-u"><a href="javascript:OpenDevicePopup('+ index +',1)" data-ajax="false"><img class="ui-li-icon" id=switch_'+ device.id+' src="media/off.png">'+ device.naam +'</a></li>');
					}
					
					//Met dim functie
					//Buttons
					if (device.dim == 1) {
						$('#devices').append('<li data-icon="arrow-u"><a href="javascript:OpenDevicePopup('+ index +',1)" data-ajax="false"><img class="ui-li-icon" id=switch_'+ device.id+' src="media/off.png">'+ device.naam +'</a><a href="javascript:OpenDevicePopup('+ index +',2)"> Dim</a></li>');
					}
					if (device.dim == 2) {
						$('#devices').append('<li data-icon="arrow-u"><a href="javascript:OpenDevicePopup('+ index +',1)" data-ajax="false"><img class="ui-li-icon" id=switch_'+ device.id+' src="media/off.png">'+ device.naam +'</a><a href="javascript:OpenDevicePopup('+ index +',3)"> Dim</a></li>');
					}
				}
				break;
            }
        });
        $('#devices').listview('refresh');
		Device_State();
    });
}

