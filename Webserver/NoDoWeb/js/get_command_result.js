function Get_Command_Result(command,defcommand)
 {  
   
			
				var
					$http,
					$self = arguments.callee,
					element = document.getElementById('result_div');
					
					element.innerHTML = '<img src="../media/loading.gif"/> Please wait, loading results...</h4>'; 
					
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
					$http.open('GET', 'command_result.php' + '?command=' + command + '&defcommand=' + defcommand + '&' + new Date().getTime(), true);
					$http.send(null);
				}

			}




 


 
