# -*- coding: utf-8 -*-
"""
Created on Thu Nov 22 10:27:16 2012

@author: Mikel Emaldi, Aitor Almeida


The server accepts 2 type of commands:
    - INSERT: is sent when the cooker starts eating, after the food to be cooked
      and the number of people is known.
        - Format: INSERT@latitude@longitude@cookerName@timestampUnixTime@numPersons@food
        - Example: INSERT@43.29564@-2.99729@megacooker@1234567890@4@rice
    - UPDATE: the cooker sends periodic updates with sensor data. 2 posible states: cooking and stop
        - Format: UPDATE@megacooker@luminosity@extenalTemp@internalTemp@timestapUnixTime@state
        - Example: UPDATE@megacooker@134@23.0@70.3@2012-11-30@cooking

"""

import SocketServer
import re
import urllib
from xml.dom import minidom

verbose = True

HOST = "localhost"
PORT = 9999
GEONAMES_USERNAME = 'rhokbilbao'
VIRTUOSO_SERVER = 'http://helheim.deusto.es:8890/sparql?%s'

        
# gets the nearest place using geonames
def getNearestPlace(lat, lng):
       
    params = urllib.urlencode({'lat': lat, 'lng' : lng, 'username': GEONAMES_USERNAME})
    response = urllib.urlopen('http://api.geonames.org/extendedFindNearby?%s' % params)
    
    # the only method in geonames API that does not support JSON :-/
    xmldoc = minidom.parseString(response.read())
    itemlist = xmldoc.getElementsByTagName('geonameId')
    
    # Nearest place. Places in geonames are ordered from the most general to the
    # most specific, thats why we take the last one.
    place = itemlist[len(itemlist)-1].childNodes[0].data 
    if verbose:
        print "Nearest place: " + place

    return place
    
def insertCooker(data):
    if verbose:
        print "Executing insert..."
        
    command, latitude, longitude, name, timestamp, persons, menu = re.split("@", data)
    nearplace = getNearestPlace(latitude,longitude)
                
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
                                                            :timestamp %(TIMESTAMP_HOLDER)s .
                                        
      <%(COOKER_ID_HOLDER)s/location/%(TIMESTAMP_HOLDER)s>   a               :Location;
                                                            :latitude           "%(LATITUDE_HOLDER)s"^^<http://www.w3.org/2001/XMLSchema#float> ;
                                                            :longitude          "%(LONGITUDE_HOLDER)s"^^<http://www.w3.org/2001/XMLSchema#float> ;
                                                            :nearTo             <http://www.geonames.org/%(NEAR_TO_HOLDER)s/about.rdf> ;
                                                            :timestamp          %(TIMESTAMP_HOLDER)s .
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

    sendToVirtuoso(rdfStrCooker)
    
def updateData(data):
    if verbose:
        print "Executing update..."
        
    command, name, luminosity, external_temp, internal_temp, timestamp, status = re.split("@", data)
    
    updateTemplate = '''PREFIX dc: <http://purl.org/dc/elements/1.1/>
    PREFIX : <http://www.morelab.deusto.es/agronautasSimple.owl#>

    INSERT DATA INTO <http://agronautas>
    { <%(COOKER_ID_HOLDER)s/%(TIMESTAMP_HOLDER)s/measure>   a :Measure ;
                                                            :externalTemperature   "%(TEMP_EXTERNAL_HOLDER)s"^^<http://www.w3.org/2001/XMLSchema#float> ;
                                                            :internalTemperature   "%(TEMP_INTERNAL_HOLDER)s"^^<http://www.w3.org/2001/XMLSchema#float> ;
                                                            :luminosity            "%(LUMINOSITY_HOLDER)s"^^<http://www.w3.org/2001/XMLSchema#float> ;
                                                            :timestamp             %(TIMESTAMP_HOLDER)s ;
                                                            :status                "%(STATUS_HOLDER)s"^^<http://www.w3.org/2001/XMLSchema#string> .
      <%(COOKER_ID_HOLDER)s>                                :hasMeasure            <%(COOKER_ID_HOLDER)s/%(TIMESTAMP_HOLDER)s/measure> .
    }
    
    '''
    
    rdfStrCooker = updateTemplate % {
        "COOKER_ID_HOLDER" : "cooker" + name,
        "TIMESTAMP_HOLDER": timestamp,
        "TEMP_EXTERNAL_HOLDER": external_temp,
        "TEMP_INTERNAL_HOLDER": internal_temp,
        "LUMINOSITY_HOLDER": luminosity,
        "STATUS_HOLDER": status
    }
    
    if verbose:
        print rdfStrCooker
    
    sendToVirtuoso(rdfStrCooker)
    
def sendToVirtuoso(rdfToStore):
    params = urllib.urlencode({'query': rdfToStore})
    opener = urllib.urlopen(VIRTUOSO_SERVER, params)
    print opener.read()

class MyTCPHandler(SocketServer.BaseRequestHandler): 
    
     def handle(self):
        self.data = self.request.recv(1024).strip()
        
        if verbose:        
            import threading
            print threading.currentThread()
            print "{} wrote:".format(self.client_address[0])
            print self.data
        
        if self.data.startswith('INSERT'):
            insertCooker(self.data)        
        elif self.data.startswith('UPDATE'):
            updateData(self.data)          

if __name__ == "__main__":
    
    print "server running..."
    class ReuseServer(SocketServer.ThreadingTCPServer):
        allow_reuse_address = True
        
    server = ReuseServer((HOST, PORT), MyTCPHandler)

    server.serve_forever()
