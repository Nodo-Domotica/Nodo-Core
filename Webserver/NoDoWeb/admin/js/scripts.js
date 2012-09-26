$('#scripts_page').bind('pageinit', function(event) {
	checkSession(); 
	getFiles(); //files ophalen vanaf nodo
});


function getFiles() {
	
	$('#scriptfile').empty();
	$('#scriptfile').append('<option value="list">Please wait, getting scripts...</option>');
	$('#scriptfile').selectmenu("refresh", true);
						
	$.getJSON('../webservice/admin/json_scripts.php?files', function(data) {
		files = data.files;
		
		$('#scriptfile').empty();
		
		//Eventlist wijkt af van de andere files
		$('#scriptfile').append('<option data-placeholder="true" value="choose">Choose script...</option>');
		$('#scriptfile').append('<option value="EVENTLST">Eventlist</option>');
		
		$.each(files, function(index, file) {
								
			$('#scriptfile').append('<option value="'+file.file+'">'+file.file+'</option>');
			
						
		});
		$('#scriptfile').selectmenu("refresh", true);	
	});
}


$('#scriptfile').change( function( e ) { 
        
		e.preventDefault();
		
		$('#scriptcontent').val('').css('height', '50px');
		
        $.post("../webservice/admin/json_scripts.php", { read: "1", scriptfile: $('#scriptfile').val() },function(data) {   
			
		$('#scriptcontent').val(data).keyup();
				
    });
});




$('#write').click( function( e ) { 
		
		
		e.preventDefault();
		$('#write_message').empty();
        
		$.post("../webservice/admin/json_scripts.php", { checkbox: $("#checkbox").is(":checked"), write: "1", scriptfile: $('#scriptfile').val(), script: $('#scriptcontent').val() },function(data) {   
		
		if ($("#checkbox").is(":checked")==true) {
			$('#write_message').append('The script is sent to the Nodo<BR \> and will be executed.');
		}
		else {
			$('#write_message').append('The script is send to the Nodo.');
		}
		
		//alert($("#checkbox").is(":checked"));
		$( "#scripts_popup_msg" ).popup("open");
		
				
    });
});


$('#scriptfilenew').keyup(function() {
	if (this.value.match(/[^a-zA-Z0-9 ]/g)) {
		this.value = this.value.replace(/[^a-zA-Z0-9 ]/g, '');
	}
});


$('#new').click(function( e ) { 
	
	$('#scriptfilenewdiv').show();
	$("#new2 .ui-btn-text").text("Cancel");
	$('#new3').button('enable');
	$('#new_message').empty();
	$( "#scripts_popup_new" ).popup("open");
	
});


$('#new3').click(function( e ) { 
	
	$("#new2 .ui-btn-text").text("Close");
	$('#new3').button('disable');
	$('#new_message').empty();
	$('#scriptfilenewdiv').hide();
	
	if ($('#scriptfilenew').val() != '') {
	
		$('#new_message').append('Creating file: ' + $('#scriptfilenew').val().toUpperCase() +'...');
	
		$.post("../webservice/admin/json_scripts.php", { new: "1", scriptfile: $('#scriptfilenew').val() },function(data) {
		
		$('#new_message').empty();
		$('#new_message').append('Script '+ $('#scriptfilenew').val().toUpperCase() +' created.');
		$('#scriptfilenew').val('');
		getFiles()
		
		});
	
	}
	
	else {
		
		$('#new_message').append('Please fill in a scriptname!');
		$('#scriptfilenewdiv').show();
		$("#new2 .ui-btn-text").text("Cancel");
		$('#new3').button('enable');
		
		}
		
});


$('#delete').click(function( e ) { 
	
	$('#scripts_popup_delete').popup("open");
	$("#delete2 .ui-btn-text").text("Cancel");
	
	$('#delete3').button('enable');
	$('#delete_message').empty();
	$('#delete_message').append('Delete script: ' + $('#scriptfile').val() + '?');
	
});


$('#delete3').click(function( e ) { 
	
	$('#delete3').button('disable');
	
	$.post("../webservice/admin/json_scripts.php", { delete: "1", scriptfile: $('#scriptfile').val() },function(data) {
		
		$("#delete2 .ui-btn-text").text("Close");
		
		$('#delete_message').empty();
		$('#delete_message').append($('#scriptfile').val() + ' deleted.');
		$('#scriptcontent').val('').css('height', '50px');
		
		getFiles()
		
	});
	
});
	
	