$('#scripts_page').on('pageshow', function(event) {
	
	pagetitle = 'Setup: Scripts';
		
	$('#header_setup_scripts').append('<div id="nodostate">'+pagetitle+'</div>');
	Nodo_State();
	checkSession();
	
	getFiles(); //files ophalen vanaf nodo
});

$('#scripts_page').on('pagehide', function(event) {
	
	$('#header_setup_scripts').empty();
});


function getFiles() {
	
	$('#scriptfile').empty();
	$('#scriptfile').append('<option value="list">Please wait, getting scripts...</option>');
	$('#scriptfile').selectmenu("refresh", true);
	$('#delete').button('disable');
	$('#write').button('disable');
	$('#checkbox').checkboxradio('disable');
	$('#scriptcontent').textinput('disable');
						
	$.getJSON('../webservice/admin/json_scripts.php?files', function(data) {
		files = data.files;
		
		$('#scriptfile').empty();
	
		
		
			$('#scriptfile').append('<option data-placeholder="true" value="choose">Select script...</option>');
		
		
		
		$.each(files, function(index, file) {
								
			$('#scriptfile').append('<option value="'+file.file+'">'+file.file+'</option>');
			
						
		});
		
		
		//Bij een nieuwe file selecteren we deze
		if ($('#scriptfilenew').val().toUpperCase() != '') {
		
			$('#scriptfile').val($('#scriptfilenew').val().toUpperCase());
			$('#scriptfile').change();
		
		}
		
		$('#scriptfile').selectmenu('refresh', true);
	});
	
	
}


$('#scriptfile').change( function( e ) { 
        
		e.preventDefault();
		
		$('#scriptcontent').val('').css('height', '50px');
		
					
			if ($('#scriptfile').val() != 'choose') {
				$('#delete').button('enable');
				$('#write').button('enable');
				$('#checkbox').checkboxradio('enable');
				$('#scriptcontent').textinput('enable');
				
			}
			else {
				$('#delete').button('disable');
				$('#write').button('disable');
				$('#checkbox').checkboxradio('disable');
				$('#scriptcontent').textinput('disable');
			}
						
			if ($('#scriptfile').val() != 'choose') {			
				$.post("../webservice/admin/json_scripts.php", { read: "1", scriptfile: $('#scriptfile').val() },function(data) {   
				$('#scriptcontent').val(data).keyup();
				$('#scriptcontent').focus();
				});
			}
});

//Eventlist wijkt af van de andere files
$('#eventlist').click( function( e ) { 
		
		e.preventDefault();
		
		$('#scriptfile').val('choose');
		$('#scriptfile').change();
		
		$('#scriptcontent').val('').css('height', '50px');
		
		$.post("../webservice/admin/json_scripts.php", { read: "1", scriptfile: "EVENTLST" },function(data) {   
			$('#scriptcontent').val(data).keyup();
			$('#scriptcontent').textinput('enable');
			//$('#write').button('enable');
		    //$('#checkbox').checkboxradio('enable');
			$('#scriptcontent').focus();
		});
       
});


$('#write').click( function( e ) { 
		
		
		e.preventDefault();
		$('#write_message').empty();
		
        if ($('#scriptfile').val() != 'choose') {
			$.post("../webservice/admin/json_scripts.php", { checkbox: $("#checkbox").is(":checked"), write: "1", scriptfile: $('#scriptfile').val(), script: $('#scriptcontent').val() },function(data) {   
			
			if ($("#checkbox").is(":checked")==true) {
				$('#write_message').append('The script is sent to the Nodo<br \> and will be executed.');
			}
			else {
				$('#write_message').append('The script is send to the Nodo.');
			}
			
			//alert($("#checkbox").is(":checked"));
			$( "#scripts_popup_msg" ).popup("open");
			
					
			});
		}
});


$('#scriptfilenew').keyup(function() {
	if (this.value.match(/[^a-zA-Z0-9 ]/g)) {
		this.value = this.value.replace(/[^a-zA-Z0-9 ]/g, '');
	}
});


$('#new').click(function( e ) { 
	
	$('#scriptfilenewdiv').show();
	$('#new2 .ui-btn-text').text("Cancel");
	$('#new3').button('enable');
	$('#new_message').empty();
	$('#scriptfilenew').val('');
	$('#scripts_popup_new').popup("open");
	
	
});


$('#new3').click(function( e ) { 
	
	$('#new2 .ui-btn-text').text("Close");
	$('#new3').button('disable');
	$('#new_message').empty();
	$('#scriptfilenewdiv').hide();
	
	//controleren of het script al bestaat
	var existingfiles = $('#scriptfilenew').val().toUpperCase();
	var fileexists = 0 != $('#scriptfile option[value='+existingfiles+']').length;
	//alert (exists);
	
	if ($('#scriptfilenew').val() != '' && fileexists == false ) {
	
		$('#scriptcontent').val('').css('height', '50px');
	
		$('#new_message').append('Creating file: ' + $('#scriptfilenew').val().toUpperCase() +'...');
	
		$.post("../webservice/admin/json_scripts.php", { newfile: "1", scriptfile: $('#scriptfilenew').val() },function(data) {
		
		$('#new_message').empty();
		$('#new_message').append('Script '+ $('#scriptfilenew').val().toUpperCase() +' created.');
		
		
		getFiles()
		
		
		
		});
	
	}
	
	else {
	
		if ($('#scriptfilenew').val() == '') {
		
			$('#new_message').append('Please fill in a scriptname!');
			$('#scriptfilenewdiv').show();
			$("#new2 .ui-btn-text").text("Cancel");
			$('#new3').button('enable');
		}
		
		if (fileexists == true) {
		
			$('#new_message').append('Script already exists!');
			$('#scriptfilenewdiv').show();
			$("#new2 .ui-btn-text").text("Cancel");
			$('#new3').button('enable');
		}
		
		
		
	}
		
});


$('#delete').click(function( e ) { 
	
	if ($('#scriptfile').val() != 'choose') {
		$('#scripts_popup_delete').popup("open");
		$("#delete2 .ui-btn-text").text("Cancel");
		
		$('#delete3').button('enable');
		$('#delete_message').empty();
		$('#delete_message').append('Delete script: ' + $('#scriptfile').val() + '?');
	}
});


$('#delete3').click(function( e ) { 
	
	$('#delete3').button('disable');
	
	$.post("../webservice/admin/json_scripts.php", { deletefile: "1", scriptfile: $('#scriptfile').val() },function(data) {
		
		$("#delete2 .ui-btn-text").text("Close");
		
		$('#delete_message').empty();
		$('#delete_message').append($('#scriptfile').val() + ' deleted.');
		$('#scriptcontent').val('').css('height', '50px');
		
		getFiles()
		
	});
	
});

$( "#scripts_popup_new" ).bind({
   popupafteropen: function(event, ui) {  $('#scriptfilenew').focus(); }
});

$( "#scripts_popup_new" ).bind({
   popupafterclose: function(event, ui) { $('#scriptcontent').focus(); }
});

	
	