var ArrayDeviceDimvalue=new Array();
function Device_State(){

var url='webservice/json_device_state.php';



var status;

	
		$.getJSON(url,function(data){
			$.each(data, function(i,data){
			 
				if (data.status == 1) {
					
					if (data.dimvalue > 0) {
						
						ArrayDeviceDimvalue[data.id]=data.dimvalue; //dimwaarde in array zetten welke weer gebruikt word voor de slider
						document.getElementById('switch_'+data.id).src = 'media/on_dim'+data.dimvalue+'.png';
					}
					else
					{
						ArrayDeviceDimvalue[data.id]=data.dimvalue;
						document.getElementById('switch_'+data.id).src = 'media/on.png';
					}
				}
				
				else {
				
				ArrayDeviceDimvalue[data.id]=data.dimvalue;
				document.getElementById('switch_'+data.id).src = 'media/off.png';
				
				}
			   
			  
			   
			   
			});
		});

}