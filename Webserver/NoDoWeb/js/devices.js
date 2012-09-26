var varDeviceTimer;
var varDeviceDialogTimer;
var varDeviceIndex;

var ArrayDeviceID=new Array();
var ArrayDeviceName=new Array();
var ArrayLabelOn=new Array();
var ArrayLabelOff=new Array();

$('#devices_page').on('pageinit', function(event) {
    checkSession();
	getDevices();
	
	
	
});


function getDevices() {
    $.getJSON('webservice/json_devices.php', function(data) {
        devices = data.devices;
		
        $('#devices li').remove();
        $.each(devices, function(index, device) {

            ArrayDeviceID[index]=device.id;
            ArrayDeviceName[index]=device.naam;
			if (device.labelon != '') {ArrayLabelOn[index]=device.labelon;} else {ArrayLabelOn[index]='On';}
			if (device.labeloff != '') {ArrayLabelOff[index]=device.labeloff;} else {ArrayLabelOff[index]='Off';}
			
          switch (device.type) {

            case "1": //KAKU
            case "3": //WiredOut
            case "4": //SendUserEvents
            case "5": //UserEvents
                			
				//Aan uit
				if (device.toggle == 0) {
					$('#devices').append('<li data-icon="arrow-u"><a href="javascript:OpenDeviceDialog('+ index +',1)" data-ajax="false"><img class="ui-li-icon" id=switch_'+ device.id+' src="media/off.png">'+ device.naam +'</a></li>');
					
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
						$('#devices').append('<li data-icon="toggle"><a href="javascript:send_event(&quot;&action=toggle&device_id=' + device.id  + '&quot;,&quot;device&quot;)" data-ajax="false"><img class="ui-li-icon" id=switch_'+ device.id+' src="media/off.png">'+ device.naam +'</a><a href="javascript:OpenDeviceDialog('+ index +',2)"> Dim</a></li>');
					}
					//Slider
					if (device.dim == 2 ) {
						$('#devices').append('<li data-icon="toggle"><a href="javascript:send_event(&quot;&action=toggle&device_id=' + device.id  + '&quot;,&quot;device&quot;)" data-ajax="false"><img class="ui-li-icon" id=switch_'+ device.id+' src="media/off.png">'+ device.naam +'</a><a href="javascript:OpenDeviceDialog('+ index +',3)"> Dim</a></li>');
					}
				}
				
				//Aan/uit/dim
				if (device.toggle == 0) { 
					
					//Zonder dim function
					if (device.dim == 0) {
						$('#devices').append('<li data-icon="arrow-u"><a href="javascript:OpenDeviceDialog('+ index +',1)" data-ajax="false"><img class="ui-li-icon" id=switch_'+ device.id+' src="media/off.png">'+ device.naam +'</a></li>');
					}
					
					//Met dim functie
					//Buttons
					if (device.dim == 1) {
						$('#devices').append('<li data-icon="arrow-u"><a href="javascript:OpenDeviceDialog('+ index +',1)" data-ajax="false"><img class="ui-li-icon" id=switch_'+ device.id+' src="media/off.png">'+ device.naam +'</a><a href="javascript:OpenDeviceDialog('+ index +',2)"> Dim</a></li>');
					}
					if (device.dim == 2) {
						$('#devices').append('<li data-icon="arrow-u"><a href="javascript:OpenDeviceDialog('+ index +',1)" data-ajax="false"><img class="ui-li-icon" id=switch_'+ device.id+' src="media/off.png">'+ device.naam +'</a><a href="javascript:OpenDeviceDialog('+ index +',3)"> Dim</a></li>');
					}
					
									
					
					
				}
				break;
            }
        });
        $('#devices').listview('refresh');
		Device_State();
    });
}



$('#devices_page').on('pageshow', function(event) {
	//Device status loop opstarten indien de pagina word weergegeven
	checkSession();
	Device_State();
    varDeviceTimer=setInterval(function() {Device_State()},5000);
	
	
});



$('#devices_page').on('pagehide', function(event) {
	//Device status loop stoppen indien een andere pagina word geopend
    clearInterval(varDeviceTimer);

});


function OpenDeviceDialog(index,type) {
	varDeviceIndex = index;
	varType = type;
   
	//$.mobile.changePage('#device_dialog', {transition: 'none',role: 'dialog'});
$( "#device_popup" ).popup("open");
};




$( "#device_popup" ).bind({
   popupbeforeposition: function(event, ui) { 
   
   if (varType == 1) { //aan uit
        $('#divdevice').append('<li data-theme="'+themeheader+'" data-role="divider">' + ArrayDeviceName[varDeviceIndex] + '</li>' +
		'<li data-icon="check"><a href="javascript:send_event(&quot;&action=on&device_id=' + ArrayDeviceID[varDeviceIndex]  + '&quot;,&quot;device&quot;)" data-ajax="false">'+ ArrayLabelOn[varDeviceIndex] +'</a></li>' +
	    '<li data-icon="delete"><a href="javascript:send_event(&quot;&action=off&device_id=' + ArrayDeviceID[varDeviceIndex]  + '&quot;,&quot;device&quot;)" data-ajax="false">'+ ArrayLabelOff[varDeviceIndex] +'</a></li>');
    }

    if (varType == 2) { //dim buttons
        $('#divdevice').append('<li data-theme="'+themeheader+'" data-role="divider">Dim: ' + ArrayDeviceName[varDeviceIndex] + '</li>' +
        '<li data-icon="false"><a href="javascript:send_event(&quot;&action=dim&device_id=' + ArrayDeviceID[varDeviceIndex]  + '&dim=2&quot;,&quot;device&quot;)" data-ajax="false">10%</a></li>' +
        '<li data-icon="false"><a href="javascript:send_event(&quot;&action=dim&device_id=' + ArrayDeviceID[varDeviceIndex]  + '&dim=3&quot;,&quot;device&quot;)" data-ajax="false">20%</a></li>' +
        '<li data-icon="false"><a href="javascript:send_event(&quot;&action=dim&device_id=' + ArrayDeviceID[varDeviceIndex]  + '&dim=5&quot;,&quot;device&quot;)" data-ajax="false">30%</a></li>' +
        '<li data-icon="false"><a href="javascript:send_event(&quot;&action=dim&device_id=' + ArrayDeviceID[varDeviceIndex]  + '&dim=6&quot;,&quot;device&quot;)" data-ajax="false">40%</a></li>' +
        '<li data-icon="false"><a href="javascript:send_event(&quot;&action=dim&device_id=' + ArrayDeviceID[varDeviceIndex]  + '&dim=8&quot;,&quot;device&quot;)" data-ajax="false">50%</a></li>' +
        '<li data-icon="false"><a href="javascript:send_event(&quot;&action=dim&device_id=' + ArrayDeviceID[varDeviceIndex]  + '&dim=10&quot;,&quot;device&quot;)" data-ajax="false">60%</a></li>' +
        '<li data-icon="false"><a href="javascript:send_event(&quot;&action=dim&device_id=' + ArrayDeviceID[varDeviceIndex]  + '&dim=11&quot;,&quot;device&quot;)" data-ajax="false">70%</a></li>' +
        '<li data-icon="false"><a href="javascript:send_event(&quot;&action=dim&device_id=' + ArrayDeviceID[varDeviceIndex]  + '&dim=13&quot;,&quot;device&quot;)" data-ajax="false">80%</a></li>' +
        '<li data-icon="false"><a href="javascript:send_event(&quot;&action=dim&device_id=' + ArrayDeviceID[varDeviceIndex]  + '&dim=14&quot;,&quot;device&quot;)" data-ajax="false">90%</a></li>' +
        '<li data-icon="false"><a href="javascript:send_event(&quot;&action=dim&device_id=' + ArrayDeviceID[varDeviceIndex]  + '&dim=16&quot;,&quot;device&quot;)" data-ajax="false">100%</a></li>');
		
	}
	
	if (varType == 3) { //dim slider
         $('#divdevice').append('<li data-theme="'+themeheader+'" class="ui-corner-top" data-role="divider">Dim: ' + ArrayDeviceName[varDeviceIndex] + '</li>' +
        '<div style="min-width:275px; padding:20px 20px;">'+
		'<input  name="distSlider" id="distSlider'+ ArrayDeviceID[varDeviceIndex] +'" value="' + ArrayDeviceDimvalue[ArrayDeviceID[varDeviceIndex]] + '" min="1" max="16" step="1" data-type="range" onChange="update_distance_device_timer('+ ArrayDeviceID[varDeviceIndex] +')"></div>').trigger("create");
		
		
		
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


$('#device_dialog').on('pagehide', function(event) {
   
});


//Device slider functie
function update_distance_device_timer(index)	{
	SliderIndex = index;
	clearTimeout(SliderTimer);
	SliderTimer=setTimeout("update_distance_device(SliderIndex)",200);
	
}
function update_distance_device(index) {
	
	var val = $('#distSlider'+index).val();
	//alert(index);
	send_event('&action=dim&device_id=' + ArrayDeviceID[varDeviceIndex]  + '&dim='+ val,'device')
	
					
}

