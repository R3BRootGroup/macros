/* Additional info:
 * To generate the header file used for the R3BUcesbSource (ext_h101.h), use:
 *
 * ./201902_s444 --ntuple=RAW:CALIFA,id=h101_CALIFA,ext_h101_raw_califa_febex.h
 *
 * at $UCESB_DIR/upexps/201902_s444
 *
 * Put the header file into the 'r3bsource' directory and recompile
 *
 */

typedef struct EXT_STR_h101_t {
  EXT_STR_h101_unpack_t unpack;
  EXT_STR_h101_CALIFA_t califa;
} EXT_STR_h101;

void califa_unpack() {
  TStopwatch timer;
  timer.Start();
  
  const Int_t nev = -1; /* number of events to read, -1 - until CTRL+C */
  
  /* Create source using ucesb for input ------------------ */
  TString filename = "--stream=lxg0898:6002";
  //TString filename = "~/lmd/data_0001.lmd";
  TString outputFileName = "./datamap_0001.root";


  /* Create source using ucesb for input ------------------ */
  //UCESB paths
  TString ntuple_options = "UNPACK:EVENTNO,UNPACK:TRIGGER,RAW";
  TString ucesb_dir = getenv("UCESB_DIR");
  TString ucesb_path = ucesb_dir + "/../upexps/201902_s444/201902_s444";
  ucesb_path.ReplaceAll("//","/");

  EXT_STR_h101 ucesb_struct;
  
  R3BUcesbSource* source = new R3BUcesbSource(filename, ntuple_options,
					      ucesb_path, &ucesb_struct, sizeof(ucesb_struct));
  source->SetMaxEvents(nev);
  
  source->AddReader(new R3BUnpackReader((EXT_STR_h101_unpack*)&ucesb_struct,
					offsetof(EXT_STR_h101, unpack)));
  source->AddReader(new R3BCalifaFebexReader((EXT_STR_h101_CALIFA*)&ucesb_struct.califa,
					     offsetof(EXT_STR_h101, califa)));

  
  /* Create online run ------------------------------------ */
  FairRunOnline* run = new FairRunOnline(source);
  run->SetRunId(1);
  run->SetSink(new FairRootFileSink(outputFileName));

  
  /* Add analysis task ------------------------------------ */
 
  
  /* Initialize ------------------------------------------- */
  run->Init();
  //    FairLogger::GetLogger()->SetLogScreenLevel("warn");
  //    FairLogger::GetLogger()->SetLogScreenLevel("debug");
  FairLogger::GetLogger()->SetLogScreenLevel("info");


  /* Runtime data base ------------------------------------ */
  FairRuntimeDb* rtdb = run->GetRuntimeDb();

  
  /* Run -------------------------------------------------- */
  run->Run((nev < 0) ? nev : 0, (nev < 0) ? 0 : nev);


  /* Finish ----------------------------------------------- */
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is " << outputFileName << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s"
       << endl << endl;
  gApplication->Terminate();
}
