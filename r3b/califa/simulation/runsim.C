void runsim(Int_t nEvents = 0)
{

    // =========== Configuration area =============================

    TString OutFile = "sim_out.root"; // Output file for data
    TString ParFile = "sim_par.root"; // Output file for params

    Bool_t fVis = true;             // Store tracks for visualization
    Bool_t fUserPList = false;      // Use of R3B special physics list
    Bool_t fR3BMagnet = true;       // Magnetic field definition
    Bool_t fCalifaDigitizer = true; // Apply hit digitizer task
    Bool_t fCalifaClusterFinder = true; // Apply hit finder task

    TString fMC = "TGeant4";       // MonteCarlo engine: TGeant3, TGeant4, TFluka
    TString fGenerator = "gammas"; // Event generator type: box, gammas, r3b, ion, ascii
    TString fEventFile = "";       // Input event file in the case of ascii generator

    Int_t fFieldMap = -1;          // Magentic field map selector
    Double_t fMeasCurrent = 2000.; // Magnetic field current
    Int_t fFieldScale = 1;         // Magnetic field scale factor

    // ---------------  Detector selection: true - false -------------------------------

    Bool_t fTarget = false;      // Target
    TString fTargetType = "LiH"; // Target selection: LeadTarget, Para, Para45, LiH

    Bool_t fVesselcool = false; // SiTracker Cooling
    TString fVesselcoolGeo = "vacvessel_v13a.geo.root";

    Bool_t fAladin = false; // Aladin Magnet
    TString fAladinGeo = "aladin_v13a.geo.root";

    Bool_t fGlad = false; // Glad Magnet
    TString fGladGeo = "glad_v17_flange.geo.root";

    Bool_t fXBall = false; // Crystal Ball
    TString fXBallGeo = "cal_v13a.geo.root";
    Bool_t fCalifa = true;          // Califa Calorimeter
    Bool_t CalifaExpConfig = false; // Experimental Set-Up for CALIFA
    TString fCalifaGeo;
    Int_t fCalifaGeoVer;
    Double_t fCalifaNonU;
    TString califaSimParamsFile;

    if (CalifaExpConfig)
    {

        fCalifaGeo =
            "califa_v2019.11.geo.root"; // Experimental Set-Up: 1204 Crystals (1024 Barrel + 180 IPhos), Jan - Feb 2020
        fCalifaGeoVer = 2020;
        fCalifaNonU = 1.0; // Non-uniformity: 1 means +-1% max deviation
    }

    else
    {

        fCalifaGeo = "califa_v2021.3.geo.root";
        fCalifaGeoVer = 2021;
        fCalifaNonU = 1.0; // Non-uniformity: 1 means +-1% max deviation
    }

    Bool_t fTracker = false; // Tracker
    TString fTrackerGeo = "ams_s444.geo.root";

    //Bool_t fStarTrack = false; // STaR Tracker
    //TString fStarTrackGeo = "startrack_v13a.geo.root";

    Bool_t fDch = false; // Drift Chambers
    TString fDchGeo = "dch_v13a.geo.root";

    Bool_t fTof = false; // ToF Detector
    TString fTofGeo = "tof_v13a.geo.root";

    // Bool_t fmTof = false; // mTof Detector
    // TString fmTofGeo = "mtof_v13a.geo.root";

    Bool_t fGfi = false; // Fiber Detector
    TString fGfiGeo = "gfi_v13a.geo.root";

    Bool_t fLand = false; // Land Detector
    TString fLandGeo = "land_v12a_10m.geo.root";

    Bool_t fNeuLand = false; // NeuLand Detector
    TString fNeuLandGeo = "neuland_v12a_14m.geo.root";

    Bool_t fMfi = false; // MFI Detector
    TString fMfiGeo = "mfi_v13a.geo.root";

    Bool_t fPsp = false; // PSP Detector
    TString fPspGeo = "psp_v13a.geo.root";

    Bool_t fLumon = false; // Luminosity Detector
    TString fLumonGeo = "lumon_v13a.geo.root";

    // ========= End of Configuration area =======================

    TString dir = gSystem->Getenv("VMCWORKDIR");
    TString r3bdir = dir + "/macros/";
    r3bdir.ReplaceAll("//", "/");

    TString r3b_geomdir = dir + "/geometry/";
    gSystem->Setenv("GEOMPATH", r3b_geomdir.Data());
    r3b_geomdir.ReplaceAll("//", "/");

    TString r3b_confdir = dir + "/gconfig/";
    gSystem->Setenv("CONFIG_DIR", r3b_confdir.Data());
    r3b_confdir.ReplaceAll("//", "/");

    if (CalifaExpConfig)
    {

        califaSimParamsFile = r3bdir + "/r3b/califa/CalifaExpPars4Sim.par";
        califaSimParamsFile.ReplaceAll("//", "/");
    }

    // ----    Debug option   -------------------------------------------------
    gDebug = 0;
    // ------------------------------------------------------------------------

    // -----   Timer   --------------------------------------------------------
    TStopwatch timer;
    timer.Start();
    // ------------------------------------------------------------------------

    // -----   Create simulation run   ----------------------------------------
    FairRunSim* run = new FairRunSim();
    run->SetName(fMC);           // Transport engine
    run->SetOutputFile(OutFile); // Output file

    //  R3B Special Physics List in G4 case
    if ((fUserPList) && (fMC.CompareTo("TGeant4") == 0))
    {
        run->SetUserConfig("g4R3bConfig.C");
        run->SetUserCuts("SetCuts.C");
    }

    // -----   Create media   -------------------------------------------------
    run->SetMaterials("media_r3b.geo"); // Materials

    // -----   Create R3B geometry --------------------------------------------

    // Cave definition
    FairModule* cave = new R3BCave("CAVE");
    cave->SetGeometryFileName("r3b_cave.geo");
    run->AddModule(cave);

    // Target definition
    if (fTarget)
    {
        run->AddModule(new R3BTarget(fTargetType, "target_" + fTargetType + ".geo.root"));
    }

    // SiTracker Cooling definition
    if (fVesselcool)
    {
        run->AddModule(new R3BVacVesselCool(fTargetType, fVesselcoolGeo));
    }

    // Aladin Magnet definition
    if (fAladin && !fGlad)
    {
        fFieldMap = 0;
        run->AddModule(new R3BAladinMagnet(fAladinGeo));
    }

    // Glad Magnet definition
    if (fGlad && !fAladin)
    {
        fFieldMap = 1;
        run->AddModule(new R3BGladMagnet(fGladGeo));
    }

    // Crystal Ball
    if (fXBall && !fCalifa)
    {
        run->AddModule(new R3BXBall(fXBallGeo));
    }

    // CALIFA Calorimeter
    if (fCalifa && !fXBall)
    {
        R3BCalifa* califa = new R3BCalifa(fCalifaGeo);
        califa->SelectGeometryVersion(fCalifaGeoVer);
        run->AddModule(califa);
    }

    // Tracker
    if (fTracker)
    {
        run->AddModule(new R3BTra(fTrackerGeo));
    }

    // STaRTrack
    /*if (fStarTrack)
    {
        run->AddModule(new R3BStartrack(fStarTrackGeo));
    }*/

    // DCH drift chambers
    if (fDch)
    {
        run->AddModule(new R3BDch(fDchGeo));
    }

    // Tof
    if (fTof)
    {
        run->AddModule(new R3BTof(fTofGeo));
    }

    // mTof
    // if (fmTof)
    //{
    //    run->AddModule(new R3BmTof(fmTofGeo));
    //}

    // GFI detector
    if (fGfi)
    {
        run->AddModule(new R3BGfi(fGfiGeo));
    }

    // Land Detector
    if (fLand && !fNeuLand)
    {
        run->AddModule(new R3BLand(fLandGeo));
    }

    // NeuLand Scintillator Detector
    if (fNeuLand && !fLand)
    {
        run->AddModule(new R3BLand(fNeuLandGeo));
    }

    // MFI Detector
    if (fMfi)
    {
        run->AddModule(new R3BMfi(fMfiGeo));
    }

    // PSP Detector
    if (fPsp)
    {
        run->AddModule(new R3BPsp(fPspGeo));
    }

    // Luminosity detector
    if (fLumon)
    {
        run->AddModule(new ELILuMon(fLumonGeo));
    }

    // -----   Create R3B  magnetic field ----------------------------------------
    // NB: <D.B>
    // If the Global Position of the Magnet is changed
    // the Field Map has to be transformed accordingly
    if (fFieldMap == 0)
    {
        R3BAladinFieldMap* magField = new R3BAladinFieldMap("AladinMaps");
        magField->SetCurrent(fMeasCurrent);
        magField->SetScale(fFieldScale);

        if (fR3BMagnet == kTRUE)
        {
            run->SetField(magField);
        }
        else
        {
            run->SetField(NULL);
        }
    }
    else if (fFieldMap == 1)
    {
        R3BGladFieldMap* magField = new R3BGladFieldMap("R3BGladMap");
        magField->SetScale(fFieldScale);

        if (fR3BMagnet == kTRUE)
        {
            run->SetField(magField);
        }
        else
        {
            run->SetField(NULL);
        }
    } //! end of field map section

    // -----   Create PrimaryGenerator   --------------------------------------

    // 1 - Create the Main API class for the Generator
    FairPrimaryGenerator* primGen = new FairPrimaryGenerator();

    if (fGenerator.CompareTo("box") == 0)
    {
        // 2- Define the BOX generator
        Double_t pdgId = 211;  // pion beam
        Double_t theta1 = 30.; // polar angle distribution
        Double_t theta2 = 160.;
        Double_t momentum = .8; // 10 GeV/c
        FairBoxGenerator* boxGen = new FairBoxGenerator(pdgId, 10);
        boxGen->SetThetaRange(theta1, theta2);
        boxGen->SetPRange(momentum, momentum * 2.);
        boxGen->SetPhiRange(0., 360.);
        boxGen->SetXYZ(0.0, 0.0, 0.0);
        // add the box generator
        primGen->AddGenerator(boxGen);
    }
    if (fGenerator.CompareTo("gammas") == 0)
    {
        // 2- Define the CALIFA Test gamma generator
        Double_t pdgId = 22;       // 22 for gamma emission, 2212 for proton emission
        Double_t theta1 = 0.;      // polar angle distribution: lower edge (50)
        Double_t theta2 = 180.;    // polar angle distribution: upper edge (51)
        Double_t momentum = 0.003; // GeV/c
        Int_t multiplicity = 3;
        R3BCALIFATestGenerator* gammasGen = new R3BCALIFATestGenerator(pdgId, multiplicity);
        gammasGen->SetThetaRange(theta1, theta2);
        gammasGen->SetCosTheta();
        gammasGen->SetPRange(momentum, momentum);
        gammasGen->SetPhiRange(0., 360.); //(2.5,4)
        gammasGen->SetBoxXYZ(-0.1, 0.1, -0.1, 0.1, -0.1, 0.1);
        gammasGen->SetLorentzBoost(0.0); // beta=0.8197505718204776 for 700 A MeV
        // add the gamma generator
        primGen->AddGenerator(gammasGen);
    }
    run->SetGenerator(primGen);

    //-------Set visualisation flag to true------------------------------------
    run->SetStoreTraj(fVis);

    FairRuntimeDb* rtdb = run->GetRuntimeDb();

    if (CalifaExpConfig)
    {
        FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo(); // Ascii file
        parIo1->open(califaSimParamsFile, "in");

        rtdb->setFirstInput(parIo1);
        rtdb->print();
    }

    FairLogger::GetLogger()->SetLogVerbosityLevel("LOW");

    // ----- Initialize CalifaDigitizer task (from Point Level to Cal Level)
    if (fCalifaDigitizer)
    {
        R3BCalifaDigitizer* califaDig = new R3BCalifaDigitizer();
        califaDig->SetNonUniformity(fCalifaNonU);
        califaDig->SetRealConfig(CalifaExpConfig); // Real Configuration goes here
        califaDig->SetExpEnergyRes(6.);            // 5. means 5% at 1 MeV
        califaDig->SetComponentRes(6.);
        califaDig->SetDetectionThreshold(0.000010); // in GeV!! 0.000010 means 10 keV
        califaDig->SetParContainers();              // Only if SetRealConfig is set to TRUE!
        run->AddTask(califaDig);
    }

    // ----- Initialize Califa HitFinder task (from CrystalCal Level to Hit Level)
    if (fCalifaClusterFinder)
    {
      R3BCalifaCrystalCal2Cluster* CalifaCal2Cluster = new R3BCalifaCrystalCal2Cluster();
        CalifaCal2Cluster->SetRandomization(kFALSE);
        //CalifaCal2Cluster->SetClusterWindow(0.25);
        CalifaCal2Cluster->SetCrystalThreshold(0.0001); // 100keV
        //CalifaCal2Cluster->SetGammaClusterThreshold(0.002);
        //CalifaCal2Cluster->SetProtonClusterThreshold(0.03);
        //CalifaCal2Cluster->IsSimulation(kTRUE);
        run->AddTask(CalifaCal2Cluster);
    }

    // -----   Runtime database   ---------------------------------------------

    // -----   Initialize simulation run   ------------------------------------
    run->Init();

    Bool_t kParameterMerged = kTRUE;
    FairParRootFileIo* parOut = new FairParRootFileIo(kParameterMerged);
    parOut->open(ParFile.Data());
    rtdb->setOutput(parOut);
    rtdb->saveOutput();
    rtdb->print();

    // -----   Start run   ----------------------------------------------------
    if (nEvents > 0)
        run->Run(nEvents);

    // -----   Finish   -------------------------------------------------------
    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << endl << endl;
    cout << "Macro finished succesfully." << endl;
    cout << "Output file is " << OutFile << endl;
    cout << "Parameter file is " << ParFile << endl;
    cout << "Real time " << rtime << " s, CPU time " << ctime << "s" << endl << endl;
    // ------------------------------------------------------------------------

    std::cout << "Macro finished successfully." << std::endl;
}
