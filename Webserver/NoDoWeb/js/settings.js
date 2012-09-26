var theme='c';
var themeheader='a';
var nodoip;
var nodoport;
var nodoid;
var webappversion='WebApp (SWINWA-PROD-V099)';

var url='/webapp/webservice/json_settings.php';
//$.ajax omdat we de settings synchroon willen ophalen zodat we zeker weten dat de settings beschikbaar zijn als de pagina's worden geladen.
$.ajax({ 
         async: false, 
         url: url, 
         dataType: "json", 
         success: function(data) {
 			settings = data.settings;
			$.each(settings, function(index, setting) {
			theme=setting.theme;
			themeheader=setting.themeheader;
			nodoip=setting.nodoip;
			nodoport=setting.nodoport;
			nodoid=setting.nodoid;
			});
		}
});



//Thema instellen
$(document).delegate('[data-role="page"]', 'pagecreate', function () { 
 
 
	$(this).removeClass('ui-body-a ui-body-b ui-body-c ui-body-d ui-body-e').addClass('ui-body-'+theme).attr('data-theme', theme); 
	$('[data-role=header]').removeClass('ui-bar-a ui-bar-b ui-bar-c ui-bar-d ui-bar-e').addClass('ui-header ui-bar-'+themeheader);
	$('[data-role=footer]').removeClass('ui-bar-a ui-bar-b ui-bar-c ui-bar-d ui-bar-e').addClass('ui-footer ui-bar-'+themeheader);
	$('[data-role=listview]').attr('data-split-theme', theme);
	$('[data-role=dialog]').attr('data-theme', theme);
	
	
});





