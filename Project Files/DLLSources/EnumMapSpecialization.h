#ifndef ENUM_MAP_SPECIALIZATION_H
#define ENUM_MAP_SPECIALIZATION_H
#pragma once

#include "CvEnumsFunctions.h"
#include "EnumMapSpecializationInt.h"

// special cases for EnumMap. Needs to be included after this file
template<class T, int DEFAULT>
class EnumMap<CityPlotTypes, T, DEFAULT> : public EnumMapBase <int, T, DEFAULT, CityPlotTypes> {};


#endif
