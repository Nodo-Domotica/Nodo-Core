$('#activities_page').on('pageinit', function(event) {
	checkSession();
	getActivities();
});

function getActivities() {
	$.ajaxSetup({ cache: false });
	$.getJSON('webservice/json_activities.php', function(data) {
		activities = data.activities;
		$('#activities li').remove();
		$.each(activities, function(index, activitie) {
			
			$('#activities').append('<li data-icon="star"><a href="javascript:send_event(&quot;' + activitie.events  + '&quot;)" data-ajax="false">'+ activitie.naam +'</a></li>');	
								
		});
		$('#activities').listview('refresh');
	});
}

$('#activities_page').on('pageshow', function(event) {
	
	pagetitle = 'Activities';
	
	checkSession();
	$('#header_activities').append('<div id="nodostate">'+pagetitle+'</div>');
	Nodo_State();
});

$('#activities_page').on('pagehide', function(event) {
	checkSession();
	$('#header_activities').empty();
});