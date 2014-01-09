
$('#login_form').bind('submit', function () {

    //send a post request to your web-service
    $.post("login.php", $(this).serialize(), function (response) {
       
	    //check if the authorization was successful or not
        
		if (response.authorized > 0) {
		
			//$.mobile.changePage('webapp.html', {transition: 'none',reloadPage: true});
			document.location.href = 'webapp.html'
			
        } else {
            
			alert('Login failed');
			
            
        }
    }, "json");

    return false;
});

$("#logon_page").on("pageinit", function() {
	$.getJSON('login.php', function(data) {
				
			
			if (data.authorized > 0) {
			
				var html = '<h4>Logging you in with saved credentials...</h4>';
				
				$( "#login_form" ).append( html ).trigger('create');
				
				if (data.page == 1) {document.location.href = 'webapp.html';}
				if (data.page == 2) {document.location.href = 'webapp.html#activities_page';}
				if (data.page == 3) {document.location.href = 'webapp.html#values_page';}
				
				
				
			}
			else {
			
				var html = '<label for="username">Username:</label>' +
				'<input type="text" name="username" id="username" value=""  />' +
				'<label for="password">Password:</label>' +
				'<input type="password" name="password" id="password" value=""  />' +
				'<br \>' +
				'<input type="checkbox" name="rememberme" id="rememberme_1" class="custom" />' +
				'<label for="rememberme_1">Remember me</label>' +
				'<br \>' +
				'<input type="submit" name="submit" value="Login" >';
				
				
				$( "#login_form" ).append( html ).trigger('create');
			
			}
	});
});

