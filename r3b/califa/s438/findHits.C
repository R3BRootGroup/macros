void findHits(TString inputFile="", TString outputFile="", Int_t nEvents = 0)
{
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  // -----   Create analysis run   ----------------------------------------
  FairRunAna* fRun = new FairRunAna();
  fRun->SetSource(new FairFileSource(inputFile));
  fRun->SetOutputFile(outputFile);

  // Hit finder
  R3BCalifaCrystalCal2Cluster *hitFinder = new R3BCalifaCrystalCal2Cluster();
  // Select s438b Demonstrator Geometry
  hitFinder->SelectGeometryVersion(0x438b);
  hitFinder->SetSquareWindowAlg(6.0*TMath::Pi()/180.0, 6.0*TMath::Pi()/180.0);    //[0.25 around 14.3 degrees, 3.2 for the complete calorimeter]
  fRun->AddTask(hitFinder);


  fRun->Init();
  FairLogger::GetLogger()->SetLogScreenLevel("info");
//  FairLogger::GetLogger()->SetLogVerbosityLevel("HIGH");

  fRun->Run(0,nEvents);

  delete fRun;

  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------

}
