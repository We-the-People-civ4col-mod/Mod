import codecs
import xml.sax
import xml.sax.handler
import xml.sax.xmlreader
from pprint import  pprint
import os
import os.path
import sys

class Civ4TextXMLHandler(xml.sax.ContentHandler):

    def __init__(self, filename, dict_to_fill):
        self.within_tag = None
        self.dict_to_fill = dict_to_fill
        self.filename = filename
        self.locator = None
        self.start_end = None
        self.current_Tag_content = None
        #super(type(Civ4TextXMLHandler)).__init__(type(Civ4TextXMLHandler))

    def setDocumentLocator(self, locator):
        self.locator = locator

    def startDocument(self):
     #   print "Starting parsing %s"%(self.filename)
        pass

    def startElement(self, name, attrs):
        if name == "TEXT":
            self.start_end = self.locator.getLineNumber()
            self.current_Tag_content = ""
        if name in ["Tag","English","French","Russian"]:
            self.within_tag = name


    def characters(self, content):
        if  self.within_tag is not None:
            pass

        if  self.within_tag == "Tag":
            self.current_Tag_content += content

            self.last_tag = content


    def endElement(self, name):
        self.within_tag = None
        if name == "TEXT":
            self.current_tag["line_end"] = self.locator.getLineNumber()
        if name == "Tag":
            self.current_tag = {"FILE": self.filename, "line_start": self.start_end, "line_end": None}
            self.dict_to_fill[ self.current_Tag_content] = self.current_tag
            self.last_tag = self.current_Tag_content
    def endDocument(self):
        pass

class LineCopier:
    """This class  msut only output UTF-8 files.It is not XML aware"""
    destination_encoding = "utf8"

    opening_text_tag = "<Civ4GameText xmlns=\"http://www.firaxis.com\">"
    closing_text_tag = "</Civ4GameText>"

    def __init__(self,dest_file_name, vanilla_folder):

        self.dest_file_name = dest_file_name
        self.dest_file = None

        self.vanilla_folder = vanilla_folder
        self.current_vanilla_file_name = None
        self.current_vanilla_file = None
        self.current_vanilla_line = 0

    def init_dest_file(self):
        self.dest_file = codecs.open(self.dest_file_name, mode="wb", encoding=self.destination_encoding)
        self.dest_file.write(self.opening_text_tag+"\r\n")

    def open_new_file(self, new_vanilla_file_name):
        assert new_vanilla_file_name != self.current_vanilla_file_name,"We try to reopen a file we're already on %s"%new_vanilla_file_name

        if self.current_vanilla_file != None:
            self.current_vanilla_file.close()

        path_to_open = os.path.join(self.vanilla_folder, new_vanilla_file_name)

        self.current_vanilla_file_name = new_vanilla_file_name
        self.current_vanilla_file = codecs.open(path_to_open,"r",encoding="cp1252")
        self.current_vanilla_line = 0

    def process_extract_list_entry(self,entry):
        file_name, start, end, tag = entry

        if file_name != self.current_vanilla_file_name:
            self.open_new_file(file_name)

        content = ""
        while self.current_vanilla_line < start:
            content = self.current_vanilla_file.readline()
            self.current_vanilla_line += 1


        assert u"<TEXT>" in content, "we should get a <TEXT> start"

        while self.current_vanilla_line < end :
            self.dest_file.write(content)
            content = self.current_vanilla_file.readline()
            self.current_vanilla_line += 1


        assert  u"</TEXT>" in content, "we should get a </TEXT> end"

        self.dest_file.write(content)
        self.dest_file.flush()

    def process_extract_list(self,entries):
        if len(entries) == 0:
            return
        self.init_dest_file()

        for entry in entries:
            self.process_extract_list_entry(entry)

        self.close_dest_file()

    def close_dest_file(self):
        "places the right closing tags and close the files"

        self.dest_file.write(self.closing_text_tag + "\r\n")
        self.dest_file.close()

        if not self.current_vanilla_file.closed:
            self.current_vanilla_file.close()






def get_tags_from_text(filename,tags):

    input_source = xml.sax.xmlreader.InputSource();
    file_stream = open(filename, "rb")
    input_source.setByteStream(file_stream)
    input_source.setEncoding("utf8")
    handler = Civ4TextXMLHandler(filename, tags)

    xml.sax.parse(input_source, handler)


def used_in_mod(key):
    if "SAN_MARTIN" in key: ## we don't have San Martin as a leader
        return False
    return True


def make_tags_list(text_folder, tags_to_fill):

    os.chdir(text_folder)
    for root, dir, files in os.walk("."):
        for file in files:
            # print "MOD",file
            if file[-4:] != ".xml":
                continue
            get_tags_from_text(file, tags_to_fill)
    os.chdir(start)



if __name__ == "__main__":
    tags_from_mod = dict()
    tags_from_vanilla = dict()

    mod_text_folder = r"..\Assets\XML\Text"
    vanilla_text_folder = "..\..\..\Assets\XML\Text"

    start =  os.getcwd() ##assumed to be "Project Files"

    make_tags_list(mod_text_folder, tags_from_mod)
    make_tags_list(vanilla_text_folder, tags_from_vanilla)

    print "##############"
    os.chdir(start)

    extracts = list()
    print "TXT_KEY\tFILE\tLINE_START\tLINE_END"
    for key in tags_from_vanilla:
        if not key in tags_from_mod and used_in_mod(key):
            value = tags_from_vanilla[key]
            print key+"\t"+value["FILE"]+"\t"+value["line_start"].__str__()+"\t"+value["line_end"].__str__()
            extracts.append((value["FILE"],value["line_start"],value["line_end"],key))

    extracts.sort()
    print len(extracts)

    dest_file_name = "Vanilla_TEXT_XML_key.xml"
    line_copier = LineCopier(dest_file_name,vanilla_text_folder)
    line_copier.process_extract_list(extracts)


