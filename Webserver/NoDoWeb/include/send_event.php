<?php
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
?>	
	<script type="text/javascript">
		
		function send_event(event)
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

		<?php
			
		switch ($send_method) {
		 
		 case 1:
		 ?>  
						$http.open('GET', 'forward_apop.php' + '?event=' + event + '&unique=' + new Date().getTime(), true);
		<?php   
		 break;
		 case 2:
		 ?>
		 
						$http.open('GET', 'forward_http.php' + '?event=' + event + '&unique=' + new Date().getTime(), true);
		<?php
		   break;
		 
		 } 
		?>				
						$http.send(null);
						}

					}


	</script>
