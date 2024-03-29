/* In order to generate input for this, please go to $UCESB_DIR and run:
 *
 * Additional info:
 * To generate the header file used for the R3BUcesbSource (ext_h101.h), use:
 *
 * $UCESB_DIR/upexps/s438b/s438b --ntuple=UNPACK:TRIGGER,UNPACK:EVENTNO,RAW\
 *     STRUCT_HH,ext_h101.h
 *
 * Put this header file into the 'r3bsource' directory and recompile.
 * */

/*
extern "C" {
//#include "/home/bloeher/git/R3BRoot/r3bsource/ext_h101_full.h"
//#include "ext_data_client.h"
#include "/u/syndikus/R3BRoot/r3bsource/ext_h101_los.h"
#include "/u/syndikus/R3BRoot/r3bsource/ext_h101_psp.h"
}
*/

typedef struct EXT_STR_h101_t
{
//    EXT_STR_h101_LOS_onion_t los;
    EXT_STR_h101_unpack_t unpack;
    EXT_STR_h101_PSP_onion_t psp;
} EXT_STR_h101;

void unpack_kvi2018_pspx_main(Int_t RunId = 17)
{
    TStopwatch timer;
    timer.Start();

    const Int_t nev = -1; /* number of events to read, -1 - until CTRL+C */
    // const Int_t nev = 100;

    /* Create source using ucesb for input ------------------ */
    TString filename;
    // TString lmddir;
    TString lmddir = "/u/syndikus/nyx/bloeher/data/kvi2018/lmd/";
    if (RunId < 83)
    {
        // lmddir = "/data/land2/kvi2018/lmd/";
        filename = Form(lmddir + "pspx_run%03d_*.lmd", RunId);
    }
    else
    {
        // lmddir = "/data/land3/kvi2018/lmd/";
        filename = Form(lmddir + "run%03d_*.lmd", RunId);
    }
    TString outputFileName;
    // TString outdir = "/data/land3/kvi2018/rootfiles/";
    TString outdir = "/u/syndikus/rootfiles/kvi2018/";
    if (RunId < 68)
    {
        outputFileName = Form(outdir + "pspx_run%03d_mapped_precal_cal_calib_clean_hit_default.root", RunId);
    }
    else if (RunId < 83)
    {
        outputFileName = Form(outdir + "pspx_run%03d_mapped_precal_cal_hit_default.root", RunId);
    }
    else 
    {
        outputFileName = Form(outdir + "run%03d_pspx_mapped_precal_cal_hit_default.root", RunId);
    }

    TString ntuple_options = "UNPACK:EVENTNO,UNPACK:TRIGGER,RAW,PSPX";
    // TString ucesb_dir = getenv("UCESB_DIR");
    // TString ucesb_path = ucesb_dir + "/../upexps/jun16Xe/jun16Xe";
    TString ucesb_path;
    // TString upexps_dir = "/home/land/upexps/kvi2018/";
    TString upexps_dir = "/lynx/Lynx/land/usr/land/landexp/kvi2018/upexps/kvi2018/";
    if (RunId<73)
    {
        ucesb_path = upexps_dir + "kvi2018_001_060 --allow-errors";
    } 
    else if (RunId == 73)
    {
        ucesb_path = upexps_dir + "kvi2018_pspx_073 --allow-errors";
    }
    else if (RunId > 74 && RunId<83)
    {
        ucesb_path = upexps_dir + "kvi2018_pspx_075_082 --allow-errors";
    }
    else
    {
        ucesb_path = upexps_dir + "kvi2018_069_101 --allow-errors";
    }

    // TString pspxpar_dir = "/home/land/R3BRoot_PSPX/psp/par/";
    TString pspxpar_dir = "/u/syndikus/R3BRoot/psp/par/";
    TString parPspxMappedFileName = "kvi2018_pspx_mapped.par";
    TString parPspxPrecalFileName;
    TString parPspxCalFileName;
    TString parPspxHitFileName;
    if (RunId < 63)
    {
        parPspxPrecalFileName = "kvi2018_pspx_precal_3126-9.par";
        parPspxCalFileName = "kvi2018_pspx_cal_3126-9.par";
        parPspxHitFileName = "kvi2018_pspx_hit_default.par";
    }
    else if (RunId < 68)
    {
        parPspxPrecalFileName = "kvi2018_pspx_precal_3126-7.par";
        parPspxCalFileName = "kvi2018_pspx_cal_3126-7.par";
        parPspxHitFileName = "kvi2018_pspx_hit_default.par";
    }
    else
    {
        parPspxPrecalFileName = "kvi2018_pspx_precal_default.par";
        parPspxCalFileName = "kvi2018_pspx_cal_default.par";
        parPspxHitFileName = "kvi2018_pspx_hit_default.par";
    }

    EXT_STR_h101 ucesb_struct;
    R3BUcesbSource* source =
        new R3BUcesbSource(filename, ntuple_options, ucesb_path, &ucesb_struct, sizeof(ucesb_struct));
    source->SetMaxEvents(nev);

    source->AddReader(new R3BUnpackReader((EXT_STR_h101_unpack*)&ucesb_struct.unpack, offsetof(EXT_STR_h101, unpack)));
    source->AddReader(new R3BPspxReader((EXT_STR_h101_PSP*)&ucesb_struct.psp, offsetof(EXT_STR_h101, psp)));

    /* Create online run ------------------------------------ */
    FairRunOnline* run = new FairRunOnline(source);
    run->SetRunId(RunId);

    // Create analysis run ----------------------------------
    // FairRunAna* run = new FairRunAna();
    run->SetOutputFile(outputFileName.Data());

    /* Runtime data base ------------------------------------ */
    FairRuntimeDb* rtdb1 = run->GetRuntimeDb();

    FairParAsciiFileIo* parInput = new FairParAsciiFileIo();
    TList* parList = new TList();
    parList->Add(new TObjString(pspxpar_dir + parPspxMappedFileName));
    parList->Add(new TObjString(pspxpar_dir + parPspxPrecalFileName));
    parList->Add(new TObjString(pspxpar_dir + parPspxCalFileName));
    parList->Add(new TObjString(pspxpar_dir + parPspxHitFileName));
    parInput->open(parList);
    rtdb1->setFirstInput(parInput);

    rtdb1->print();

    /* Add analysis task ------------------------------------ */

    R3BPspxMapped2Precal* pspxMapped2Precal = new R3BPspxMapped2Precal("PspxMapped2Precal", 1);
    run->AddTask(pspxMapped2Precal);
    R3BPspxPrecal2Cal* pspxPrecal2Cal = new R3BPspxPrecal2Cal("PspxPrecal2Cal", 1);
    run->AddTask(pspxPrecal2Cal);
    R3BPspxCal2Hit* pspxCal2Hit = new R3BPspxCal2Hit("PspxCal2Hit", 1);
    run->AddTask(pspxCal2Hit);

    /* Initialize ------------------------------------------- */
    run->Init();
    rtdb1->print();
    //    FairLogger::GetLogger()->SetLogScreenLevel("warn");
    //    FairLogger::GetLogger()->SetLogScreenLevel("debug");
    FairLogger::GetLogger()->SetLogScreenLevel("info");

    /* Run -------------------------------------------------- */
    run->Run((nev < 0) ? nev : 0, (nev < 0) ? 0 : nev);
    //    rtdb1->saveOutput();

    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << endl << endl;
    cout << "Macro finished succesfully." << endl;
    cout << "Output file is " << outputFileName << endl;
    cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl << endl;

    gApplication->Terminate();
}
