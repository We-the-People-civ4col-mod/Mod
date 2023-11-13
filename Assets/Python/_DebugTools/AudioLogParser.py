# Parses audio.log and generates a readable list of problems encountered

import sys


class TextParser:
	def __init__(self, l_Text, outputFile ):
		self.__list_Text = l_Text
		self.outputFile = outputFile
		self.run()

	def run(self):
		f = open(self.outputFile,'w')
		self.CollectWRNERR()
		self.removeUselessLines()
		f.write("########### Audio Results ###########\n\n")
		f.write("COULD NOT FIND FILE PROBLEMS\n\n")
		f.writelines(self.getListByToken("FSharedSoundData::Load: Could not find file "))
		self.printSpacer(f)
		f.write("COULD NOT LOAD SCRIPT ID\n\n")
		f.writelines(self.getListByToken("Could not load scriptId "))
		self.printSpacer(f)
		f.write("STREAM COULD NOT BE LOADED\n\n")
		f.writelines(self.getListByToken("Stream could not be loaded, "))
		self.printSpacer(f)
		f.write("COULD NOT INITIALIZE HANDLE FOR SOUND\n\n")
		f.writelines(self.getListByToken("FSound3D::DoLoad: Could not initialize handle for sound "))
		self.printSpacer(f)
		f.write("COULD NOT START SOUNDTRACK WITH\n\n")
		f.writelines(self.getListByToken("FSoundtrack::Start(): Could not start soundtrack with "))
		f.close()

	def printSpacer(self,f):
		f.write("\n\n\n----------------------------------------------------------\n\n\n")

	def getListByToken(self, token):
		newlist = list()
		for line in self.__list_Text:
			tok = None
			if line and line.find(token) != -1:
				tok = line[line.index(token) + len(token):]	# remove everything but the important info
				if tok.find('./n') != -1:
					tok = int(tok[:tok.index('./n')])
				if tok and not tok in newlist:
					newlist.append(tok)
		newlist.sort()
		if type(newlist[0]) == type(int()):
			strList = []
			[ strList.append("%s\n"%item) for item in newlist ]
			return strList
		return newlist

	def removeUselessLines(self):
		useless = [ "FAudioSystemMiles::Acquire3DSampleId: Could not acquire Id",
					"Illegal index -1",
					"FAudioSystemMiles::Set3DSoundPosition: Illegal index -1 of type 1" ]
		for line in self.__list_Text:
			for item in useless:
				if line.find(item) != -1:
					self.__list_Text.remove(line)
					break

	def CollectWRNERR(self):
		newList = []
		for line in self.__list_Text:
			idx = -1
			if line.find('WRN:') != -1:
				idx = line.index('WRN:')
			elif line.find('ERR:') != -1:
				idx = line.index('ERR:')
			if idx != -1:
				newList.append(line[idx:])
		self.__list_Text = newList

audioLog = sys.argv[1]
finalLog = sys.argv[2]
f = open(audioLog,'r')
if ( TextParser( f.readlines(), finalLog ) ):
	print "Audio Log Parsed"
f.close()