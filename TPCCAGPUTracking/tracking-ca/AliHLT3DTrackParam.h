//-*- Mode: C++ -*-
// @(#) $Id$

//* This file is property of and copyright by the ALICE HLT Project           * 
//* ALICE Experiment at CERN, All rights reserved.                            *
//* See cxx source for full Copyright notice                                  *

//*                                                                           *
//* AliHLT3DTrackParam class is under development and currently not in use    *
//*                                                                           *

#ifndef ALIHLT3DTRACKPARAM_H
#define ALIHLT3DTRACKPARAM_H

#include "Rtypes.h"
#include "TObject.h"

/**
 * @class AliHLT3DTrackParam
 */
class AliHLT3DTrackParam :public TObject
{
 public:

  //*
  //*  INITIALIZATION
  //*

  //* Constructor 

  AliHLT3DTrackParam(): fChi2(0),fNDF(0),fSignQ(0){} 

  //* Destructor (empty)

  virtual ~AliHLT3DTrackParam(){}

  //*
  //*  ACCESSORS
  //*


  //* Simple accessors 

  Double_t GetX()      const { return fParam[0]; }
  Double_t GetY()      const { return fParam[1]; }
  Double_t GetZ()      const { return fParam[2]; }
  Double_t GetPx()     const { return fParam[3]; }
  Double_t GetPy()     const { return fParam[4]; }
  Double_t GetPz()     const { return fParam[5]; }
  Double_t GetChi2()   const { return fChi2;  }
  Int_t    GetNDF()    const { return fNDF;   }
  Int_t    GetCharge() const { return fSignQ; }
  
  Double_t GetParameter ( int i ) const { return fParam[i]; }
  Double_t GetCovariance( int i ) const { return fCov[i]; }
  Double_t GetCovariance( int i, int j ) const { return fCov[( j<=i ) ? i*(i+1)/2+j :j*(j+1)/2+i]; }

  //* Accessors with calculations( &value, &estimated sigma )
  //* error flag returned (0 means no error during calculations) 


  //*
  //*  MODIFIERS
  //*
  
  Double_t *Param()  { return fParam; }
  Double_t *Cov()    { return fCov;   }
  Double_t &X()      { return fParam[0]; }
  Double_t &Y()      { return fParam[1]; }
  Double_t &Z()      { return fParam[2]; }
  Double_t &Px()     { return fParam[3]; }
  Double_t &Py()     { return fParam[4]; }
  Double_t &Pz()     { return fParam[5]; }
  Double_t &Chi2()   { return fChi2;  }
  Int_t    &NDF()    { return fNDF;   }
  Int_t    &Charge() { return fSignQ; }


  //*
  //*  UTILITIES
  //*
  
  //* Transport utilities
  
  Double_t GetDStoPoint( Double_t Bz, const Double_t xyz[3], const Double_t *T0=0 ) const;

  void TransportToDS( Double_t Bz, Double_t DS, Double_t *T0=0 );

  void TransportToPoint( Double_t Bz, const Double_t xyz[3], Double_t *T0=0 )
    { 
      TransportToDS( Bz,GetDStoPoint(Bz, xyz, T0), T0 ) ; 
    }

  void TransportToPoint( Double_t Bz, Double_t x, Double_t y, Double_t z, Double_t *T0=0 )
    { 
      Double_t xyz[3] = {x,y,z};
      TransportToPoint( Bz, xyz, T0 );
    }

  //* Fit utilities 

  void InitializeCovarianceMatrix();

  void GetGlueMatrix( const Double_t p[3], Double_t G[6], const Double_t *T0=0  ) const ;

  void Filter( const Double_t m[3], const Double_t V[6], const Double_t G[6] );

  //* Other utilities

  void SetDirection( Double_t Direction[3] );

  void RotateCoordinateSystem( Double_t alpha );

  void Get5Parameters( Double_t alpha, Double_t T[6], Double_t C[15] ) const;

 protected:

  Double_t fParam[6]; // Parameters ( x, y, z, px, py, pz ): 3-position and 3-momentum
  Double_t fCov[21];  // Covariance matrix
  Double_t fChi2;     // Chi^2
  Int_t    fNDF;      // Number of Degrees of Freedom
  Int_t    fSignQ;    // Charge

  ClassDef(AliHLT3DTrackParam, 1);

};


#endif