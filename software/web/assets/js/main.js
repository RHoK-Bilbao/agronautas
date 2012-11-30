
$(document).ready(function() {
	//google.load("visualization", "1", {packages:["corechart"]});
    //google.setOnLoadCallback(init_main_all); 
    init_main_all();
    getCookers();
    
});

function init_main_all(){
	   	resizeElementHeight();
 	    init_boxes();
   		init_map();

   		//drawChart();
 }

$(window).resize(function() {
  resizeElementHeight();
});

 function resizeElementHeight() {
  var height = 0;
  var body = window.document.body;
  if (window.innerHeight) {
      height = window.innerHeight;
  } else if (body.parentElement.clientHeight) {
      height = body.parentElement.clientHeight;
  } else if (body && body.clientHeight) {
      height = body.clientHeight;
  }
  element = document.getElementById('list-scrollable');
  element.style.height = ((height - element.offsetTop) + "px");
}
