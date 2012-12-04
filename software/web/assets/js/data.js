function getCookers(){
  	$.ajax(
	  	{
	  		url:'http://helheim.deusto.es:8890/sparql?default-graph-uri=http%3A%2F%2Fagronautas&query=SELECT+distinct+%3Fid+%3Flat+%3Flong+MAX%28%3Ftimestamp%29+as+%3Ftimestamp+WHERE+%7B+%3Fid+a+%3Chttp%3A%2F%2Fwww.morelab.deusto.es%2FagronautasSimple.owl%23SolarCooker%3E+.%0D%0A%3Fid+%3Chttp%3A%2F%2Fwww.morelab.deusto.es%2FagronautasSimple.owl%23hasLocation%3E+%3Flocation+.%0D%0A%3Flocation+%3Chttp%3A%2F%2Fwww.morelab.deusto.es%2FagronautasSimple.owl%23timestamp%3E+%3Ftimestamp+.%0D%0A%3Flocation+%3Chttp%3A%2F%2Fwww.morelab.deusto.es%2FagronautasSimple.owl%23latitude%3E+%3Flat+.%0D%0A%3Flocation+%3Chttp%3A%2F%2Fwww.morelab.deusto.es%2FagronautasSimple.owl%23longitude%3E+%3Flong+.%7D&should-sponge=&format=json', 
	  		dataType:'jsonp', 
	  		success:function(data) {
	  			console.log(data.results.bindings);
	  			printCookersOnMap(data.results.bindings);
	  			printCookersStats(data.results.bindings);
	  		}
	  	}
  	);
}

//fucntion getCooker

function getLocationInfo(parent_data){
  $.ajax(
      {
        url:'http://api.geonames.org/findNearbyPlaceNameJSON?lat='+parent_data.lat.value+'&lng='+parent_data.long.value+'&username=rhokbilbao', 
        dataType:'jsonp', 
        success:function(data) {
            console.log(data);
            drawCookerInfoResume(parent_data.id.value, parent_data.lat.value, parent_data.long.value,data.geonames[0].name);
        },
        error:function(data) {
            drawCookerInfoResume(parent_data.id.value, parent_data.lat.value, parent_data.long.value,'unknown');
        }
      }
  );
  
}

function printCookersStats(data){
  var new_html = "";
  for (var i=0;i<data.length;i++)
    { 
      $('#list-scrollable').html($('#list-scrollable').html() + "<p class=\"heading\">"+data[i].id.value+"</p><div class=\"content\"><div id=\""+data[i].id.value+"\" class=\"summary\"><img src=\"assets/imgs/coc_demo.png\"></div></div>");
      getLocationInfo(data[i]);
    }
}