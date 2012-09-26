

$('#setup_connection_page').bind('pageinit', function(event) {
	checkSession();
	getConnectionSettings();
});

function getConnectionSettings() {
	$.getJSON('../webservice/admin/json_setup_connection.php', function(data) {
		connection = data.connection;
		
		$.each(connection, function(index, connectionsetting) {
			
			$('#nodo_ip').val(connectionsetting.nodoip);
			
			$('#nodo_port').val(connectionsetting.nodoport).attr('selected', true).siblings('option').removeAttr('selected');
			$('#nodo_port').selectmenu("refresh", true);
			
			$('#nodo_password').val(connectionsetting.nodopassword);
			$('#nodo_id').val(connectionsetting.nodoid);
			
			$('#nodo_id_txt').append(connectionsetting.nodoid);
			
		});
			
	});
}


$(document).ready(function(e){
    $("#form_connection").bind("submit", function(e){
        
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
				
				if (connsetting.response == 'ok') {	$('#result_msg').append('Connection succeeded!'); }
				if (connsetting.response == 'not_ok') {	$('#result_msg').append('Connection failed!'); }
				if (connsetting.response == 'forbidden') { $('#result_msg').append('Authentication failed!'); $('#result_sub_msg').append('Does the Nodo password and Nodo ID match your Nodo?');}	
				if (connsetting.response == 'forbidden_config') { $('#result_msg').append('Configuration failed!'); $('#result_sub_msg').append('Please make sure your Nodo has default settings');}
				if (connsetting.response == 'ok_config') { $('#result_msg').append('Configuration succeeded!');}
				if (connsetting.response == 'error_config') { $('#result_msg').append('Configuration warning!'); $('#result_sub_msg').append('We could not verify the Nodo configuration.<br> Is there an sd card in your Nodo?');}				
			});
		
		}, 'json');
		
		
		
    });
});

