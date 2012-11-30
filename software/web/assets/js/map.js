var map;
var markers;

function init_map() {
        map = new OpenLayers.Map("basicMap");
        var mapnik         = new OpenLayers.Layer.OSM();
        //var mapnik         = new OpenLayers.Layer.WMS("NASA Global Mosaic",
        //                           "http://wms.jpl.nasa.gov/wms.cgi",
        //                           {layers: "modis,global_mosaic"});
        var fromProjection = new OpenLayers.Projection("EPSG:4326");   // Transform from WGS 1984
        var toProjection   = new OpenLayers.Projection("EPSG:900913"); // to Spherical Mercator Projection
        var position       = new OpenLayers.LonLat(-2.935581,43.263713).transform( fromProjection, toProjection);
        var zoom           = 15; 
        map.addLayer(mapnik);
        map.setCenter(position, zoom );
}

function printCookersOnMap(data){

  console.log("drawing markers...");
  markers = new OpenLayers.Layer.Markers( "Markers" );
  map.addLayer(markers);

  var size = new OpenLayers.Size(21,25);
  var offset = new OpenLayers.Pixel(-(size.w/2), -size.h);
  var icon = new OpenLayers.Icon('http://www.openlayers.org/dev/img/marker.png', size, offset);
 
  for (var i=0;i<data.length;i++)
  { 
     markers.addMarker(new OpenLayers.Marker(new OpenLayers.LonLat(data[i].long.value,data[i].lat.value),icon.clone()));
  }

}