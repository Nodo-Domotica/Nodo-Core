var ValueTimer;
var SliderTimer;
var SliderIndex;

var ArrValueID=new Array();
var ArrValueSuffix=new Array();
var ArrValueHours=new Array();
var ArrValueBar=new Array();
var ArrValueType=new Array();
var ArrTicksize=new Array();
var ArrValueDisplay=new Array();
var ArrValueInputoutput=new Array();
var ArrValuePar1=new Array();
var ArrLineColor=new Array();

var GraphTimer = new Array();


$('#values_page').on('pageinit', function(event) {
	checkSession();
	getValues();
});

$('#values_page').on('pageshow', function (event) {

	pagetitle = 'Values';

	checkSession();
	//clearInterval(ValueTimer);
	//Eerste maal de functie getValueState() opstarten zodat de loop gaat lopen welke de waarde elke x seconde ververst. 
	getValueState();
	ValueTimer=setInterval(function(){getValueState()},5000);
	
	$('#header_values').append('<div id="nodostate">'+pagetitle+'</div>');
	Nodo_State();


});

 $('#values_page').on('pagehide', function(event) {
		
	clearInterval(ValueTimer);
	$('#header_values').empty();
	
}); 

function getValues() {
	$.ajaxSetup({ cache: false });
	$.getJSON('webservice/json_values.php', function(data) {
		values = data.values;
		$('#values').empty();
		$.each(values, function(index, value) {
			
			ArrValueID[index]=value.id;
			ArrValueSuffix[index]=value.suffix;
			ArrValueHours[index]=value.hours;
			ArrValueType[index]=value.type;
			ArrValuePar1[index]=value.par1;
			ArrLineColor[index]=value.linecolor;
						
			//Lijn Grafiek
			if (value.type == 1) {ArrValueBar[index]=0;}
			
			//Staaf Grafiek
			if (value.type == 2) {ArrValueBar[index]=1;}
			
			ArrTicksize[index]=value.ticksize;
			ArrValueDisplay[index]=value.display;
			ArrValueInputoutput[index]=value.inputoutput;
			
			//Hoofd items
			if (value.collapsed == 1) {
				var ValueHtml = '<div data-role="collapsible" data-collapsedid='+ index +' class="value" data-collapsed-icon="plus" data-collapsed="false" data-expanded-icon="minus" data-iconpos="right" data-inset="false">';
			}
			else {
				var ValueHtml = '<div data-role="collapsible" data-collapsedid='+ index +' class="value" data-collapsed-icon="plus" data-collapsed="true" data-expanded-icon="minus" data-iconpos="right" data-inset="false">';
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
	
	
	var collapsedid = $(this).data("collapsedid");
	var sensor_id = ArrValueID[collapsedid];
	var suffix = ArrValueSuffix[collapsedid];
	var hours = ArrValueHours[collapsedid];
	var bars = ArrValueBar[collapsedid];
	var linecolor = ArrLineColor[collapsedid];
	
	if (bars == 1){
		var ticksize = 'day';
	}
	else {
		var ticksize = 'minute';
	}
	
	var display = ArrValueDisplay[collapsedid];
	var inputoutput = ArrValueInputoutput[collapsedid];
	//alert (bars);
		
	if (display == 1 && inputoutput == 2) { //alleen grafiek gegevens ophalen indien het een value is en een output betreft
		
		Get_Graph_data(hours,sensor_id,suffix,bars,ticksize,linecolor);
				
		GraphTimer[collapsedid]=setInterval(function(){Get_Graph_data(hours,sensor_id,suffix,bars,ticksize,linecolor)},30000+Math.floor(Math.random() * 10000));	
		
	}
});

$('div.value').live('collapse', function(){

	var collapsedid = $(this).data("collapsedid");
	clearInterval(GraphTimer[collapsedid]);		

});

function Get_Graph_data(hours,sensor_id,label,bars,ticksize,linecolor,date1,date2,filter) {  
     
		if (linecolor =='') {
					
				//Standaard lijn kleuren
				switch(theme)
				 {
				 case 'a':
				  linecolor = "#FF0000"; //rood
				  break;
				 case 'b':
				  linecolor = "#2065E6"; //blauw
				  break;
				 case 'c':
				  linecolor = "black"; 
				  break;
				 case 'd':
				  linecolor = "black"; 
				   break;
				 case 'e':
					linecolor = "#FFDF0F"; //geel
				   break;
				 default:
				   linecolor = "black"; 
				 }
			}
	 
	 
			var options = {
						xaxis: { mode: "time",minTickSize: [1,ticksize]},
						legend: { backgroundOpacity: 0 },
						bars: {lineWidth: 2},
						lines: {lineWidth: 2,fill: true},
						shadowSize: 3
						
						
			};
					
			element = document.getElementById('graph_'+sensor_id);	
		
					
		$.getJSON("webservice/json_graph_data.php?hours=" + hours +"&sensor_id="+sensor_id +"&bars="+bars +"&date1="+date1 +"&date2="+date2+"&filter="+filter, function(graph_data) {
              
			if (graph_data != null) {
		   
				var plotarea = $("#graph_" + sensor_id);
		   
					if (bars != 1) {
						$.plot(plotarea , [
								{ label: label, data: graph_data, color: linecolor }
								],options );
					}
					else {
					   
					   $.plot(plotarea , [
								 { label: label, data: graph_data, color: linecolor ,bars: {show: true, barWidth:43200000, align: "center",} }
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
	send_event('variableset '+ArrValuePar1[index]+',' + val,'value')
}





