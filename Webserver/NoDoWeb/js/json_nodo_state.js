Nodo_State();
NodoStateTimer=setInterval(function(){Nodo_State()},3000);

function Nodo_State(){

var url='/webapp/webservice/json_nodo_state.php';



var status;
	
		$.getJSON(url,function(data){
		NodoState = data.nodostate;
			$.each(NodoState, function(index,state){
			 
				if (state.busy == 1) {
					
					
						document.getElementById('nodostate_devices').innerHTML = '<FONT COLOR=\"yellow\">Nodo busy!</FONT>';
						document.getElementById('nodostate_activities').innerHTML = '<FONT COLOR=\"yellow\">Nodo busy!</FONT>';
						document.getElementById('nodostate_values').innerHTML = '<FONT COLOR=\"yellow\">Nodo busy!</FONT>';
						document.getElementById('nodostate_info').innerHTML = '<FONT COLOR=\"yellow\">Nodo busy!</FONT>';
						
						
					}
					
				if (state.online == 0) {
					
					
						document.getElementById('nodostate_devices').innerHTML = '<FONT COLOR=\"red\">No connection to Nodo!</FONT>';
						document.getElementById('nodostate_activities').innerHTML = '<FONT COLOR=\"red\">No connection to Nodo!</FONT>';
						document.getElementById('nodostate_values').innerHTML = '<FONT COLOR=\"red\">No connection to Nodo!</FONT>';
						document.getElementById('nodostate_info').innerHTML = '<FONT COLOR=\"red\">No connection to Nodo!</FONT>';
					}
					
				if (state.busy != 1 && state.online != 0) {
					
					
						document.getElementById('nodostate_devices').innerHTML = '';
						document.getElementById('nodostate_activities').innerHTML = '';
						document.getElementById('nodostate_values').innerHTML = '';
						document.getElementById('nodostate_info').innerHTML = '';
						
					}
				
				
			      
			});
		});

}