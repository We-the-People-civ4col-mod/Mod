#pragma once

#ifndef CvDLLPythonIFaceBase_h
#define CvDLLPythonIFaceBase_h

//
// abstract interface for Python functions used by DLL
// Creator - Mustafa Thamer
// Copyright 2005 Firaxis Games
//

#include <boost/python/object.hpp>

class CvUnit;
class CvPlot;
class CvDLLPythonIFaceBase
{
public:

	virtual const char* getMapScriptModule() = 0;

	template <typename T>	void setSeqFromArray(const T* /*src*/, int size, PyObject* /*dst*/);
	template <typename T>	int putSeqInArray(PyObject* /*src*/, T** /*dst*/);
	template <typename T>	int putStringSeqInArray(PyObject* /*src*/, T** /*dst*/);
	template <typename T>	int putFloatSeqInArray(PyObject* /*src*/, T** /*dst*/);
	template <typename T>	PyObject* makePythonObject(T*);

	virtual PyObject* MakeFunctionArgs(void** args, int argc) = 0;

	virtual bool moduleExists(const char* moduleName, bool bLoadIfNecessary) = 0;
	virtual bool callFunction(const char* moduleName, const char* fxnName, void* fxnArg=NULL) = 0;
	virtual bool callFunction(const char* moduleName, const char* fxnName, void* fxnArg, long* result) = 0;
	virtual bool callFunction(const char* moduleName, const char* fxnName, void* fxnArg, CvString* result) = 0;
	virtual bool callFunction(const char* moduleName, const char* fxnName, void* fxnArg, CvWString* result) = 0;
	virtual bool callFunction(const char* moduleName, const char* fxnName, void* fxnArg, std::vector<byte>* pList) = 0;
	virtual bool callFunction(const char* moduleName, const char* fxnName, void* fxnArg, std::vector<int> *pIntList) = 0;
	virtual bool callFunction(const char* moduleName, const char* fxnName, void* fxnArg, int* pIntList, int* iListSize) = 0;
	virtual bool callFunction(const char* moduleName, const char* fxnName, void* fxnArg, std::vector<float> *pFloatList) = 0;
	virtual bool callPythonFunction(const char* szModName, const char* szFxnName, int iArg, long* result) = 0; // HELPER version that handles 1 arg for you

	virtual bool pythonGetGridSize(WorldSizeTypes eWorldSize, int *iGridWidth, int *iGridHeight) = 0;
	virtual bool pythonGetWrapXY(bool *bWrapX, bool *bWrapY) = 0;
	virtual bool pythonGetLatitudes(int *iTopLatitude, int *iBottomLatitude) = 0;
	virtual bool pythonIsBonusIgnoreLatitudes(bool *bBonusIgnoreLatitudes) = 0;
	virtual bool pythonIsEuropeStart(EuropeTypes eEurope, bool *bStart) = 0;
	virtual bool pythonGetEuropeWidthPercent(EuropeTypes eEurope, int *iWidth) = 0;
	virtual bool pythonGetEuropeMinLandDistance(EuropeTypes eEurope, int *iDistance) = 0;
	virtual bool pythonCalculateScore(PlayerTypes id, long* plScore, bool bFinal = false, bool bVictory = false) = 0;
	virtual bool pythonDoGoody(PlayerTypes id, CvPlot* pGoodyPlot, CvUnit* pGoodyUnit) = 0;
	virtual bool pythonAssignStartingPlots() = 0;
	virtual bool pythonFindStartingArea(PlayerTypes id, long* result) = 0;
	virtual bool pythonFindStartingPlot(PlayerTypes id, long* result) = 0;
	virtual bool pythonAddLakes() = 0;
	virtual bool pythonAddRivers() = 0;
	virtual bool pythonGetRiverStartCardinalDirection(CvPlot* pRiverPlot, long* result) = 0;
	virtual bool pythonGetRiverAltitude(CvPlot* pRiverPlot, long* result) = 0;
	virtual bool pythonAddFeatures() = 0;
	virtual bool pythonAddBonuses() = 0;
	virtual bool pythonAddBonusType(BonusTypes eBonusType) = 0;
	virtual bool pythonCanPlaceBonusAt(CvPlot* pBonusPlot, long* result) = 0;
	virtual bool pythonAddGoodies() = 0;
	virtual bool pythonCanPlaceGoodyAt(CvPlot* pGoodyPlot, long* result) = 0;
	virtual bool pythonGeneratePlotTypes(std::vector<int>& plotTypesOut) = 0;
	virtual bool pythonGenerateTerrainTypes(std::vector<int>& terrainTypesOut) = 0;
	virtual bool pythonGenerateRandomMap() = 0;
	virtual bool pythonNormalizeStartingPlotLocations() = 0;
	virtual bool pythonUsingDefaultImpl() = 0;
};

template <typename T>
PyObject* CvDLLPythonIFaceBase::makePythonObject(T* pObj)
{
	if (!pObj)
		return Py_None;

	boost::python::object bpo(pObj);
	PyObject* pyobj = bpo.ptr();
	Py_INCREF(pyobj);
	assert(pyobj->ob_refcnt==2);
	return pyobj;	// decrefs pyobj when bpo goes out of scope
}

//
// static
// convert array to python list
//
template <typename T>
void CvDLLPythonIFaceBase::setSeqFromArray(const T* aSrc, int size, PyObject* dst)
{
	if (size<1)
		return;

	int iSeqSize=PySequence_Length(dst);
	FAssertMsg(iSeqSize>=size, "sequence length too small");

	int i;
	int ret=0;
	for (i=0;i<size;i++)
	{
		PyObject* x = PyInt_FromLong(aSrc[i]);
		ret=PySequence_SetItem(dst, i, x);
		FAssertMsg(ret!=-1, "PySequence_SetItem failed");
		Py_DECREF(x);
	}

	// trim extra space
	if (iSeqSize>size)
	{
		ret=PySequence_DelSlice(dst, size, iSeqSize);
		FAssertMsg(ret!=-1, "PySequence_DelSlice failed");
	}
}

//
// static
// convert python list to array
// allocates array
//
template <typename T>
int CvDLLPythonIFaceBase::putSeqInArray(PyObject* src, T** aDst)
{
	*aDst = NULL;
	int size = PySequence_Length(src);
	if (size<1)
		return 0;

	*aDst = new T[size];
	int i;
	for (i=0;i<size;i++)
	{
		PyObject* item = PySequence_GetItem(src, i); /* Can't fail */
		FAssertMsg(PyInt_Check(item), "sequence item is not an int");
		(*aDst)[i] = (T)PyInt_AsLong(item);
		Py_DECREF(item);
	}
	return size;
}

//
// static
// convert python list to array
// allocates array
//
template <typename T>
int CvDLLPythonIFaceBase::putFloatSeqInArray(PyObject* src, T** aDst)
{
	*aDst = NULL;
	int size = PySequence_Length(src);
	if (size<1)
		return 0;

	*aDst = new T[size];
	int i;
	for (i=0;i<size;i++)
	{
		PyObject* item = PySequence_GetItem(src, i); /* Can't fail */
		FAssertMsg(PyFloat_Check(item), "sequence item is not an float");
		(*aDst)[i] = (T)PyFloat_AsDouble(item);
		Py_DECREF(item);
	}
	return size;
}

//
// static
// convert python list to array
// allocates array
//
template <typename T>
int CvDLLPythonIFaceBase::putStringSeqInArray(PyObject* src, T** aDst)
{
	*aDst = NULL;
	int size = PySequence_Length(src);
	if (size<1)
		return 0;

	*aDst = new T[size];
	int i;
	for (i=0;i<size;i++)
	{
		PyObject* item = PySequence_GetItem(src, i); /* Can't fail */
		FAssertMsg(PyString_Check(item), "sequence item is not a string");
		(*aDst)[i] = (T)PyString_AsString(item);
		Py_DECREF(item);
	}
	return size;
}

#endif	//  CvDLLPythonIFaceBase_h
