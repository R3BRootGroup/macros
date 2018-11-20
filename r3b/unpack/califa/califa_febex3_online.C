/* Additional info:
 * To generate the header file used for the R3BUcesbSource (ext_h101.h), use:
 *
 * ./201810_s444 --ntuple=RAW:SST,id=h101_CALIFA,ext_h101_califa.h
 *
 * at $UCESB_DIR/upexps/201810_s444
 *
 * Put this header file into the 'r3bsource' directory and recompile.
 *
 * @since October 23th, 2018
 * */

typedef struct EXT_STR_h101_t {
  EXT_STR_h101_unpack_t unpack;
  EXT_STR_h101_CALIFA_t califa;
} EXT_STR_h101;


void califa_febex3_online(){
  TStopwatch timer;
  timer.Start();

  auto t = std::time(nullptr);
  auto tm = *std::localtime(&t);
  //std::cout << put_time(&tm, "%d_%m_%Y_%H_%M_%S") << std::endl;

  std::ostringstream oss;
  oss << std::put_time(&tm, "%d_%m_%Y_%H_%M_%S");

  const Int_t nev = -1; /* number of events to read, -1 - until CTRL+C */
  
  // Create source using ucesb for input ---------------------------------------
  TString filename = "--stream=lxg0898:6002";
  //TString filename = "~/lmd/data_0010.lmd";
  TString outputFileName = "./data_online_" + oss.str() + ".root";
  TString ntuple_options = "UNPACK:EVENTNO,UNPACK:TRIGGER,RAW";
  TString ucesb_dir = getenv("UCESB_DIR");
  TString ucesb_path = ucesb_dir + "/../upexps/201810_s444/201810_s444";

  EXT_STR_h101 ucesb_struct;
  
  R3BUcesbSource* source = new R3BUcesbSource(filename, ntuple_options,
					      ucesb_path, &ucesb_struct, sizeof(ucesb_struct));
  source->SetMaxEvents(nev);
  
  source->AddReader(new R3BUnpackReader((EXT_STR_h101_unpack*)&ucesb_struct,
					offsetof(EXT_STR_h101, unpack)));

  R3BCalifaFebexReader *unpackcalifa = new R3BCalifaFebexReader((EXT_STR_h101_CALIFA*)&ucesb_struct.califa,
					     offsetof(EXT_STR_h101, califa));

  unpackcalifa->SetOnline(true);
  source->AddReader(unpackcalifa);
  
  // Create online run ---------------------------------------------------------
  FairRunOnline* run = new FairRunOnline(source);
  run->SetOutputFile(outputFileName);
  Int_t refresh = 2000;
  Int_t port=8044;
  run->ActivateHttpServer(refresh,port);
  
  // Create analysis task ------------------------------------------------------

  //R3BCalifaMapped2CrystalCal ---
  //R3BCalifaMapped2CrystalCal* Map2Cal = new R3BCalifaMapped2CrystalCal();
  //run->AddTask(Map2Cal);

  // R3BOnlineSpectra ----------------------------------------------------------
  Int_t petals=7;
  //
  R3BCalifaOnlineSpectra* online= new R3BCalifaOnlineSpectra();
  online->SetPetals(petals);
  run->AddTask(online);
  
  // Initialize ----------------------------------------------------------------
  run->Init();
  FairLogger::GetLogger()->SetLogScreenLevel("INFO");
  
  // Runtime data base ---------------------------------------------------------
  FairRuntimeDb* rtdb = run->GetRuntimeDb();
  //Choose Root or Ascii file	
  //1-Root file with the Calibartion Parameters
  /*FairParRootFileIo* parIo1 = new FairParRootFileIo();
  parIo1->open("Califa_CalibParam.root","in");
  rtdb->setFirstInput(parIo1);
  */
  //2-Ascii file with the Calibartion Parameters
  /*FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
    parIo1->open("Califa_CalibParam.par","out");
    rtdb->setOutput(parIo1);
    rtdb->saveOutput();
  */
  

  /* Run -------------------------------------------------- */
  run->Run((nev < 0) ? nev : 0, (nev < 0) ? 0 : nev);


  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished successfully." << endl;
  cout << "Output file is " << outputFileName << endl;
  cout << "Real time " << rtime << " s, CPU time " 
       << ctime << "s" << endl << endl;
  //gApplication->Terminate();
}
