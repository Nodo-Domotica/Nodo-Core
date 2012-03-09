function Get_Nodo_Status()
 {  
   
			
				var
					$http,
					$self = arguments.callee,
					element = document.getElementById('status_div');
					
					element.innerHTML = '<h4><img src="/media/loading.gif"/> Please wait, loading status...</h4>'; 
					
				if (window.XMLHttpRequest) {
					$http = new XMLHttpRequest();
				} else if (window.ActiveXObject) {
					try {
						$http = new ActiveXObject('Msxml2.XMLHTTP');
					} catch(e) {
						$http = new ActiveXObject('Microsoft.XMLHTTP');
					}
				}

				if ($http) {
					$http.onreadystatechange = function()
					{
						if (/4|^complete$/.test($http.readyState)) {
														
							element.innerHTML = $http.responseText;  
						}
					};
					$http.open('GET', 'get_nodo_status.php' + '?' + new Date().getTime(), true);
					$http.send(null);
				}

			}




 


 
