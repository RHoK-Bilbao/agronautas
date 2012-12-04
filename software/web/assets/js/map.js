var map;
var markers;

var fromProjection = new OpenLayers.Projection("EPSG:4326");   // Transform from WGS 1984
var toProjection   = new OpenLayers.Projection("EPSG:900913"); // to Spherical Mercator Projection


function init_map() {
        var streets = [
                "http://a.tiles.mapbox.com/v3/adrelanex.map-aq9bz8ek/${z}/${x}/${y}.png",
                "http://b.tiles.mapbox.com/v3/adrelanex.map-aq9bz8ek/${z}/${x}/${y}.png",
                "http://c.tiles.mapbox.com/v3/adrelanex.map-aq9bz8ek/${z}/${x}/${y}.png",
                "http://d.tiles.mapbox.com/v3/adrelanex.map-aq9bz8ek/${z}/${x}/${y}.png"
            ];


        map = new OpenLayers.Map("basicMap");
        var mapnik         = new OpenLayers.Layer.OSM("MapBox",streets);
        //var mapnik         = new OpenLayers.Layer.WMS("NASA Global Mosaic",
        //                           "http://wms.jpl.nasa.gov/wms.cgi",
        //                           {layers: "modis,global_mosaic"});
        
        var position       = new OpenLayers.LonLat(-2.935581,43.263713).transform( fromProjection, toProjection);
        var zoom           = 15; 
        map.addLayer(mapnik);
        map.setCenter(position, zoom );
}

function printCookersOnMap(data){

  console.log("drawing markers...");
  markers = new OpenLayers.Layer.Markers( "Markers" );
  map.addLayer(markers);

  var size = new OpenLayers.Size(25,25);
  var offset = new OpenLayers.Pixel(-(size.w/2), -(size.h/2));
  var icon = new OpenLayers.Icon('assets/imgs/marker.png', size, offset);
 
  var lastposition;
  for (var i=0;i<data.length;i++)
  { 
     //console.log(new OpenLayers.Marker(new OpenLayers.LonLat(data[i].long.value,data[i].lat.value),icon.clone()));
     markers.addMarker(new OpenLayers.Marker(new OpenLayers.LonLat(data[i].long.value,data[i].lat.value).transform( fromProjection, toProjection),icon.clone()));
     lastposition = new OpenLayers.LonLat(data[i].long.value,data[i].lat.value).transform( fromProjection, toProjection);
  }
  console.log(lastposition);
  map.setCenter(lastposition, 15 );

}