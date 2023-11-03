import codecs
import xml.sax
import xml.sax.handler
import xml.sax.xmlreader
from pprint import  pprint
import os
import sys

class Civ4TextXMLHandler(xml.sax.ContentHandler):

    def __init__(self, filename, dict_to_fill):
        self.within_tag = None
        self.dict_to_fill = dict_to_fill
        self.filename = filename
        #super(type(Civ4TextXMLHandler)).__init__(type(Civ4TextXMLHandler))

    def setDocumentLocator(self, locator):
        pass

    def startDocument(self):
        #print "Starting parsing %s"%(self.filename)
        pass

    def startElement(self, name, attrs):
        if name in ["Tag","English","French","Russian"]:
            self.within_tag = name


    def characters(self, content):
        if  self.within_tag is not None:
            pass

        if  self.within_tag == "Tag":
            self.dict_to_fill[content] = self.filename


    def endElement(self, name):
            self.within_tag = None

    def endDocument(self):
        pass


tags_from_mod = dict()
tags_from_vanilla = dict()



def get_tags_from_text(filename,tags):

    input_source = xml.sax.xmlreader.InputSource();
    file_stream = open(filename, "rb")
    input_source.setByteStream(file_stream)
    input_source.setEncoding("utf8")
    handler = Civ4TextXMLHandler(filename, tags)

    xml.sax.parse(input_source, handler)



start =  os.getcwd() ##assumed to be "Project Files"

mod_folder = r"..\Assets\XML\Text"
os.chdir(mod_folder)

for root, dir, files  in os.walk("."):
    for file in files:
        #print "MOD",file
        if file[-4:] != ".xml":
            continue
        get_tags_from_text(file, tags_from_mod)

os.chdir(start)
print start

vanilla_folder = "..\..\..\Assets\XML\Text"

os.chdir(vanilla_folder)
for root, dir, files  in os.walk("."):
    for file in files:
        #print "VANILLA", file
        if file[-4:] != ".xml":
            continue
        get_tags_from_text(file, tags_from_vanilla)

print "##############"
for key in tags_from_vanilla:
    if not key in tags_from_mod:
        print key+"\t"+tags_from_vanilla[key]

