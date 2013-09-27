


$('#alarms_page').on('pageinit', function(event) {
	

	Nodo_State();
	checkSession();
	getAlarms();
	
	
	
	
});

$('#alarms_page').on('pageshow', function(event) {
	pagetitle = 'Alarms';
	$('#header_alarms').append('<div id="nodostate">'+pagetitle+'</div>');
	Nodo_State();
	checkSession();
	getAlarms();
	
});

$('#alarms_page').on('pagehide', function(event) {
	
	$('#header_alarms').empty();
});

function getAlarms() {
	
	$.getJSON('webservice/json_alarms.php?alarms=1', function(data) {
		
		$('#alarms').empty();
				
		if (data.alarms != null) {
		
			$.each(data.alarms, function(index, alarm) {
				
				if (alarm.state == 'Off') { 
					$('#alarms').append('<li data-icon="gear"><a href="javascript:editAlarm('+alarm.alarmnr+')" data-ajax="false">Alarm '+ alarm.alarmnr+', ' + alarm.hour +':'+alarm.minute+ ', '+ alarm.day +'<img class="ui-li-icon" src="media/off.png"></a></li>'); 
					}
				else {
					$('#alarms').append('<li data-icon="gear"><a href="javascript:editAlarm('+alarm.alarmnr+')" data-ajax="false">Alarm '+ alarm.alarmnr+', ' + alarm.hour +':'+alarm.minute+ ', '+ alarm.day +'<img class="ui-li-icon" src="media/on.png"></a></li>'); 
					}
									
			});
			
			$('#alarms').listview('refresh');
		}
	});
}

$('#alarms_page').on('pagehide', function(event) {
	
	$('#header_setup_alarms').empty();
});





        
		
				
function editAlarm(nr) {		
		
		$.getJSON('webservice/json_alarms.php?alarm=' + nr, function(data) {
		alarms = data.alarm;
				
			
			$.each(alarms, function(index, alarm) {
				
				$('#alarmnr').val(nr);
				$('#state').val(alarm.state);
				$('#state').change();
				$('#state').selectmenu('refresh', true);
				
				$('#hour').val(alarm.hour);
				//$('#hour').change();
				//$('#hour').selectmenu('refresh', true);
				
				$('#minute').val(alarm.minute);
				//$('#minute').change();
				//$('#minute').selectmenu('refresh', true);
				
				
					$('#day').val(alarm.day);
					$('#day').change();
					$('#day').selectmenu('refresh', true);
								
									
			});
			
			$( "#alarm_edit_popup" ).popup("open");			
	    });
		
		
};

$('#save').click(function( e ) {


	$response=send_event('alarmset '+$('#alarmnr').val()+','+$('#state').val()+','+$('#hour').val()+$('#minute').val()+','+$('#day').val());

	//alert ('Alarm setting send to Nodo... Nodo response: '+$response);
	
	$( "#alarm_edit_popup" ).popup("close");
	setTimeout(function(){getAlarms()}, 1000);

});

$('#hour').keyup(function() {
	if (this.value.match(/[^*0-9 ]/g)) {
		this.value = this.value.replace(/[^*0-9 ]/g, '');
	}
});

$('#minute').keyup(function() {
	if (this.value.match(/[^*0-9 ]/g)) {
		this.value = this.value.replace(/[^*0-9 ]/g, '');
	}
});