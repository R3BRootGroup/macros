#include <iomanip>
#include <iostream>
#include "TGeoManager.h"
#include "TMath.h"




// Create Matrix Unity
TGeoRotation *fGlobalRot = new TGeoRotation();

// Create a null translation
TGeoTranslation *fGlobalTrans = new TGeoTranslation();
TGeoRotation *fRefRot = NULL;

TGeoManager*   gGeoMan           = NULL;




Double_t fThetaX = 0.;
Double_t fThetaY = 0.;
Double_t fThetaZ = 0.;
Double_t fPhi   = 0.;
Double_t fTheta = 0.;
Double_t fPsi   = 0.;
Double_t fX = 0.;
Double_t fY = 0.;
Double_t fZ = 0.;
Bool_t fLocalTrans = kFALSE;
Bool_t fLabTrans = kFALSE;

TGeoCombiTrans* GetGlobalPosition(TGeoCombiTrans *fRef);


void create_califa_single_crystal2(const char* geoTag = "teetos2")
{

  fGlobalTrans->SetTranslation(0.0,0.0,0.0);

  // -------   Load media from media file   -----------------------------------
  FairGeoLoader*    geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
  FairGeoInterface* geoFace = geoLoad->getGeoInterface();
  TString geoPath = gSystem->Getenv("VMCWORKDIR");
  TString medFile = geoPath + "/geometry/media_r3b.geo";
  geoFace->setMediaFile(medFile);
  geoFace->readMedia();
  gGeoMan = gGeoManager;
  // --------------------------------------------------------------------------



  // -------   Geometry file name (output)   ----------------------------------
  TString geoFileName = geoPath + "/geometry/califa_";
  geoFileName = geoFileName + geoTag + ".geo.root";
  // --------------------------------------------------------------------------



  // -----------------   Get and create the required media    -----------------
  FairGeoMedia*   geoMedia = geoFace->getMedia();
  FairGeoBuilder* geoBuild = geoLoad->getGeoBuilder();

  FairGeoMedium* mAir      = geoMedia->getMedium("Air");
  if ( ! mAir ) Fatal("Main", "FairMedium Air not found");
  geoBuild->createMedium(mAir);
  TGeoMedium* pAirMedium = gGeoMan->GetMedium("Air");
  if ( ! pAirMedium ) Fatal("Main", "Medium Air not found");

  FairGeoMedium* mCsI      = geoMedia->getMedium("CsI");
  if ( ! mCsI ) Fatal("Main", "FairMedium CsI not found");
  geoBuild->createMedium(mCsI);
  TGeoMedium* pCsIMedium = gGeoMan->GetMedium("CsI");
  if ( ! pCsIMedium ) Fatal("Main", "Medium CsI not found");

  FairGeoMedium* mCar      = geoMedia->getMedium("CarbonFibre");
  if ( ! mCar ) Fatal("Main", "FairMedium CarbonFibre not found");
  geoBuild->createMedium(mCar);
  TGeoMedium* pCarbonFibreMedium = gGeoMan->GetMedium("CarbonFibre");
  if ( ! pCarbonFibreMedium ) Fatal("Main", "Medium CarbonFibre not found");

  FairGeoMedium* mLaCl      = geoMedia->getMedium("LaCl");
  if ( ! mLaCl ) Fatal("Main", "FairMedium LaCl not found");
  geoBuild->createMedium(mLaCl);
  TGeoMedium* pLaClMedium = gGeoMan->GetMedium("LaCl");
  if ( ! pLaClMedium ) Fatal("Main", "Medium LaCl not found");

  FairGeoMedium* mLaBr      = geoMedia->getMedium("LaBr");
  if ( ! mLaBr ) Fatal("Main", "FairMedium LaBr not found");
  geoBuild->createMedium(mLaBr);
  TGeoMedium* pLaBrMedium = gGeoMan->GetMedium("LaBr");
  if ( ! pLaBrMedium ) Fatal("Main", "Medium LaBr not found");
  
  FairGeoMedium* mTfl      = geoMedia->getMedium("Tefflon");
  if ( ! mTfl ) Fatal("Main", "FairMedium Tefflon not found");
  geoBuild->createMedium(mTfl);
  TGeoMedium* pWrappingMedium = gGeoMan->GetMedium("Tefflon");
  if ( ! pWrappingMedium ) Fatal("Main", "Medium Tefflon not found");
  // --------------------------------------------------------------------------



  // --------------   Create geometry and top volume  -------------------------
  gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
  gGeoMan->SetName("CALIFAgeom");
  TGeoVolume* top = new TGeoVolumeAssembly("TOP");
  gGeoMan->SetTopVolume(top);
  // --------------------------------------------------------------------------




  //WORLD
  TGeoVolume *pAWorld  =  top;
  
  // Defintion of the Mother Volume
  Double_t length = 300.;
  TGeoShape *pCBWorldOut = new TGeoBBox("Califa_boxOut",
                                     length/2.0,
                                     length/2.0,
                                     length/2.0);


  TGeoShape *pCBWorldIn1 = new TGeoTube("Califa_Centerpart1",  // hole to accommodate the tracker
                                     0.,  // Rmin
                                     26.4, // Rmax
                                     190./2.0);  // half length
  TGeoCombiTrans *t_part1 = new TGeoCombiTrans("t_part1",0.,0.,-55,fRefRot);
  t_part1->RegisterYourself();

  TGeoShape *pCBWorldIn2 = new TGeoTube("Califa_Centerpart2",  // hole to accommodate the pipe through the end-cap
                                     0.,  // Rmin
                                     5.,  // Rmax
                                     110./2.0);  // half length
  TGeoCombiTrans *t_part2 = new TGeoCombiTrans("t_part2",0.,0.,95,fRefRot);
  t_part2->RegisterYourself();
 
  TGeoCompositeShape *pCBWorld = new TGeoCompositeShape("Califa_box", " Califa_boxOut - (Califa_Centerpart1:t_part1  + Califa_Centerpart2:t_part2)");

  TGeoVolume* pWorld = new TGeoVolume("CalifaWorld",pCBWorld, pAirMedium);
  
  TGeoCombiTrans *t0 = new TGeoCombiTrans();
  TGeoCombiTrans *pGlobalc = GetGlobalPosition(t0);
  
  // add the sphere as Mother Volume
  pAWorld->AddNode(pWorld, 0, pGlobalc);
  
  //finally the v7.05 code

  
  //Crystals with and without wrapping

  Double_t* vertices=new Double_t[16];
  Double_t* vertices2=new Double_t[16];


  //TEST TO ADD ONE CRYSTAL (BACALHAU)
  vertices[0] = 2.92525; vertices[1] = -0; 				 vertices2[0] = 2.95525; vertices2[1] = -0.03;
  vertices[2] = 5.8505; vertices[3] = -0; 				 vertices2[2] = 5.8205; vertices2[3] = -0.03;
  vertices[4] = 5.8505; vertices[5] = -1.535; 				 vertices2[4] = 5.8205; vertices2[5] = -1.505;
  vertices[6] = 2.92525; vertices[7] = -1.535; 				 vertices2[6] = 2.95525; vertices2[7] = -1.505;
  vertices[8] = 2.92525; vertices[9] = 0.71190775305; 				 vertices2[8] = 2.95525; vertices2[9] = 0.68190775305;
  vertices[10] = 7.48315401295; vertices[11] = 0.71190775305; 				 vertices2[10] = 7.45315401295; vertices2[11] = 0.68190775305;
  vertices[12] = 7.48315401295; vertices[13] = -1.535; 				 vertices2[12] = 7.45315401295; vertices2[13] = -1.505;
  vertices[14] = 2.92525; vertices[15] = -1.535; 				 vertices2[14] = 2.95525; vertices2[15] = -1.505;
  TGeoVolume *CrystalWithWrapping_4_1 = gGeoManager->MakeArb8("CrystalWithWrapping_4_1", pWrappingMedium,8.5, vertices);
  CrystalWithWrapping_4_1->SetLineColor(kGreen);
  TGeoVolume *Crystal_4_1 = gGeoManager->MakeArb8("Crystal_4_1", pCsIMedium,8.47, vertices2);
  Crystal_4_1->SetLineColor(kRed);
  

  Double_t phiEuler_4_1=0; Double_t thetaEuler_4_1=0; Double_t psiEuler_4_1=0;
  Double_t rotAngle_4_1=0;
  phiEuler_4_1 = 100;
  thetaEuler_4_1 = 0; //180.*TMath::ATan2(-0.6560740922919081600,-0.7546964856307127900)/TMath::Pi();
  psiEuler_4_1 = 0;
  
  //The right transformation describing the difference in the coordinate system in CAD and simulation
  TGeoRotation *rotDef_4_1 = new TGeoRotation();
  rotDef_4_1->RotateX(-90);
  TGeoTranslation* shiftDef_4_1=new TGeoTranslation("shiftDef_4_1",0.,10.,0.);
  TGeoCombiTrans* transDef_4_1 = new TGeoCombiTrans(*shiftDef_4_1,*rotDef_4_1);

  TGeoRotation *rotCrystal_4_1 = new TGeoRotation("rotCrystal_4_1",phiEuler_4_1,thetaEuler_4_1,psiEuler_4_1);

  rotAngle_4_1 = TMath::Pi()*11.25*20/180.;
  rotCrystal_4_1->RotateZ(-11.25);
  TGeoCombiTrans* transCrystal_4_1 = new TGeoCombiTrans(0,0,0,0); //new TGeoCombiTrans(0,0,0,rotCrystal_4_1);
  *transCrystal_4_1 =  (*transCrystal_4_1) * (*transDef_4_1);
  pWorld->AddNode(Crystal_4_1,20,transCrystal_4_1);

  
  
  //Some common geometrical operations
  TGeoRotation *rotUni = new TGeoRotation();          //unitary rotation
  TGeoTranslation* noTrans=new TGeoTranslation("noTrans",0.,0.,0.);
  TGeoRotation *rotSymmetric = new TGeoRotation(); //Symmetric crystal
  rotSymmetric->RotateZ(180);
  //The right transformation describing the difference in the coordinate system in CAD and simulation
  TGeoRotation *rotDef = new TGeoRotation();
  rotDef->RotateX(-90);
  TGeoTranslation* shiftDef=new TGeoTranslation("shiftDef",0.,10.,0.);
  TGeoCombiTrans* transDef = new TGeoCombiTrans(*shiftDef,*rotDef);
  
  TGeoRotation *rotDef1 = new TGeoRotation();
  rotDef1->RotateZ(-36.*1);
  TGeoCombiTrans* transDefRot1 = new TGeoCombiTrans(*noTrans,*rotDef1);
  
  TGeoRotation *rotDef2 = new TGeoRotation();
  rotDef2->RotateZ(-36.*2);
  TGeoCombiTrans* transDefRot2 = new TGeoCombiTrans(*noTrans,*rotDef2);
  
  TGeoRotation *rotDef3 = new TGeoRotation();
  rotDef3->RotateZ(-36.*3);
  TGeoCombiTrans* transDefRot3 = new TGeoCombiTrans(*noTrans,*rotDef3);
  
  TGeoRotation *rotDef4 = new TGeoRotation();
  rotDef4->RotateZ(-36.*4);
  TGeoCombiTrans* transDefRot4 = new TGeoCombiTrans(*noTrans,*rotDef4);
  
  TGeoRotation *rotDef5 = new TGeoRotation();
  rotDef5->RotateZ(-36.*5);
  TGeoCombiTrans* transDefRot5 = new TGeoCombiTrans(*noTrans,*rotDef5);
  
  TGeoRotation *rotDef6 = new TGeoRotation();
  rotDef6->RotateZ(-36.*6);
  TGeoCombiTrans* transDefRot6 = new TGeoCombiTrans(*noTrans,*rotDef6);
  
  TGeoRotation *rotDef7 = new TGeoRotation();
  rotDef7->RotateZ(-36.*7);
  TGeoCombiTrans* transDefRot7 = new TGeoCombiTrans(*noTrans,*rotDef7);
  
  TGeoRotation *rotDef8 = new TGeoRotation();
  rotDef8->RotateZ(-36.*8);
  TGeoCombiTrans* transDefRot8 = new TGeoCombiTrans(*noTrans,*rotDef8);
  
  TGeoRotation *rotDef9 = new TGeoRotation();
  rotDef9->RotateZ(-36.*9);
  TGeoCombiTrans* transDefRot9 = new TGeoCombiTrans(*noTrans,*rotDef9);
  

  CrystalWithWrapping_4_1->AddNode(Crystal_4_1,1,new TGeoCombiTrans(0,0,0,rotUni));
  pWorld->AddNode(CrystalWithWrapping_4_1,20,transCrystal_4_1);
  //AddSensitiveVolume(Crystal_4_1);



  
  //seguridad para que los alveolos no solapen
  double ss1=0.00;
  //double thicknessC0=0.025;
  double ss2=0.00;
  double ss3=0.00;
  double ss4=0.00;
  double ss5=0.00;
  double ss6=0.00;
  
  
  double thicknessW0=0.0025;
  //double thicknessC0=0.025;
  double thicknessC00=0.0025;
  double thickness1=0.0025+ss1;
  double thickness11=0.0025+ss1;
  double thickness2=0.0025+ss2;
  double thickness22=0.0025+ss2;
  double thickness3=0.0025+ss3;
  double thickness33=0.0025+ss3;
  double thickness4=0.0025+ss4;
  double thickness44=0.0025+ss4;
  double thickness5=0.0025+ss5;
  double thickness55=0.0025+ss5;
  double thickness6=0.0025+ss6;
  double thickness66=0.0025+ss6;
  
  

  

  // ---------------   Finish   -----------------------------------------------
  gGeoMan->CloseGeometry();
  gGeoMan->CheckOverlaps(0.001);
  gGeoMan->PrintOverlaps();
  gGeoMan->Test();

  TFile* geoFile = new TFile(geoFileName, "RECREATE");
  top->Write();
  geoFile->Close();
  // --------------------------------------------------------------------------
}




TGeoCombiTrans* GetGlobalPosition(TGeoCombiTrans *fRef)
{
  if (fLocalTrans == kTRUE ) {
    
    if ( ( fThetaX == 0 )  && ( fThetaY==0 )  && ( fThetaZ == 0 )
	 &&
	 ( fX == 0 ) && ( fY == 0 ) && ( fZ == 0 )
	 )  return fRef;
    
    
    // X axis
    Double_t xAxis[3] = { 1. , 0. , 0. };
    Double_t yAxis[3] = { 0. , 1. , 0. };
    Double_t zAxis[3] = { 0. , 0. , 1. };
    // Reference Rotation
    fRefRot = fRef->GetRotation();
    
    if (fRefRot) {
      Double_t mX[3] = {0.,0.,0.};
      Double_t mY[3] = {0.,0.,0.};
      Double_t mZ[3] = {0.,0.,0.};
      
      fRefRot->LocalToMasterVect(xAxis,mX);
      fRefRot->LocalToMasterVect(yAxis,mY);
      fRefRot->LocalToMasterVect(zAxis,mZ);
      
      Double_t a[4]={ mX[0],mX[1],mX[2], fThetaX };
      Double_t b[4]={ mY[0],mY[1],mY[2], fThetaY };
      Double_t c[4]={ mZ[0],mZ[1],mZ[2], fThetaZ };
      
      ROOT::Math::AxisAngle aX(a,a+4);
      ROOT::Math::AxisAngle aY(b,b+4);
      ROOT::Math::AxisAngle aZ(c,c+4);
      
      ROOT::Math::Rotation3D fMatX( aX );
      ROOT::Math::Rotation3D fMatY( aY );
      ROOT::Math::Rotation3D fMatZ( aZ );
      
      ROOT::Math::Rotation3D  fRotXYZ = (fMatZ * (fMatY * fMatX));
      
      //cout << fRotXYZ << endl;
      
      Double_t fRotable[9]={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
      fRotXYZ.GetComponents(
			    fRotable[0],fRotable[3],fRotable[6],
			    fRotable[1],fRotable[4],fRotable[7],
			    fRotable[2],fRotable[5],fRotable[8]
			    );
      TGeoRotation *pRot = new TGeoRotation();
      pRot->SetMatrix(fRotable);
      TGeoCombiTrans *pTmp = new TGeoCombiTrans(*fGlobalTrans,*pRot);
      
      // ne peut pas etre applique ici
      // il faut differencier trans et rot dans la multi.
      TGeoRotation rot_id;
      rot_id.SetAngles(0.0,0.0,0.0);
      
      TGeoCombiTrans c1;
      c1.SetRotation(rot_id);
      const Double_t *t = pTmp->GetTranslation();
      c1.SetTranslation(t[0],t[1],t[2]);
      
      TGeoCombiTrans c2;
      c2.SetRotation(rot_id);
      const Double_t *tt = fRefRot->GetTranslation();
      c2.SetTranslation(tt[0],tt[1],tt[2]);
      
      TGeoCombiTrans cc = c1 * c2 ;
      
      TGeoCombiTrans c3;
      c3.SetRotation(pTmp->GetRotation());
      TGeoCombiTrans c4;
      c4.SetRotation(fRefRot);
      
      TGeoCombiTrans ccc = c3 * c4;
      
      TGeoCombiTrans pGlobal;
      pGlobal.SetRotation(ccc.GetRotation());
      const Double_t *allt = cc.GetTranslation();
      pGlobal.SetTranslation(allt[0],allt[1],allt[2]);
      
      return  ( new TGeoCombiTrans( pGlobal ) );
      
    }else{
      
      cout << "-E- R3BDetector::GetGlobalPosition() \
	      No. Ref. Transformation defined ! " << endl;
      cout << "-E- R3BDetector::GetGlobalPosition() \
	      cannot create Local Transformation " << endl;
      return NULL;
    } //! fRefRot
    
  } else {
    // Lab Transf.
    if ( ( fPhi == 0 )  && ( fTheta==0 )  && ( fPsi == 0 )
	 &&
	 ( fX == 0 ) && ( fY == 0 ) && ( fZ == 0 )
	 )  return fRef;
    
    
    return ( new TGeoCombiTrans(*fGlobalTrans,*fGlobalRot) );
    
  }
}

