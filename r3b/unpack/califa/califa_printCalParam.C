/* Additional info:
 * To generate the header file used for the R3BUcesbSource (ext_h101.h), use:
 *
 * ./201902_s444 --ntuple=RAW:CALIFA,id=h101_CALIFA,ext_h101_raw_califa_febex.h
 *
 * at $UCESB_DIR/upexps/201902_s444
 *
 * Put the header file into the 'r3bsource' directory and recompile.
 *
 */
#include <iostream>
#include <iomanip>


void califa_printCalParam() {


  /* Create Instance of Runtime data base ------------------------------------------- */
  FairRuntimeDb* rtdb = FairRuntimeDb::instance();
  /* For invoking the *.root file, set #if 1; for invoking a *.par file, set #if 0 ---*/
#if 1
  
  auto parIo1 = new FairParRootFileIo();//Root
  parIo1->open("Califa_CalParam_20190218.root","in"); //Insert the *.root file name
#else
  auto parIo1 = new FairParAsciiFileIo();//Ascii
  parIo1->open("cal_AmBe.par","in"); //Insert the *.par file name
#endif
  rtdb->setFirstInput(parIo1);
  /* "califaCrystalCalPar" is th ename of the parameter container---------------------*/
  auto fCal_Par = dynamic_cast<R3BCalifaCrystalCalPar*>(rtdb->getContainer("califaCrystalCalPar"));
  rtdb->initContainers(1);

  auto NumCrystals = fCal_Par->GetNumCrystals();    // Number of Crystals
  auto NumParams = fCal_Par->GetNumParametersFit(); // Number of Parameters
  auto fCalParams = fCal_Par->GetCryCalParams(); // Array with the Cal parameters
  cout << "NumCrystals: " << NumCrystals << endl;
  cout << "NumParams: " << NumParams << endl;
  cout << "params: " << fCalParams << endl;

  /* Two ways to execute macro:
     #if 0: just execute macro with "root -l califa_printCalParam.C"
     #if 1: execute macro by redirecting the output to a file, eg.: "root -l califa_printCalParam.C  > some_file.txt"
  */
#if 1
  fCal_Par->printParams();
#else
  for(auto i=0; i< NumCrystals*NumParams; i++)
     if (fCalParams->At(i))
     cout << i << "-> " << (fCalParams->At(i)) << endl;
#endif
  cout << "Macro finished succesfully." << endl;
  gApplication->Terminate();
}
