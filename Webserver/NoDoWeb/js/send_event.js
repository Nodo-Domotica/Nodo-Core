/***********************************************************************************************************************
"Nodo Web App" Copyright © 2012 Martin de Graaf

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*************************************************************************************************************************/

var $nodohttpresponse;	
		
		function send_event(event,type,sync)
		//stuur opdrachten door naar de nodo
					{
						var
							$http,
							$self = arguments.callee;

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
														
							$nodohttpresponse = $http.responseText;
							
						}
					};

				 
						if (sync == 1) {
							$http.open('GET', '/webapp/webservice/forward_http.php' + '?event=' + event + '&unique=' + new Date().getTime(), false); }
						else {
							$http.open('GET', '/webapp/webservice/forward_http.php' + '?event=' + event + '&unique=' + new Date().getTime(), true); }
						
						
						$http.send(null);
							if (type == 'device'){
							//status van een device na 1 seconde ophalen
							//alert (type);
							setTimeout(function(){Device_State()},1000);
							}
							if (type == 'value'){
							//status van een device na 1 seconde ophalen
							//alert (type);
							setTimeout(function(){getValueState()},1000);
							}
							
						}
					if (sync == 1) {
						return $nodohttpresponse;
					}
					}


	
