/*

// works only with ROOT >= 6

alienv load ROOT/latest-root6
alienv load Vc/latest

root -l 

.x loadlibs.C
.x IrregularSpline1DTest.C++
*/

#include "TFile.h"
#include "TRandom.h"
#include "TNtuple.h"
#include "Riostream.h"
#include "TSystem.h"
#include "TH1.h"
#include "TCanvas.h"
#include "IrregularSpline1D.h"

const int PolynomDegree = 7;
double coeff[PolynomDegree+1];

float F( float u )
{
  u-=0.5;
  double uu = 1.;  
  double f = 0;
  for( int i=0; i<=PolynomDegree; i++ ){
    f+=coeff[i]*uu;
    uu*=u;
  }
  return f;
}

typedef double myfloat;

int IrregularSpline1DTest()
{

  using namespace ali_tpc_common::tpc_fast_transformation ;

  cout<<"Test roundf(): "<<endl;
  for( float x=0.; x<=1.; x+=0.1 ){
    cout<<"roundf("<<x<<") = "<<roundf(x)<<" "<<(int)roundf(x)<<endl;
  }
  
  cout<<"Test 5 knots for n bins:"<<endl;
  for( int n=4; n<20; n++ ){
    int bin1 = (int) roundf( .25*n );
    int bin2 = (int) roundf( .50*n );
    int bin3 = (int) roundf( .75*n );
    cout<<n<<": 0 "<<bin1<<" "<<bin2<<" "<<bin3<<" "<<n<<endl;
  }

  cout<<"Test interpolation.."<<endl;


  gRandom->SetSeed(0);
  UInt_t seed = gRandom->Integer(100000); // 605

  gRandom->SetSeed(seed);
  cout<<"Random seed: "<<seed<<" "<<gRandom->GetSeed()<<endl;
  for( int i=0; i<=PolynomDegree; i++ ){
    coeff[i] = gRandom->Uniform(-1,1);
  }


  const int initNKnotsU = 10;
  int nAxisBinsU = 10;
 
  float du = 1./(initNKnotsU-1);  

  float knotsU[ initNKnotsU ];

  knotsU[0] = 0;
  knotsU[initNKnotsU-1] = 1;

  for( int i=1; i<initNKnotsU-1; i++ ){
    knotsU[i] = i*du + gRandom->Uniform(-du/3,du/3);
  }
  
  IrregularSpline1D spline;

  spline.construct( initNKnotsU, knotsU, nAxisBinsU );
 
  int nKnotsTot = spline.getNumberOfKnots();
  cout << "Knots: initial "<<initNKnotsU<<", created "<< nKnotsTot<<endl;
  for( int i=0; i<nKnotsTot; i++){
    cout<<"knot "<<i<<": "<<spline.getKnot(i).u<<endl;
  }

  const IrregularSpline1D &gridU = spline;

  myfloat *data0 = new myfloat[ nKnotsTot ]; // original data
  myfloat *data = new myfloat[ nKnotsTot ]; // corrected data
 
  int nu = gridU.getNumberOfKnots();
 
  for( int i=0; i<gridU.getNumberOfKnots(); i++ ){
    data0[i] = F( gridU.getKnot(i).u );
    //SG random
    data0[i] = gRandom->Uniform(-1.,1.);
    data[i] = data0[i];
  }
  
  spline.correctEdges(data);
  
  TCanvas *canv = new TCanvas("cQA","IrregularSpline1D  QA",2000,1000);
  canv->Draw();

  TH1F *qaX = new TH1F("qaX","qaX [um]",1000,-1000.,1000.);

  int iter=0;
  float stepu = 1.e-4;
  int nSteps = (int) (1./stepu+1);
  
  TNtuple *knots = new TNtuple("knots","knots", "u:f");
  double diff=0;
  for( int i=0; i<nu; i++ ){
    double u = gridU.getKnot( i ).u;
    double f0 = data0[i];
    knots->Fill( u, f0 );
    double fs = spline.getSpline( data, u );
    diff+=(fs-f0)*(fs-f0);
  }
  cout<<"mean diff at knots: "<<sqrt(diff)/nu<<endl;

  knots->SetMarkerSize(1.);
  knots->SetMarkerStyle(8);
  knots->SetMarkerColor(kRed);

  TNtuple *n = new TNtuple("n","n","u:f0:fs");

  for( float u=0; u<=1.; u+=stepu ){
    iter++;
    float f = spline.getSpline( data, u );
    qaX->Fill( 1.e4*(f - F(u)) );
    n->Fill(u,F(u),f);
  }

  
  /*
canv->cd(1);
  qaX->Draw();
  canv->cd(2);
  */
  n->SetMarkerColor(kBlack);
  //n->Draw("f0:u");
  n->SetMarkerColor(kBlue); 
  n->Draw("fs:u","","");
  knots->Draw("f:u","","same");

  canv->Update();

  return 0;

}



