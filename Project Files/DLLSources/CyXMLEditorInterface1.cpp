#include "CvGameCoreDLL.h"
#include "CyXMLEditor.h"
#include "CyXMLObject.h"

#include <boost/python/class.hpp>
#include <boost/python/manage_new_object.hpp>
namespace python = boost::python;


void CyXMLEditorPythonInterface1(python::class_<CyXMLEditor>& x)
{
	OutputDebugString("Python Extension Module - CyXMLEditorPythonInterface1\n");
	x
		.def("getNumFiles", &CyXMLEditor::getNumFiles, "int ()")
		.def("getFileName", &CyXMLEditor::getFileName, "string (int)")
		.def("getActiveFile", &CyXMLEditor::getActiveFile, "int ()")
		.def("setActiveFile", &CyXMLEditor::setActiveFile, "void (int)")
		.def("getNumTypes", &CyXMLEditor::getNumTypes, "int (int)")
		.def("getComboFile", &CyXMLEditor::getComboFile, "int (int, int)")
		.def("isCombo", &CyXMLEditor::isCombo, "bool (int)")
		.def("getType", &CyXMLEditor::getType, "string (int, int)")
		.def("getTypeNoPrefix", &CyXMLEditor::getTypeNoPrefix, "string (int, int)")
		.def("getFilePrefix", &CyXMLEditor::getFilePrefix, "string (int)")
		.def("getFileTag", &CyXMLEditor::getFileTag, "string (int)")
		.def("getFileDir", &CyXMLEditor::getFileDir, "string (int)")
		.def("getXMLDir", &CyXMLEditor::getXMLDir, "string (int)")
		.def("getNumXMLDir", &CyXMLEditor::getXMLNumDir, "int ()")
		.def("getList", &CyXMLEditor::getList, python::return_value_policy<python::manage_new_object>(), "object ()")
		.def("getMaxGameFontID", &CyXMLEditor::getMaxGameFontID, "int ()")
		.def("getKey", &CyXMLEditor::getKey, "string (int, bool, bool, bool)")
		.def("getWKey", &CyXMLEditor::getWKey, "string (int, bool, bool, bool)")
		.def("getGameFontGroupIndex", &CyXMLEditor::getGameFontGroupIndex, "int (int)")
		.def("getNextGameFontGroupStart", &CyXMLEditor::getNextGameFontGroupStart, "int (int)")
		.def("getColor", &CyXMLEditor::getColor, "NiColorA (string)")
		.def("getPlayerColor", &CyXMLEditor::getPlayerColor, "NiColorA (string, int)")
		.def("writeAllFiles", &CyXMLEditor::writeAllFiles, "void ()")
		.def("getIcon", &CyXMLEditor::getIcon, "string (string)")
		.def("isColonizationExe", &CyXMLEditor::isColonizationExe, "bool ()")
		.def("isEditorInMod", &CyXMLEditor::isEditorInMod, "bool ()")
		.def("getKeyboard", &CyXMLEditor::getKeyboard, "string ()")
		.def("resetKeyboard", &CyXMLEditor::resetKeyboard, "void ()")
		.def("setKeyboardKey", &CyXMLEditor::setKeyboardKey, "void (string, bool, bool, bool)")
		.def("toUnicode", &CyXMLEditor::toUnicode, "string (int)")
		.def("cleanActiveFile", &CyXMLEditor::cleanActiveFile, "void ()")

		.def("copyFiles", &CyXMLEditor::copyFiles, "void ()")

		.def("altKey"    , &CyXMLEditor::altKey    , "bool ()")
		.def("shiftKey"  , &CyXMLEditor::shiftKey  , "bool ()")
		.def("ctrlKey"   , &CyXMLEditor::ctrlKey   , "bool ()")
		.def("scrollLock", &CyXMLEditor::scrollLock, "bool ()")
		.def("capsLock"  , &CyXMLEditor::capsLock  , "bool ()")
		.def("numLock"   , &CyXMLEditor::numLock   , "bool ()")

		.def("quit"      , &CyXMLEditor::quit      , "void ()")

		.def("getCommandItem", &CyXMLEditor::getCommandItem, python::return_value_policy<python::manage_new_object>(), "object (string)")

		;
}

void CyXMLEditorPythonObjectInterface1(python::class_<CyXMLObject>& x)
{
	OutputDebugString("Python Extension Module - CyXMLEditorPythonObjectInterface1\n");
	x
		.def("getNextSiblingSameName", &CyXMLObject::getNextSiblingSameName, python::return_value_policy<python::manage_new_object>(), "object ()")
		.def("getFirstSchemaChild", &CyXMLObject::getFirstSchemaChild, python::return_value_policy<python::manage_new_object>(), "object ()")
		.def("next", &CyXMLObject::next, python::return_value_policy<python::manage_new_object>(), "object ()")
		
		.def("getID", &CyXMLObject::getID, "int ()")
		.def("isAllocated", &CyXMLObject::isAllocated, "bool ()")
		.def("isBool", &CyXMLObject::isBool, "bool ()")
		.def("isInt", &CyXMLObject::isInt, "bool ()")
		.def("isListElement", &CyXMLObject::isListElement, "bool ()")
		.def("isCombo", &CyXMLObject::isCombo, "bool ()")
		.def("isDir", &CyXMLObject::isDir, "bool ()")
		.def("isString", &CyXMLObject::isString, "bool ()")
		.def("isMandatory", &CyXMLObject::isMandatory, "bool ()")
		.def("isOptional", &CyXMLObject::isOptional, "bool ()")
		.def("getTextType", &CyXMLObject::getTextType, "int ()")
		.def("isText", &CyXMLObject::isText, "bool ()")
		.def("canDelete", &CyXMLObject::canDelete, "bool ()")
		.def("getName", &CyXMLObject::getName, "string ()")
		.def("getGameFontDisplayID", &CyXMLObject::getGameFontDisplayID, "int ()")

		.def("getInfoTypeString", &CyXMLObject::getInfoTypeString, "string ()")
		.def("getInfoType", &CyXMLObject::getInfoType, "int ()")
		.def("allowsTypeNone", &CyXMLObject::allowsTypeNone, "bool ()")
		.def("isRemoteCreate", &CyXMLObject::isRemoteCreate, "bool ()")
		.def("isRemoteCreatePrefix", &CyXMLObject::isRemoteCreatePrefix, "bool ()")
		.def("getHelp", &CyXMLObject::getHelp, "string ()")
		.def("getInfoClass", &CyXMLObject::getInfoClass, "string ()")
		.def("getSchemaChild", &CyXMLObject::getSchemaChild, "string (i)")
		.def("getNumSchemaChildren", &CyXMLObject::getNumSchemaChildren, "int ()")
		.def("getParentType", &CyXMLObject::getParentType, "string ()")
		.def("setInfo", &CyXMLObject::setInfo, "void (int)")
		
		.def("getType", &CyXMLObject::getType, "string ()")
		.def("getChildType", &CyXMLObject::getChildType, "string ()")
		.def("getValue", &CyXMLObject::getValue, "string ()")
		.def("getBoolValue", &CyXMLObject::getBoolValue, "bool ()")
		.def("getChildString", &CyXMLObject::getChildString, "string (string)")
		.def("getButtonArt", &CyXMLObject::getButtonArt, "string ()")
		.def("getButtonArtChild", &CyXMLObject::getButtonArtChild, "string ()")
		.def("getActiveComboFile", &CyXMLObject::getActiveComboFile, "int ()")
		.def("getText", &CyXMLObject::getText, "wstring ()")

		.def("setValue", &CyXMLObject::setValue, "void (string)")
		.def("createRemote", &CyXMLObject::createRemote, "void ()")
		.def("setGameFontChar", &CyXMLObject::setGameFontChar, "void (int)")
		.def("deleteXMLEntry", &CyXMLObject::deleteXMLEntry, "void ()")
		.def("dragTo", &CyXMLObject::dragTo, "void (object)")
		.def("clone", &CyXMLObject::clone, "void ()")
		;
}

void CyXMLEditorPythonCommandItemInterface1(python::class_<CyXMLCommandItem>& x)
{
	OutputDebugString("Python Extension Module - CyXMLEditorPythonMenuItemInterface1\n");
	x
		.def("getName", &CyXMLCommandItem::getName, "str ()")
		.def("getHelp", &CyXMLCommandItem::getHelp, "str ()")
		.def("getText", &CyXMLCommandItem::getText, "str ()")
		;
}
