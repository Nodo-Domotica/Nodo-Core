$('#info_page').on('pageinit', function(event) {
   checkSession();
   $('#webappdetails').append('<b>Web App version: </b>'+ webappversion +'<br>' +
							  '<b>Nodo IP-address: </b>'+ nodoip + ':' + nodoport + '<br>').trigger("create");
							  
   
   
   
   
});


$('#info_page').on('pageshow', function(event) {
	checkSession();
    Get_Nodo_Events(); 
    varEventsTimer=setInterval(function() {Get_Nodo_Events()},2000);
	
});

$('#info_page').on('pagehide', function(event) {
	
    clearInterval(varEventsTimer);

});

$('div.nodostatus').on('expand', function(){

	Get_Nodo_Status();
});