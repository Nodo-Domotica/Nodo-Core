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

function CheckConnection() {


	$('#form_connection').hide();
	$('#check_connection_div').show();
	//IE bug!?
	document.getElementById('check_connection_div').innerHTML = document.getElementById('check_connection_div').innerHTML;



  $.post('../webservice/admin/json_setup_connection.php', $(this).serialize(), function (data) {
 
 alert ("ok");
 
  }, "json");


	$('#form_connection').show();
	$('#check_connection_div').hide();
}
 
  
