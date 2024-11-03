#pragma once

#ifdef TEXT_ASSERT
void my_assert(const char*, const char*, int);
#define assert(exp) (void)( (exp) || (my_assert(__FILE__, __FUNCTION__, __LINE__), 0) )

#else
#ifdef assert
#undef assert
#endif
#define assert(exp) (void)( (exp) || (((int*)0)[0] = 0, 0) )
#endif
