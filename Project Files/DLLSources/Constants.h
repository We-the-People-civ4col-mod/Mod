#pragma once

// Unllike CvConstants, this file contains nothing related to exe expectations.
// This way we have a file, which can be edited without considering exe and python constrains.

typedef unsigned char    byte;
typedef unsigned short   word;
typedef unsigned int     uint;
typedef unsigned long    dword;
typedef unsigned __int64 qword;
typedef wchar_t          wchar;


typedef          char     int8;
typedef unsigned char    uint8;
typedef          short    int16;
typedef unsigned short   uint16;
typedef          int      int32;
typedef unsigned int     uint32;
typedef          __int64  int64;
typedef unsigned __int64 uint64;



const          int MAX_CHAR = 0x7f;
const          int MIN_CHAR = (char)0x80;
const          int MAX_SHORT = 0x7fff;
const          int MIN_SHORT = (short)0x8000;
const          int MAX_INT = 0x7fffffff;
const          int MIN_INT = 0x80000000;
const unsigned int MAX_UNSIGNED_CHAR = 0xff;
const unsigned int MIN_UNSIGNED_CHAR = 0x00;
const unsigned int MAX_UNSIGNED_SHORT = 0xffff;
const unsigned int MIN_UNSIGNED_SHORT = 0x0000;
const unsigned int MAX_UNSIGNED_INT = 0xffffffff;
const unsigned int MIN_UNSIGNED_INT = 0x00000000;

#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

#define SQR(x)      ( (x) * (x) )
#define DEGTORAD(x) ( (float)( (x) * (M_PI / 180) ))
#define LIMIT_RANGE(low, value, high) value = (value < low ? low : (value > high ? high : value));

// pi
const double M_PI  = 3.14159265358979323846;
const float  fM_PI = 3.141592654f;
