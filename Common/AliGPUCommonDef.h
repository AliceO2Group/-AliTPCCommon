#ifndef ALIGPUCOMMONDEF_H
#define ALIGPUCOMMONDEF_H

#if defined(__CINT__) || defined(__ROOTCINT__)
#define CON_DELETE
#define CON_DEFAULT
#define CONSTEXPR const
#else
#define CON_DELETE = delete
#define CON_DEFAULT = default
#define CONSTEXPR constexpr
#endif

#include "AliGPUCommonDefGPU.h"

#if defined(GPUCA_STANDALONE) || defined(GPUCA_O2_LIB) || defined(GPUCA_GPULIBRARY)
	#define GPUCA_ALIGPUCODE
#endif

#endif
