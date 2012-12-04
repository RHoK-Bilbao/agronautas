var info_open = false;
var main_open = false;
var main_list_todo = true;

function init_boxes(){
	jQuery("#box_content").hide();
	jQuery("#info_box_content").hide();
	refresfCookerExpandables();
	
	  //toggle the componenet with class msg_body

}

$("#box-header").click(function() {
  if(info_open){
   $("#info_box_content").animate({width: 'toggle'}, 'slow');
   info_open = false;
  }
  $("#box_content").animate({width: 'toggle'}, 'slow');
  if(main_open){
  	main_open = false;
  }else{
  	main_open = true;
  }
});

$("#info-box-header").click(function() {
  if(main_open){
   $("#box_content").animate({width: 'toggle'}, 'slow');
   main_open = false;
  }
  $("#info_box_content").animate({width: 'toggle'}, 'slow');
  if(info_open){
  	info_open = false;
  }else{
  	info_open = true;
  }
});

function refresfCookerExpandables(){
	resizeElementHeight();
	/*if(main_list_todo)
		jQuery(".content").hide();
		main_list_todo
	}*/
	jQuery(".content").hide();
	if(main_list_todo){
		console.log('loading_spandable.....');
		jQuery(".heading").click(function()
		{
		    jQuery(this).next(".content").slideToggle('slow');
		    console.log("expanding...");
		});
	}
}

function drawCookerInfoResume(id, latitude, longitude, city){
  var prev = $('#'+id).html();
  $('#'+id).html(prev + "<table>\
                      <tbody>\
                          <tr>\
                              <td class=\"field\">ID</td>\
                              <td>"+id+"</td>\
                          </tr>\
                          <tr>\
                              <td class=\"field\">Model</td>\
                              <td>Normal</td>\
                          </tr>\
                          <tr>\
                              <td class=\"field\">City</td>\
                              <td>"+city+"</td>\
                          </tr>\
                          <tr>\
                              <td class=\"field\">Location</td>\
                              <td>"+latitude+","+longitude+"</td>\
                          </tr>\
                      </tbody>\
                  </table>\
                  <table cellspacing=\"5\">\
                      <tbody>\
                          <tr>\
                              <td class=\"info_box\">\
                                  9\
                                  <span>meals</span>\
                              </td>\
                              <td class=\"info_box\">\
                                  9\
                                  <span>W generated</span>\
                              </td>\
                              <td class=\"info_box\">\
                                  9\
                                  <span>W potentially saved</span>\
                              </td>\
                              <td class=\"info_box\">\
                                  9\
                                  <span>hours</span>\
                              </td>\
                          </tr>\
                      </tbody>\
                  </table>");
    refresfCookerExpandables();
    main_list_todo = false;

}

