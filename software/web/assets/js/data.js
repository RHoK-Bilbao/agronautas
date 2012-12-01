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
        url:'http://maps.googleapis.com/maps/api/geocode/json?latlng=39.76144296429947,-104.8011589050293&sensor=false&callback=?', 
        dataType:'jsonp', 
        success:function(data) {
            console.log("------");
            console.log(parent_data);
            var prev = $('#'+parent_data.id.value).html();
            $('#'+parent_data.id.value).html(prev + "<table>\
                      <tbody>\
                          <tr>\
                              <td class=\"field\">ID</td>\
                              <td>"+parent_data.id.value+"</td>\
                          </tr>\
                          <tr>\
                              <td class=\"field\">Model</td>\
                              <td>Normal</td>\
                          </tr>\
                          <tr>\
                              <td class=\"field\">City</td>\
                              <td>Unknown</td>\
                          </tr>\
                          <tr>\
                              <td class=\"field\">Location</td>\
                              <td>"+parent_data.lat.value+","+parent_data.long.value+"</td>\
                          </tr>\
                      </tbody>\
                  </table>");
            refresfCookerExpandables();
            main_list_todo = false;
        },
        error:function(data) {
            console.log("------");
            console.log(parent_data);
            var prev = $('#'+parent_data.id.value).html();
            $('#'+parent_data.id.value).html(prev + "<table>\
                      <tbody>\
                          <tr>\
                              <td class=\"field\">ID</td>\
                              <td>"+parent_data.id.value+"</td>\
                          </tr>\
                          <tr>\
                              <td class=\"field\">Model</td>\
                              <td>Normal</td>\
                          </tr>\
                          <tr>\
                              <td class=\"field\">City</td>\
                              <td>Unknown</td>\
                          </tr>\
                          <tr>\
                              <td class=\"field\">Location</td>\
                              <td>"+parent_data.lat.value+","+parent_data.long.value+"</td>\
                          </tr>\
                      </tbody>\
                  </table>");
            refresfCookerExpandables();
            main_list_todo = false;
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