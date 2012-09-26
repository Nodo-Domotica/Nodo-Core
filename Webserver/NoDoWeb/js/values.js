var ValueTimer;
var SliderTimer;
var SliderIndex;

var ArrayValueID=new Array();
var ArrayValueSuffix=new Array();
var ArrayValueHours=new Array();
var ArrayValueBar=new Array();
var ArrayValueType=new Array();
var ArrayValueTicksize=new Array();
var ArrayValueDisplay=new Array();
var ArrayValueInputoutput=new Array();
var ArrayValuePar1=new Array();

//Grafiek kleuren
var GraphColor;
switch(theme)
 {
 case 'a':
  GraphColor = "#808080"; //grijs
  break;
 case 'b':
  GraphColor = "#2065E6"; //blauw
  break;
 case 'c':
  GraphColor = "black"; 
  break;
 case 'd':
  GraphColor = "black"; 
   break;
 case 'e':
	GraphColor = "#FFDF0F"; //geel
   break;
 default:
   GraphColor = "black"; 
 }


$('#values_page').on('pageinit', function(event) {
	checkSession();
	getValues();

	
});

function getValues() {
	$.getJSON('webservice/json_values.php', function(data) {
		values = data.values;
		$('#values').empty();
		$.each(values, function(index, value) {
			
			ArrayValueID[index]=value.id;
			ArrayValueSuffix[index]=value.suffix;
			ArrayValueHours[index]=value.hours;
			ArrayValueType[index]=value.type;
			ArrayValuePar1[index]=value.par1;
			
			
			//Lijn Grafiek
			if (value.type == 1) {ArrayValueBar[index]=0;}
			
			//Staaf Grafiek
			if (value.type == 2) {ArrayValueBar[index]=1;}
			
			ArrayValueTicksize[index]=value.ticksize;
			ArrayValueDisplay[index]=value.display;
			ArrayValueInputoutput[index]=value.inputoutput;
			
			//Hoofd items
			if (value.collapsed == 1) {
				var ValueHtml = '<div data-role="collapsible" data-indexid='+ index +' class="value" data-collapsed-icon="plus" data-collapsed="false" data-expanded-icon="minus" data-iconpos="right" data-inset="false">';
			}
			else {
				var ValueHtml = '<div data-role="collapsible" data-indexid='+ index +' class="value" data-collapsed-icon="plus" data-collapsed="true" data-expanded-icon="minus" data-iconpos="right" data-inset="false">';
			}
			
			if (value.display == 1){ValueHtml = ValueHtml + '<h2>'+ value.prefix +' <span id="value_'+ value.id +'"><img src="media/loading.gif" WIDTH="15"></span> '+ value.suffix +'</h2>';}
			if (value.display == 2){ValueHtml = ValueHtml + '<h2>'+ value.prefix +' <span id="value_'+ value.id +'"><img src="media/loading.gif" WIDTH="15"></span></h2>';}
			
			//Alleen een grafiek weergeven als het een output vanuit de nodo richting webapp betreft en het een waarde betreft.
			if (value.inputoutput == 2 && value.display == 1) {
				ValueHtml = ValueHtml + '<div id="graph_' + value.id +'" style="width:100%;height:300px;position: relative;"></div>';
			}
		
			//+- knoppen
			if (value.inputcontrol == 1){
				ValueHtml = ValueHtml + '<a href="javascript:send_event(&quot;variableinc ' + value.par1 +','+ value.inputstep +'&quot;,&quot;value&quot;)" data-role="button">+</a>';
				ValueHtml = ValueHtml + '<a href="javascript:send_event(&quot;variabledec ' + value.par1 +','+ value.inputstep +'&quot;,&quot;value&quot;)" data-role="button">-</a>';		
			}
			
			//Slider
			if (value.inputcontrol == 2){
				ValueHtml = ValueHtml + '<label  id="distSlider-label" for="distSlider">Set value: </label>';
				ValueHtml = ValueHtml + '<input  name="distSlider" id="distSlider'+ index +'" value="'+ value.data +'" min="'+ value.inputminval +'" max="'+ value.inputmaxval +'" step="'+ value.inputstep +'" data-type="range" onChange="update_distance_value_timer('+ index +')">';
			}
			
		
			var ValueHtml = ValueHtml + '</div>';
			//console.log(value.prefix);
			$('#values').append(ValueHtml);
								
				
		});
		$('#values').trigger('create');
	});
 

}

$('div.value').live('expand', function(){
	
	
	var indexid = $(this).data("indexid");
	var sensor_id = ArrayValueID[indexid];
	var suffix = ArrayValueSuffix[indexid];
	var hours = ArrayValueHours[indexid];
	var bars = ArrayValueBar[indexid];
	
	if (bars == 1){
		var ticksize = 'day';
	}
	else {
		var ticksize = 'minute';
	}
	
	var display = ArrayValueDisplay[indexid];
	var inputoutput = ArrayValueInputoutput[indexid];
	//alert (bars);
		
	if (display == 1 && inputoutput == 2) { //alleen grafiek gegevens ophalen indien het een value is en een output betreft
		
		Get_Graph_data(hours,sensor_id,suffix,bars,ticksize);
		//eval('refreshgraph' + id + '();');
		//window['refreshgraph' + indexid]();
	}
});

function Get_Graph_data(hours,sensor_id,label,bars,ticksize,date1,date2,filter)
 {  
     
			var options = {
						xaxis: { mode: "time",minTickSize: [1,ticksize]}
					};
			
		 
		
			element = document.getElementById('graph_'+sensor_id);	
		//element.innerHTML = '<h4><img src="media/loading.gif"/> Please wait, loading graph...</h4>'; 
		//Andere oplossing voor maken

					
		$.getJSON("webservice/json_graph_data.php?hours=" + hours +"&sensor_id="+sensor_id +"&bars="+bars +"&date1="+date1 +"&date2="+date2+"&filter="+filter, function(graph_data) {
       //succes - data loaded, now use plot:
       
	   if (graph_data != null) {
	   
	   var plotarea = $("#graph_" + sensor_id);
       
       if (bars != 1)
		   {
		   
		   
		   
		   $.plot(plotarea , [
					 { label: label, data: graph_data, color: GraphColor  }
					 ],options );
		   }
		else
		   {
		   
		   
		   
		   $.plot(plotarea , [
					 { label: label, data: graph_data, color: GraphColor ,bars: {show: true, barWidth:43200000, align: "center",} }
					 ],options );
		   }
		   
		}
		
		else {
		
		
			element.innerHTML = '<h4>No data available......</h4>'; 
			
		}
	   
	
    });

}


//Value slider functie
function update_distance_value_timer(index)	{
	SliderIndex = index;
	clearTimeout(SliderTimer);
	SliderTimer=setTimeout("update_distance_value(SliderIndex)",1000);
	
}
function update_distance_value(index) {
	
	var val = $('#distSlider'+index).val();
	//alert(index);
	send_event('variableset '+ArrayValuePar1[index]+',' + val,'value')
					
}

$('#values_page').on('pageshow', function (event) {

	
	checkSession();
	clearInterval(ValueTimer);
	//Eerste maal de functie getValueState() opstarten zodat de loop gaat lopen welke de waarde elke x seconde ververst. 
	getValueState();
	ValueTimer=setInterval(function(){getValueState()},5000);

});

 $('#values_page').on('pagehide', function(event) {
	//Timer stoppen
	
	clearInterval(ValueTimer);
	//console.log('clearing');
	}); 



