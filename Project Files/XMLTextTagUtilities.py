import codecs
import xml.sax
import xml.sax.handler
import xml.sax.xmlreader
import re
import glob
import os.path



class PresenceSpot(object):
    
    EXE_COLONIZATION = 1
    EXE_PITBOSS = 2
    PYTHON_CODE = 3
    DLL_CODE = 4
    XML_REFERENCE = 5
    TEXT_TAG = 8
    TEXT_TAG_STANDARD = 9  # For standard
    TEXT_TAG_CIVS_VARS = 10  # For City names, Ship Names, Admirals and Generals

    #__slots__ = ["source", "file_name","line_presence_start","line_presence_end"]
    LABELS = {          
        EXE_COLONIZATION : "EXE_COLONIZATION",
        EXE_PITBOSS : "EXE_PITBOSS",
        PYTHON_CODE : "PYTHON_CODE",
        DLL_CODE : "DLL_CODE",
        XML_REFERENCE : "XML_REFERENCE",
        TEXT_TAG : "TEXT_TAG",
        TEXT_TAG_STANDARD : "TEXT_TAG_STANDARD",
        TEXT_TAG_CIVS_VARS : "TEXT_TAG_CIVS_VARS",
    }

    def __init__(self, source, file_name= None, line_start= None, line_end = None  ):
        self.source = source
        self.file_name = file_name
        self.line_start = line_start
        self.line_end = line_end

    def __repr__(self):
        if self.source in ( self.EXE_COLONIZATION,self.EXE_PITBOSS):
            return self.LABELS[self.source]
        elif self.source in ( self.PYTHON_CODE,self.DLL_CODE, self.XML_REFERENCE):
            return "%s:%s:%d"%(self.LABELS[self.source],self.file_name,self.line_start)
        return "%s:%s:%d:%d"%(self.LABELS[self.source],self.file_name,self.line_start,self.line_end)

class XMLTagUsage(object):


    ALL_SOURCES = [PresenceSpot.EXE_COLONIZATION,
                   PresenceSpot.EXE_PITBOSS,
                   PresenceSpot.PYTHON_CODE,
                   PresenceSpot.DLL_CODE,
                   PresenceSpot.XML_REFERENCE]
    ALL_PRESENCES = [PresenceSpot.TEXT_TAG_STANDARD,PresenceSpot.TEXT_TAG_CIVS_VARS]

    def __init__(self):
        self.__usage_list = []
        self.__presence_list = []

    def __nonzero__(self):
        return  self.__presence_list

    def push_usage(self, source, file_usage = None, line_usage = None ):
        """ add an usage to the place where the tag is used

        :param source:
        :type  source: int
        :param file_usage: file where the tag is used
        :param line_usage: the line number the tag is used
        :rtype: None
        """
        assert source in XMLTagUsage.ALL_SOURCES

        assert not source in (PresenceSpot.EXE_COLONIZATION, PresenceSpot.EXE_PITBOSS) \
               or (file_usage == None and line_usage == None)

        if source in (PresenceSpot.EXE_COLONIZATION, PresenceSpot.EXE_PITBOSS):
            self.__usage_list.append(PresenceSpot(source, None, None, None))
            return

        assert file_usage != None and line_usage != None \
               and type(file_usage) == str and type(line_usage) == int

        self.__usage_list.append(PresenceSpot(source, file_usage, line_usage, None))


    def push_presence(self,source, file_presence, line_presence_start, line_presence_end):
        assert source in XMLTagUsage.ALL_PRESENCES
        assert line_presence_start < line_presence_end
        self.__presence_list.append(PresenceSpot(source, file_presence, line_presence_start, line_presence_end))


    def is_duplicated(self):
        """ Is this tag declared multiple times?

        :return: True if the tag is declared more than once in the XML files
        """
        return len(self.__presence_list) >= 2

    def is_missing(self):
        return not self.__presence_list and self.__usage_list

    def is_not_used(self):
        return self.__presence_list and not self.__usage_list

    def __repr__(self):
        return  "P: "+self.__presence_list.__repr__()+", "\
               +"U: "+self.__usage_list.__repr__()



def fill_usage_exe_strings(strings_file, usage_dict, source):
    """
    :param strings_file: a file object, that contains the strings extracted from the exe
    :type strings_file: file
    :param usage_dict: the usage dict to be filled
    :type usage_dict: dict[str,XMLTagUsage]
    :param source: either PresenceSpot.EXE_COLONIZATION or PresenceSpot.EXE_PITBOSS
    :type source: int
    :rtype: None
    """
    line = strings_file.readline() #type: str
    line_no=1
    while line:
        tag = line.rstrip() #type: str
        tag_usage = usage_dict.get(tag, XMLTagUsage())

        tag_usage.push_usage(source)

        usage_dict[tag] = tag_usage
        line = strings_file.readline()
        line_no += 1

def fill_usage_cpp_py(source_file, file_name, usage_dict, source):
    """ fills the  usage_dict with

    :param source_file: a file object, opening a C++ or a Python file
    :type source_file: file
    :param file_name: the path to access the file
    :type file_name: str
    :param usage_dict: the dictionary where all keys are listed.
    :type usage_dict: dict[str, XMLTagUsage]
    :param source:
    :return:
    """
    regexp = re.compile("\"(?P<TXT>TXT_KEY_[^\"]*)\"")


    line = source_file.readline() #type : str
    line_no = 1
    while line:
        start_pos = 0
        while start_pos < len(line):
            result = regexp.search(line,start_pos) # there might be more than one tag key
            if result is None:
                break
            tag_name = result.group("TXT")

            tag_usage = usage_dict.get(tag_name, XMLTagUsage())

            tag_usage.push_usage(source,file_name,line_no)

            usage_dict[tag_name] = tag_usage
            start_pos = result.end()

        line = source_file.readline()
        line_no += 1

class Civ4TextXMLHandler(xml.sax.ContentHandler):

    def __new__(cls, *args, **kwargs):
        return Civ4TextXMLHandler(*args,**kwargs)

    def __init__(self, file_name, usage_dict, source):
        self.tags_path = []
        self.file_name = file_name
        self.locator = None
        self.source = source
        self.usage_dict = usage_dict

    def setDocumentLocator(self, locator):
        self.locator = locator



if __name__ == "__main__":
    tags_presence_dict = dict() #type: dict[str, XMLTagUsage]

    colonization = file("colonization_exe.txt")
    fill_usage_exe_strings(colonization, tags_presence_dict, PresenceSpot.EXE_COLONIZATION)
    colonization.close()

    pit_boss = file("pitboss_exe.txt")
    fill_usage_exe_strings(pit_boss, tags_presence_dict, PresenceSpot.EXE_PITBOSS)
    pit_boss.close()

    os.chdir("..")

    for file_name in glob.glob(".\\Project Files\\DLLSources\\*.cpp"):
        cpp = file(file_name)
        fill_usage_cpp_py(cpp, file_name, tags_presence_dict, PresenceSpot.DLL_CODE)
        cpp.close()

    import pprint
    pprint.pprint(tags_presence_dict)