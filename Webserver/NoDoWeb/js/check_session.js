//Deze functie controleerd of er een login sessie bestaat
function checkSession() {

	$.getJSON('/webapp/webservice/json_check_session.php', function(data) {
			
		
		if (data.authorized != 1) {
			
				//$.mobile.changePage('webapp.html', {transition: 'none',reloadPage: true});
				document.location.href = '/webapp/index.html'
		}
		});
}