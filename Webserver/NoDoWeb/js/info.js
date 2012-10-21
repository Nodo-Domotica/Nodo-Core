$('#info_page').on('pageinit', function(event) {
   checkSession();
   $('#webappdetails').append('<b>Web App version: </b>'+ webappversion +'<br \>' +
							  '<b>Nodo IP-address: </b>'+ nodoip + ':' + nodoport + '<br \>').trigger("create");
   
});


$('#info_page').on('pageshow', function(event) {
	
	pagetitle='Info';
	
	checkSession();
    Get_Nodo_Events(); 
    varEventsTimer=setInterval(function() {Get_Nodo_Events()},2000);
	$('#header_info').append('<div id="nodostate">'+pagetitle+'</div>');
	Nodo_State();
	
});

$('#info_page').on('pagehide', function(event) {
	
    clearInterval(varEventsTimer);
	$('#header_info').empty();	

});

$('div.nodostatus').on('expand', function(){

	Get_Nodo_Status();
	
});