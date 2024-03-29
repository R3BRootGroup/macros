
typedef struct EXT_STR_h101_t
{
    EXT_STR_h101_unpack_t unpack;
    EXT_STR_h101_raw_nnp_onion_t nnp;
} EXT_STR_h101;

void run(Int_t runNumber)
{
    TStopwatch timer;
    timer.Start();

    TString strRunNumber = "run";
    strRunNumber += runNumber;   
    const Int_t nev = -1;                                // number of events to read, -1 - untill CTRL+C
    TString inDir = "/Users/kresan/data/s438b/lmd/";     // directory with lmd files
    TString outDir = "/Users/kresan/data/s438b/data/";   // output directory

    TString filename = inDir + strRunNumber + "_*.lmd";
    TString outputFileName = outDir + strRunNumber + "_raw.root";                  // name of output file
    TString parFileName = outDir + "params_" + strRunNumber + "_raw.root";         // name of parameter file

    // Create source with unpackers ----------------------------------------------
    TString ntuple_options = "UNPACK:EVENTNO,UNPACK:TRIGGER,RAW";
    TString ucesb_dir = getenv("UCESB_DIR");
    TString ucesb_path = ucesb_dir + "/../upexps/s438b/s438b";

    EXT_STR_h101 ucesb_struct;
    R3BUcesbSource* source = new R3BUcesbSource(filename, ntuple_options,
                                                ucesb_path, &ucesb_struct, sizeof(ucesb_struct));
    source->SetMaxEvents(nev);
    source->AddReader(new R3BUnpackReader((EXT_STR_h101_unpack*)&ucesb_struct.unpack, offsetof(EXT_STR_h101, unpack)));
    source->AddReader(new R3BNeulandTacquilaReader((EXT_STR_h101_raw_nnp*)&ucesb_struct.nnp, offsetof(EXT_STR_h101, nnp)));
    // ---------------------------------------------------------------------------

    // Create online run ---------------------------------------------------------
    FairRunOnline* run = new FairRunOnline(source);
    run->SetRunId(runNumber);
    run->SetOutputFile(outputFileName.Data());
    //run->ActivateHttpServer(100000);
    // ---------------------------------------------------------------------------

    // Create ALADIN field map ---------------------------------------------------
    R3BAladinFieldMap* magField = new R3BAladinFieldMap("AladinMaps");
    Double_t fMeasCurrent = 2500.; // I_current [A]
    magField->SetCurrent(fMeasCurrent);
    magField->SetScale(1.);
    run->SetField(magField);
    // ---------------------------------------------------------------------------

    // Histogramming task --------------------------------------------------------
    R3BNeulandMappedHist* ana = new R3BNeulandMappedHist("LandRawAna", 1);
    run->AddTask(ana);
    // ---------------------------------------------------------------------------

    // Initialize ----------------------------------------------------------------
    run->Init();
    FairLogger::GetLogger()->SetLogScreenLevel("info");
    // ---------------------------------------------------------------------------

    // Runtime data base ---------------------------------------------------------
    FairRuntimeDb* rtdb = run->GetRuntimeDb();
    R3BFieldPar* fieldPar = (R3BFieldPar*)rtdb->getContainer("R3BFieldPar");
    fieldPar->SetParameters(magField);
    fieldPar->setChanged();
    Bool_t kParameterMerged = kTRUE;
    FairParRootFileIo* parOut = new FairParRootFileIo(kParameterMerged);
    parOut->open(parFileName);
    rtdb->setOutput(parOut);
    rtdb->print();
    // ---------------------------------------------------------------------------

    // Run -----------------------------------------------------------------------
    if(nev < 0)
    {
        run->Run(nev, 0);
    }
    else
    {
        run->Run(0, nev);
    }
    rtdb->saveOutput();
    delete run;
    // ---------------------------------------------------------------------------

    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << endl << endl;
    cout << "Macro finished succesfully." << endl;
    cout << "Output file is " << outputFileName << endl;
    cout << "Parameter file is " << parFileName << endl;
    cout << "Real time " << rtime << " s, CPU time " << ctime << "s" << endl << endl;
}

