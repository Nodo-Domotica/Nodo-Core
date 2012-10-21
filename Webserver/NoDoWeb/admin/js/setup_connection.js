

$('#setup_connection_page').on('pageinit', function(event) {
	checkSession();
	getConnectionSettings();
});

function getConnectionSettings() {
	$.getJSON('../webservice/admin/json_setup_connection.php', function(data) {
		connection = data.connection;
		
		$.each(connection, function(index, connectionsetting) {
			
			$('#ipmessage').prepend(connectionsetting.nodoipmsg);
			
			$('#nodo_ip').val(connectionsetting.nodoip);
			
			$('#nodo_ipmessage').prepend(connectionsetting.nodoipmsg);
			
			$('#nodo_port').val(connectionsetting.nodoport).attr('selected', true).siblings('option').removeAttr('selected');
			$('#nodo_port').selectmenu("refresh", true);
			
			$('#nodo_password').val(connectionsetting.nodopassword);
			$('#nodo_id').val(connectionsetting.nodoid);
			
			$('#nodo_id_txt').append(connectionsetting.nodoid);
			
		});
			
	});
}


$(document).ready(function(e){
    $("#form_connection").on("submit", function(e){
        
		if ($('#nodo_password').val() != '') {
		
			$('#form_connection').hide();
			$('#check_connection_div').show();
			//IE bug!?
			document.getElementById('check_connection_div').innerHTML = document.getElementById('check_connection_div').innerHTML;
			
			e.preventDefault();
			
			$.post("../webservice/admin/json_setup_connection.php", $('#form_connection').serialize(),function(data) {   
			
				$('#form_connection').show();
				$('#check_connection_div').hide();
				
				connection = data.connection;
				
				$.each(connection, function(index, connsetting) {
					
					$('#result_msg').empty();
					$('#result_sub_msg').empty();
					$( "#connection_popup" ).popup("open");
					//alert(connsetting.response);
					
					if (connsetting.response == '1') {	$('#result_msg').append('Connection succeeded!'); }
					if (connsetting.response == '2') {	$('#result_msg').append('Connection succeeded!'); $('#result_sub_msg').append('<FONT COLOR=\"red\">Your Nodo is not properly configured.<br \>You can use the option [Auto configure your default Nodo] to configure your Nodo.</font>');}
					if (connsetting.response == '3') {	$('#result_msg').append('Connection failed!'); }
					if (connsetting.response == '4') { $('#result_msg').append('Authentication failed!'); $('#result_sub_msg').append('Does the Nodo password and Nodo ID match your Nodo?');}	
					if (connsetting.response == '5') { $('#result_msg').append('Configuration failed!'); $('#result_sub_msg').append('Please make sure your Nodo has default settings');}
					if (connsetting.response == '6') { $('#result_msg').append('Configuration succeeded!');}
					if (connsetting.response == '7') { $('#result_msg').append('Configuration warning!'); $('#result_sub_msg').append('We could not verify the Nodo configuration.<br \> Is there an sd card in your Nodo?');}				
				});
			
			}, 'json');
		
		}
		else {
				e.preventDefault();
				$('#result_msg').empty();
				$('#result_sub_msg').empty();
				$('#connection_popup').popup("open");
				$('#result_msg').append('You have to fill in a Nodo password!');
		}
		
    });
});

$('#setup_connection_page').on('pageshow', function(event) {
	
	pagetitle='Setup: Communication';
	
	checkSession();
    $('#header_setup_communication').append('<div id="nodostate">'+pagetitle+'</div>');
	Nodo_State();
	
});

$('#setup_connection_page').on('pagehide', function(event) {
	
    $('#header_setup_communication').empty();
	
	
});



