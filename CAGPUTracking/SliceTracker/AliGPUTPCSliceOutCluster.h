//-*- Mode: C++ -*-
// ************************************************************************
// This file is property of and copyright by the ALICE HLT Project        *
// ALICE Experiment at CERN, All rights reserved.                         *
// See cxx source for full Copyright notice                               *
//                                                                        *
//*************************************************************************

#ifndef ALIHLTTPCCASLICEOUTCLUSTER_H
#define ALIHLTTPCCASLICEOUTCLUSTER_H

#include "AliGPUTPCDef.h"

/**
 * @class AliGPUTPCSliceOutCluster
 * AliGPUTPCSliceOutCluster class contains clusters which are assigned to slice tracks.
 * It is used to send the data from TPC slice trackers to the GlobalMerger
 */
class AliGPUTPCSliceOutCluster
{
  public:
	GPUhd() void Set(unsigned int id, unsigned char row, unsigned char flags, unsigned short amp, float x, float y, float z)
	{
		fRow = row;
		fFlags = flags;
		fAmp = amp;
		fId = id;
		fX = x;
		fY = y;
		fZ = z;
	}

	GPUhd() float GetX() const { return fX; }
	GPUhd() float GetY() const { return fY; }
	GPUhd() float GetZ() const { return fZ; }
	GPUhd() unsigned int GetId() const { return fId; }
	GPUhd() unsigned char GetRow() const { return fRow; }
	GPUhd() unsigned char GetFlags() const { return fFlags; }
	GPUhd() unsigned short GetAmp() const { return fAmp; }

  private:
	unsigned int fId;     // Id ( slice, patch, cluster )
	unsigned char fRow;   // row
	unsigned char fFlags; //flags
	unsigned short fAmp;  //amplitude
	float fX;             // coordinates
	float fY;             // coordinates
	float fZ;             // coordinates

#ifdef GMPropagatePadRowTime
  public:
	float fPad;
	float fTime;
#endif
};

#endif
