#ifndef ENUM_MAP_SPECIALIZATION_H
#define ENUM_MAP_SPECIALIZATION_H
#pragma once

#include "CvEnumsFunctions.h"
#include "EnumMapCore.h"
#include "EnumMapSpecializationInt.h"
#include "EnumMapSpecializationBool.h"
#include "EnumMapSpecializationClass.h"

// special cases for EnumMap. Needs to be included after this file
template<class T, int DEFAULT>
class EnumMap<CityPlotTypes, T, DEFAULT> : public EnumMapBase <int, T, DEFAULT, CityPlotTypes, VARINFO<T>::STATIC<VARINFO<CityPlotTypes>::LENGTH>::VAL, VARINFO<T>::TYPE, VARIABLE_LENGTH_FIRST_KNOWN> {};


#endif
