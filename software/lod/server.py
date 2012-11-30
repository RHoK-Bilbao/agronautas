# -*- coding: utf-8 -*-
"""
Created on Thu Nov 22 10:27:16 2012

@author: Aitor
"""

import SocketServer
import re
import urllib
from xml.dom import minidom

"""
Expected msg format: "lat@long@name@luminosity@externalTemp@internalTemp"
"""

verbose = True

HOST = "localhost"
PORT = 9999

# RDF template                                           
rdfTemplateCooker = """
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
                                <nearTo rdf:datatype="http://www.geonames.org/ontology#Feature" rdf:resource="%(NEAR_TO_HOLDER)s"/>
                            </Location>
                        </hasLocation>
                        <luminosity rdf:datatype="http://www.w3.org/2001/XMLSchema#float">%(LUMINOSITY_HOLDER)s</luminosity>
                        <externalTemperature rdf:datatype="http://www.w3.org/2001/XMLSchema#float">%(TEMP_EXTERNAL_HOLDER)s</externalTemperature>
                        <internalTemperature rdf:datatype="http://www.w3.org/2001/XMLSchema#float">%(TEMP_INTERNAL_HOLDER)s</internalTemperature>
                    </SolarCooker>
                                            
                    </rdf:RDF>
                    """

def convertToRDF(data):
    
        # get the data
        latitude, longitude, name, luminosity, tempExternal, tempInternal = re.split("@", data)
        
        getNearerPlace(latitude,longitude)
                
        # complete the RDF        
        rdfStrCooker = rdfTemplateCooker % {
            "LOCATION_ID_HOLDER" : "location" + name,
            "LATITUDE_HOLDER" : latitude,
            "LONGITUDE_HOLDER" : longitude,
            "NEAR_TO_HOLDER" : "blah", # TODO link the nearTo property to GeoNames data
            "COOKER_ID_HOLDER" : "cooker" + name,
            "NAME_HOLDER" : name,
            "LUMINOSITY_HOLDER" : luminosity,
            "TEMP_EXTERNAL_HOLDER" : tempExternal,
            "TEMP_INTERNAL_HOLDER" : tempInternal   
        }
                               
        if verbose:
            print "************RESULT**************"
            print rdfStrCooker
            print "************END**************"
            
        return rdfStrCooker  
        
        
def getNearerPlace(lat, lng):
       
    params = urllib.urlencode({'lat': lat, 'lng' : lng, 'username': 'rhokbilbao'})
    response = urllib.urlopen('http://api.geonames.org/extendedFindNearby?%s' % params)
    
    xmldoc = minidom.parseString(response.read())
    itemlist = xmldoc.getElementsByTagName('geonameId')

    return itemlist[len(itemlist)-1].childNodes[0].data
    


class MyTCPHandler(SocketServer.BaseRequestHandler): 
    
     def handle(self):
        self.data = self.request.recv(1024).strip()
        
        if verbose:
            import threading
            print threading.currentThread()
            print "{} wrote:".format(self.client_address[0])
            print self.data
        
        rdf = convertToRDF(self.data)        
        
        # TODO: send the rdf info to virtuoso
        self.request.sendall(rdf)    


if __name__ == "__main__":
    server = SocketServer.ThreadingTCPServer((HOST, PORT), MyTCPHandler)

    server.serve_forever()