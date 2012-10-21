var pagetitle;

NodoStateTimer=setInterval(function(){Nodo_State()},3000);

function Nodo_State(){

var url='/webapp/webservice/json_nodo_state.php';



var status;
	
		$.getJSON(url,function(data){
		NodoState = data.nodostate;
			$.each(NodoState, function(index,state){
			 
				if (state.busy == 1) {
					
					
						document.getElementById('nodostate').innerHTML = '<FONT COLOR=\"yellow\">Nodo busy!</FONT>';
						
						
						
					}
					
				if (state.online == 0) {
					
					
						document.getElementById('nodostate').innerHTML = '<FONT COLOR=\"red\">No connection to Nodo!</FONT>';
						
					}
					
				if (state.busy != 1 && state.online != 0) {
					
											
						document.getElementById('nodostate').innerHTML = pagetitle;
						
					}
				
				
			      
			});
		});

}