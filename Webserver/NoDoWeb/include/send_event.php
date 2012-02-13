	
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
		 //Apop nog van toepassing via de webapp?
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
