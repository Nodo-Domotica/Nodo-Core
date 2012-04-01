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

function Get_Values()
//Lees waarden uit
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
							eval($http.responseText);
							
							setTimeout(function(){$self();}, 2000);
						}
					};
					$http.open('GET', 'get_values.php' + '?' + new Date().getTime(), true);
					$http.send(null);
				}

			}




 
