$('#setup_page').on('pageinit', function(event) {
	checkSession();
	
	if (usergroup == 'admin') {
		
		$('#listviewsetup').append('<li data-role="list-divider">WebApp Administrator links</li>' +
		'<li data-icon="star"><a href="../webapp_admin/user_list.php" data-ajax="false">User list</a></li>');
		
		$('#listviewsetup').listview('refresh');
	}
});

$('#setup_page').on('pageshow', function(event) {
	
	pagetitle='Setup';
	
	checkSession();
    $('#header_setup_index').append('<div id="nodostate">'+pagetitle+'</div>');
	Nodo_State();
	
});