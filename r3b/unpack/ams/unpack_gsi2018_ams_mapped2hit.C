/* Additional info:
 * To generate the header file used for the R3BUcesbSource (ext_h101.h), use:
 *
 * ./201902_s444 --ntuple=RAW:SST,id=h101_AMS,ext_h101_ams.h
 *
 * at $UCESB_DIR/upexps/201902_s444
 *
 * Put this header file into the 'r3bsource' directory and recompile.
 * */

 typedef struct EXT_STR_h101_t {
   EXT_STR_h101_unpack_t unpack;
   EXT_STR_h101_AMS_t ams;
 } EXT_STR_h101;

void unpack_gsi2018_ams_mapped2hit() {
  TStopwatch timer;
  timer.Start();

  //const Int_t nev = -1; /* number of events to read, -1 - until CTRL+C */
  const Int_t nev = -1; /* number of events to read, -1 - until CTRL+C */
  
  /* Create source using ucesb for input ------------------ */
  
  TString filename = "~/lmd/ams.lmd";
  TString outputFileName = "data_0001.root";
  
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
  source->AddReader(new R3BAmsReader((EXT_STR_h101_AMS*)&ucesb_struct.ams,
					     offsetof(EXT_STR_h101, ams)));

  
  /* Create online run ------------------------------------ */
  FairRunOnline* run = new FairRunOnline(source);
  run->SetRunId(1);
  run->SetSink(new FairRootFileSink(outputFileName));


  /* Runtime data base ------------------------------------ */
  FairRuntimeDb* rtdb = run->GetRuntimeDb();


  /* Add analysis task ------------------------------------ */  
  R3BAmsMapped2StripCal* Map2Cal = new R3BAmsMapped2StripCal();
  run->AddTask(Map2Cal);
  R3BAmsStripCal2Hit* Cal2Hit = new R3BAmsStripCal2Hit();
  run->AddTask(Cal2Hit);


  /* Load parameters   ------------------------------------ */ 
  FairParRootFileIo* parIo1 = new FairParRootFileIo();
  parIo1->open("Ams_CalibParam.root","in");
  rtdb->setFirstInput(parIo1);
  rtdb->print();


  /* Initialize ------------------------------------------- */
  run->Init();
  //    FairLogger::GetLogger()->SetLogScreenLevel("warn");
  //    FairLogger::GetLogger()->SetLogScreenLevel("debug");
  FairLogger::GetLogger()->SetLogScreenLevel("info");


  /* Run -------------------------------------------------- */
  run->Run((nev < 0) ? nev : 0, (nev < 0) ? 0 : nev);


  /* Save parameters (if needed) -------------------------- */
  //rtdb->saveOutput();


  /* Finish ----------------------------------------------- */
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  std::cout << std::endl << std::endl;
  std::cout << "Macro finished succesfully." << std::endl;
  std::cout << "Output file is " << outputFileName << std::endl;
  std::cout << "Real time " << rtime << " s, CPU time " << ctime << " s"
            << std::endl << std::endl;
  gApplication->Terminate();
}
