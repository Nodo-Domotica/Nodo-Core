function getValueState(){

var url='webservice/json_value_state.php';

var status;
	
		$.getJSON(url,function(data){
			
			if (data != null) {
				$.each(data, function(i,result){
							
					document.getElementById('value_'+result.id).innerHTML = result.value;
											   
				});
			}
		});

}