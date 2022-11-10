#include "TFile.h"
#include "TTree.h"

void analyze()
{
  std::string filename = "gammaAnalysis_";
  std::string file_ext = ".root";

  
  double x[16] = {-15, -13, -11, -9, -7, -5, -3, -1,
		  1, 3, 5, 7, 9, 11, 13, 15};
  double y[16];
  double y_bkg[16];

  
  for(int i = 0; i<16; i++){

    std::string n = std::to_string(i);

    std::string a_file = filename + n + file_ext;
    const char* this_filename = a_file.c_str();

    TFile* this_file = new TFile(this_filename);

    TTreeReader *myReader = new TTreeReader("gDetector", this_file);
    TTreeReader *myBkgReader = new TTreeReader("gDetector_bkg", this_file);


    y[i] = 0 ;
    y_bkg[i] = 0;
    
    while(myReader->Next()){

      y[i]+=1.;
      

    }

    while(myBkgReader->Next()){

      y_bkg[i]+=1.;
      y[i] += 1.;
	
	
    }

  }

  TGraph *g1 = new TGraph(16, x, y);
  TGraph *g2 = new TGraph(16, x, y);

  auto c1 = new TCanvas("a", "a", 700, 500);
  g1->Draw();
  g2->Draw("same");
  
    
    




}
