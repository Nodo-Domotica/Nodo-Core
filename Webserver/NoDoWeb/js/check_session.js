//Deze functie controleerd of er een login sessie bestaat
function checkSession() {
	$.ajaxSetup({ cache: false });
	$.getJSON('/webapp/webservice/json_check_session.php', function(data) {
			
		
		if (data.authorized != 1) {
			
				
				document.location.href = '/webapp/index.html'
		}
	});
}