# -*- coding: utf-8 -*-
"""
Created on Thu Nov 22 10:27:16 2012

@author: Aitor
"""

import SocketServer
import re
import urllib
import StringIO
from xml.dom import minidom
from rdflib.graph import Graph

"""
Expected msg format: "lat@long@name@luminosity@externalTemp@internalTemp"
"""

verbose = True

HOST = "localhost"
PORT = 9999

# RDF template    
# TODO meter cooker id como URL                                       
rdfTemplateCookerUpdate = """
<?xml version="1.0"?>
                    
<rdf:RDF
xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
xmlns:protege="http://protege.stanford.edu/plugins/owl/protege#"
xmlns:xsp="http://www.owl-ontologies.com/2005/08/07/xsp.owl#"
xmlns:j.0="http://xmlns.com/foaf/0.1/"
xmlns:owl="http://www.w3.org/2002/07/owl#"
xmlns:j.1="http://geovocab.org/spatial#"
xmlns:xsd="http://www.w3.org/2001/XMLSchema#"
xmlns:swrl="http://www.w3.org/2003/11/swrl#"
xmlns:swrlb="http://www.w3.org/2003/11/swrlb#"
xmlns="http://www.morelab.deusto.es/agronautasSimple.owl#"
xmlns:rdfs="http://www.w3.org/2000/01/rdf-schema#"
xml:base="http://www.morelab.deusto.es/agronautasSimple.owl">

<SolarCooker rdf:ID="%(COOKER_ID_HOLDER)s">
    <name rdf:datatype="http://www.w3.org/2001/XMLSchema#string">%(NAME_HOLDER)s</name>
    <hasLocation>
        <Location rdf:ID="%(LOCATION_ID_HOLDER)s">
            <latitude rdf:datatype="http://www.w3.org/2001/XMLSchema#float">%(LATITUDE_HOLDER)s</latitude>
            <longitude rdf:datatype="http://www.w3.org/2001/XMLSchema#float">%(LONGITUDE_HOLDER)s</longitude>
            <nearTo rdf:resource="http://www.geonames.org/%(NEAR_TO_HOLDER)s/about.rdf"/>
        </Location>
    </hasLocation>
    <luminosity rdf:datatype="http://www.w3.org/2001/XMLSchema#float">%(LUMINOSITY_HOLDER)s</luminosity>
    <externalTemperature rdf:datatype="http://www.w3.org/2001/XMLSchema#float">%(TEMP_EXTERNAL_HOLDER)s</externalTemperature>
    <internalTemperature rdf:datatype="http://www.w3.org/2001/XMLSchema#float">%(TEMP_INTERNAL_HOLDER)s</internalTemperature>
</SolarCooker>
                        
</rdf:RDF>
"""

rdfTemplateCookerInsert = """<?xml version="1.0"?>
                    
<rdf:RDF
xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
xmlns:protege="http://protege.stanford.edu/plugins/owl/protege#"
xmlns:xsp="http://www.owl-ontologies.com/2005/08/07/xsp.owl#"
xmlns:j.0="http://xmlns.com/foaf/0.1/"
xmlns:owl="http://www.w3.org/2002/07/owl#"
xmlns:j.1="http://geovocab.org/spatial#"
xmlns:xsd="http://www.w3.org/2001/XMLSchema#"
xmlns:swrl="http://www.w3.org/2003/11/swrl#"
xmlns:swrlb="http://www.w3.org/2003/11/swrlb#"
xmlns="http://www.morelab.deusto.es/agronautasSimple.owl#"
xmlns:rdfs="http://www.w3.org/2000/01/rdf-schema#"
xml:base="http://www.morelab.deusto.es/agronautasSimple.owl">

<SolarCooker rdf:ID="%(COOKER_ID_HOLDER)s">
    <name rdf:datatype="http://www.w3.org/2001/XMLSchema#string">%(NAME_HOLDER)s</name>
    <hasLocation>
        <Location rdf:ID="%(LOCATION_ID_HOLDER)s">
            <latitude rdf:datatype="http://www.w3.org/2001/XMLSchema#float">%(LATITUDE_HOLDER)s</latitude>
            <longitude rdf:datatype="http://www.w3.org/2001/XMLSchema#float">%(LONGITUDE_HOLDER)s</longitude>
            <nearTo rdf:resource="http://www.geonames.org/%(NEAR_TO_HOLDER)s/about.rdf"/>
        </Location>
    </hasLocation>
</SolarCooker>
                        
</rdf:RDF>
"""
        
def getNearerPlace(lat, lng):
       
    params = urllib.urlencode({'lat': lat, 'lng' : lng, 'username': 'rhokbilbao'})
    response = urllib.urlopen('http://api.geonames.org/extendedFindNearby?%s' % params)
    
    xmldoc = minidom.parseString(response.read())
    itemlist = xmldoc.getElementsByTagName('geonameId')

    return itemlist[len(itemlist)-1].childNodes[0].data
    
def insertCooker(data):
    command, latitude, longitude, name, timestamp, persons, menu = re.split("@", data)
    nearplace = getNearerPlace(latitude,longitude)
                
    insertTemplate = '''PREFIX dc: <http://purl.org/dc/elements/1.1/>
    PREFIX : <http://www.morelab.deusto.es/agronautasSimple.owl#>

    INSERT DATA INTO <http://agronautas>
    { <%(COOKER_ID_HOLDER)s>            a                   :SolarCooker ;
                                                            :name           "%(NAME_HOLDER)s"^^<http://www.w3.org/2001/XMLSchema#string> ;
                                                            :hasLocation    <%(COOKER_ID_HOLDER)s/location/%(TIMESTAMP_HOLDER)s> ;
                                                            :cookedMenus    <%(COOKER_ID_HOLDER)s/cookedmenu/%(TIMESTAMP_HOLDER)s> .
      <%(COOKER_ID_HOLDER)s/cookedmenu/%(TIMESTAMP_HOLDER)s> a   :Cooker ;
                                                            :menu "%(MENU_HOLDER)s" ;
                                                            :persons "%(PERSON_HOLDER)s" ;
                                                            :timestamp "%(TIMESTAMP_HOLDER)s" .
                                        
      <%(COOKER_ID_HOLDER)s/location/%(TIMESTAMP_HOLDER)s>   a               :Location;
                                                            :latitude           "%(LATITUDE_HOLDER)s"^^<http://www.w3.org/2001/XMLSchema#float> ;
                                                            :longitude          "%(LONGITUDE_HOLDER)s"^^<http://www.w3.org/2001/XMLSchema#float> ;
                                                            :nearTo             <http://www.geonames.org/%(NEAR_TO_HOLDER)s/about.rdf> ;
                                                            :timestamp          "%(TIMESTAMP_HOLDER)s"^^<http://www.w3.org/2001/XMLSchema#string> .
    }
    
    '''
    
    rdfStrCooker = insertTemplate % {
        "LOCATION_ID_HOLDER" : "location" + name,
        "LATITUDE_HOLDER" : latitude,
        "LONGITUDE_HOLDER" : longitude,
        "NEAR_TO_HOLDER" : nearplace, 
        "COOKER_ID_HOLDER" : "cooker" + name,
        "NAME_HOLDER" : name,
        "TIMESTAMP_HOLDER": timestamp,
        "PERSON_HOLDER": persons,
        "MENU_HOLDER": menu
    }
    
    if verbose:
        print rdfStrCooker

    params = urllib.urlencode({'query': rdfStrCooker})
    opener = urllib.urlopen('http://helheim.deusto.es:8890/sparql?%s', params)
    print opener.read()
    
    return rdfStrCooker  
    
def updateData(data):
    
    command, name, luminosity, external_temp, internal_temp, timestamp = re.split("@", data)
    
    insertTemplate = '''PREFIX dc: <http://purl.org/dc/elements/1.1/>
    PREFIX : <http://www.morelab.deusto.es/agronautasSimple.owl#>

    INSERT DATA INTO <http://agronautas>
    { <%(COOKER_ID_HOLDER)s/%(TIMESTAMP_HOLDER)s/measure>    a :Measure
                                                            :externalTemperature   "%(TEMP_EXTERNAL_HOLDER)s"^^<http://www.w3.org/2001/XMLSchema#float> ;
                                                            :internalTemperature   "%(TEMP_INTERNAL_HOLDER)s"^^<http://www.w3.org/2001/XMLSchema#float> ;
                                                            :luminosity            "%(LUMINOSITY_HOLDER)s"^^<http://www.w3.org/2001/XMLSchema#float> ;
                                                            :timestamp             "%(TIMESTAMP_HOLDER)s"^^<http://www.w3.org/2001/XMLSchema#string> .
      <%(COOKER_ID_HOLDER)s>                                :hasMeasure            <%(COOKER_ID_HOLDER)s/%(TIMESTAMP_HOLDER)s/measure> .
    }
    
    '''
    
    rdfStrCooker = insertTemplate % {
        "COOKER_ID_HOLDER" : "cooker" + name,
        "TIMESTAMP_HOLDER": timestamp,
        "TEMP_EXTERNAL_HOLDER": external_temp,
        "TEMP_INTERNAL_HOLDER": internal_temp,
        "LUMINOSITY_HOLDER": luminosity
    }
    
    print rdfStrCooker
    
    params = urllib.urlencode({'query': rdfStrCooker})
    opener = urllib.urlopen('http://helheim.deusto.es:8890/sparql?%s', params)
    
    return rdfStrCooker

class MyTCPHandler(SocketServer.BaseRequestHandler): 
    
     def handle(self):
        self.data = self.request.recv(1024).strip()
        
        if self.data.startswith('INSERT'):
            insertCooker(self.data)        
        elif self.data.startswith('UPDATE'):
            updateData(self.data)
        
        if verbose:
            import threading
            print threading.currentThread()
            print "{} wrote:".format(self.client_address[0])
            print self.data
        
       # rdf = convertToRDF(self.data)        
        
        # TODO: send the rdf info to virtuoso
        # self.request.sendall(rdf)    


if __name__ == "__main__":
    class ReuseServer(SocketServer.ThreadingTCPServer):
        allow_reuse_address = True
        
    server = ReuseServer((HOST, PORT), MyTCPHandler)

    server.serve_forever()
