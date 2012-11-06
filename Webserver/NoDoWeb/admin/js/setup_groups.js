var GroupID;

$(document).ready(function(e){
    $("#group_add").on("submit", function(e){
				
			e.preventDefault();
			
			$.post("../webservice/admin/json_setup_groups.php", $('#group_add').serialize(),function(data) {   
			
			if (data == true) {
			
				getGroups();
				$("#group_name_add").val('');
				$("#checkbox_devices_add").attr("checked", false);
				$("#checkbox_activities_add").attr("checked", false);
				$("#checkbox_devices_default_add").attr("checked", false);
				$("#checkbox_activities_default_add").attr("checked", false);
				$("#checkbox_devices_add").checkboxradio('refresh');
				$("#checkbox_activities_add").checkboxradio('refresh');
				$("#checkbox_devices_default_add").checkboxradio('refresh');
				$("#checkbox_activities_default_add").checkboxradio('refresh')
				$.mobile.changePage( "#groups_page", { transition: "none"} );
			}
			else {
			
				alert ('ERROR: Group not saved!');
			}
			
			});
	});
	
	
	$("#group_edit").on("submit", function(e){
				
			e.preventDefault();
			
			$.post("../webservice/admin/json_setup_groups.php", $('#group_edit').serialize(),function(data) {   
			
			if (data == true) {
			
				getGroups();
				$("#group_name_edit").val('');
				$("#checkbox_devices_edit").attr("checked", false);
				$("#checkbox_activities_edit").attr("checked", false);
				$("#checkbox_devices_default_edit").attr("checked", false);
				$("#checkbox_activities_default_edit").attr("checked", false);
				$("#checkbox_devices_edit").checkboxradio('refresh');
				$("#checkbox_activities_edit").checkboxradio('refresh');
				$("#checkbox_devices_default_edit").checkboxradio('refresh');
				$("#checkbox_activities_default_edit").checkboxradio('refresh');
				$.mobile.changePage( "#groups_page", { transition: "none"} );
			}
			else {
			
				alert ('ERROR: Group not modified!');
			}
			
			});
	
	});
});

$('#groups_page').on('pageinit', function(event) {
		
	Nodo_State();
	checkSession();
	getGroups();
		
});

$('#groups_page').on('pageshow', function(event) {
	
	pagetitle = 'Setup: Groups';
	$('#header_setup_groups').append('<div id="nodostate">'+pagetitle+'</div>');
	
});

$('#groups_page').on('pagehide', function(event) {
	
	$('#header_setup_groups').empty();
});

$('#groups_add_page').on('pageshow', function(event) {
	
	pagetitle = 'Setup: Add Group';
	$('#header_setup_groups_add').append('<div id="nodostate">'+pagetitle+'</div>');
	
	checkSession();
	
});

$('#groups_add_page').on('pagehide', function(event) {
	
	$('#header_setup_groups_add').empty();
});

$('#groups_edit_page').on('pageshow', function(event) {
	
	pagetitle = 'Setup: Edit Group';
	$('#header_setup_groups_edit').append('<div id="nodostate">'+pagetitle+'</div>');
		
	
	checkSession();
	
});

$('#groups_edit_page').on('pagehide', function(event) {
	
	$('#header_setup_groups_edit').empty();
});



function getGroups() {

	$('#grouplist').empty();
	
	$.getJSON('../webservice/admin/json_setup_groups.php?readgroups=1', function(data) {
		
				
		if (data.groups != null) {
			
			$('#grouplist').append('Groups:<br \>');
			
			$.each(data.groups, function(index, group) {
								
				$('#grouplist').append('<div data-role="collapsible" data-collapsed="true" data-iconpos="right" data-content-theme="'+theme+'">' +
										'<h3>'+group.name+'</h3>' +
										'<a href="javascript:editGroup('+group.id+')" data-role="button" data-icon="gear" data-ajax="false">Edit</a>' +
										'<a href="javascript:confirmDeleteGroup('+group.id+')" data-role="button"  data-icon="delete" data-rel="dialog">Delete</a>' +
										'</div>');
											
			});
			
			
			$('#grouplist').trigger('create');
			
		}
		else {
				
				$('#grouplist').append('No groups defined...');
				                       
		}
	});
}

function confirmDeleteGroup(id) {
	GroupID = id;
	$('#delete_message').empty();
	$('#delete_message').append('Delete group?');
	$('#groups_popup_delete').popup("open");

}

function deleteGroup() {


$.post("../webservice/admin/json_setup_groups.php", { deletegroup: "1", id: GroupID },function(data) {
					
		getGroups()
		$('#groups_popup_delete').popup("close");
		
	});
	
	GroupID=0;

}

function editGroup(id) {
	
	$("#checkbox_devices_edit").attr("checked", false);
	$("#checkbox_activities_edit").attr("checked", false);
	$("#checkbox_devices_default_edit").attr("checked", false);
	$("#checkbox_activities_default_edit").attr("checked", false);
	
	$.getJSON('../webservice/admin/json_setup_groups.php?readgroups=2&id='+id, function(data) {
		
				
		if (data.groups != null) {
			
			
			$.each(data.groups, function(index, group) {
				$("#group_id_edit").val(group.id);
				$("#group_name_edit").val(group.name);
				
				if (group.devices == 1) {$("#checkbox_devices_edit").attr("checked", true);}
				if (group.activities == 1) {$("#checkbox_activities_edit").attr("checked", true);}
				if (group.devices_default == 1) {$("#checkbox_devices_default_edit").attr("checked", true);}
				if (group.activities_default == 1) {$("#checkbox_activities_default_edit").attr("checked", true);}
														
			});
			
			
			
			
		}
		
		$.mobile.changePage( "#groups_edit_page", { transition: "none"} );
		
		$("#checkbox_devices_edit").checkboxradio('refresh');
	    $("#checkbox_activities_edit").checkboxradio('refresh');
		$("#checkbox_devices_default_edit").checkboxradio('refresh');
	    $("#checkbox_activities_default_edit").checkboxradio('refresh');
	
	});
}
	


