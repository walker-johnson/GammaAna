//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
/// \file SteppingAction.cc
/// \brief Implementation of the SteppingAction class
//
// 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "SteppingAction.hh"
#include "Run.hh"
#include "TrackingAction.hh"
#include "HistoManager.hh"

#include "G4RunManager.hh"
                           
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::SteppingAction(EventAction* evt, TrackingAction* TrAct)
  : G4UserSteppingAction(),fEventAction(evt),fTrackingAction(TrAct)
{
  //get the dedector
  fDetector = static_cast<const DetectorConstruction*> (G4RunManager::GetRunManager()->GetUserDetectorConstruction());

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::~SteppingAction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SteppingAction::UserSteppingAction(const G4Step* step)
{

  //Get process information
  const G4StepPoint* endPoint = step->GetPostStepPoint();
  const G4VProcess* process = endPoint->GetProcessDefinedStep();
  G4String processName = process->GetProcessName();
  
  // count processes
  Run* run = static_cast<Run*>(G4RunManager::GetRunManager()->GetNonConstCurrentRun());
  run->CountProcesses(process);

  // Get step information
  const G4StepPoint* pre = step->GetPreStepPoint();
  const G4StepPoint* post = step->GetPostStepPoint();
  const G4VPhysicalVolume* prePhysical = pre->GetPhysicalVolume();
  const G4VPhysicalVolume* postPhysical = post->GetPhysicalVolume();
  G4double x = post->GetPosition().x(), y = post->GetPosition().y(), z = post->GetPosition().z(); 	

  // Get track information
  G4Track* track = step->GetTrack();
  G4double ekin  = endPoint->GetKineticEnergy();
  G4double trackl = step->GetTrack()->GetTrackLength();
  G4double time   = step->GetTrack()->GetLocalTime();

  // Sanity checks
  if(prePhysical == 0 || postPhysical == 0) return;  // The track does not exist  
  if(prePhysical->GetCopyNo() == -1 && postPhysical->GetCopyNo() == -1) return; // Both steps are in the World
  
  // Get logical volume
  const G4LogicalVolume* preLogical = pre->GetTouchableHandle()->GetVolume()->GetLogicalVolume();
  const G4LogicalVolume* postLogical = post->GetTouchableHandle()->GetVolume()->GetLogicalVolume();
  	
  // Get particle name
  G4String particleName = step->GetTrack()->GetDefinition()->GetParticleName();

  //Neutron passing through boundary
  if(particleName == "neutron" && post->GetStepStatus() == fGeomBoundary) {

       
    //neutrons leaving the tank
    if(preLogical == fDetector->tankL &&
       postLogical == fDetector->roomL){
      G4AnalysisManager::Instance()->FillNtupleDColumn(0,0,x/1000); //ID, column,value
      G4AnalysisManager::Instance()->FillNtupleDColumn(0,1,y/1000); //ID, column,value
      G4AnalysisManager::Instance()->FillNtupleDColumn(0,2,z/1000); //ID, column,value
      G4AnalysisManager::Instance()->FillNtupleDColumn(0,3,ekin); 
      G4AnalysisManager::Instance()->AddNtupleRow(0);
    }
    //neurons leaving concrete
    if(preLogical == fDetector->slabL && postLogical == fDetector->roomL){
      G4AnalysisManager::Instance()->FillNtupleDColumn(2,0,x/1000); //ID, column,value
      G4AnalysisManager::Instance()->FillNtupleDColumn(2,1,y/1000); //ID, column,value
      G4AnalysisManager::Instance()->FillNtupleDColumn(2,2,z/1000); //ID, column,value
      G4AnalysisManager::Instance()->FillNtupleDColumn(2,3,ekin); 
      G4AnalysisManager::Instance()->AddNtupleRow(2);
    }
    if(preLogical == fDetector->roomL && postLogical == fDetector->workL){
      G4AnalysisManager::Instance()->FillH1(6,ekin);
    }
    if(preLogical == fDetector->roomL && postLogical == fDetector->door1L){
      G4AnalysisManager::Instance()->FillH1(7,ekin);
    }
    if(preLogical == fDetector->roomL && postLogical == fDetector->door2L){
      G4AnalysisManager::Instance()->FillH1(8,ekin);
    }
    if(preLogical == fDetector->roomL && postLogical == fDetector->ceilingL){
      G4AnalysisManager::Instance()->FillH1(9,ekin);
    }
    if(preLogical == fDetector->roomL && postLogical == fDetector->beamsideL){
      G4AnalysisManager::Instance()->FillH1(11,ekin);
    }
    if(preLogical == fDetector->roomL && postLogical == fDetector->backL){
      G4AnalysisManager::Instance()->FillH1(13,ekin);
    }
    if(preLogical == fDetector->roomL && postLogical == fDetector->topL){
      G4AnalysisManager::Instance()->FillH1(15,ekin);
    }
  }

  
  //Gamma passing through boundary
  if(particleName == "gamma" && post->GetStepStatus() == fGeomBoundary) {

    //gamma entering gamma counter from gammaShield
    if(preLogical == fDetector->gammaShieldL && postLogical == fDetector->gammaDetL){
      G4AnalysisManager::Instance()->FillNtupleDColumn(5,0,x/1000); //ID, column,value
      G4AnalysisManager::Instance()->FillNtupleDColumn(5,1,y/1000); //ID, column,value
      G4AnalysisManager::Instance()->FillNtupleDColumn(5,2,z/1000); //ID, column,value
      G4AnalysisManager::Instance()->FillNtupleDColumn(5,3,ekin); 
      G4AnalysisManager::Instance()->AddNtupleRow(1);
    }
    
    //gamma entering gamma counter from lead
    if(preLogical == fDetector->gammaDetBoxL && postLogical == fDetector->gammaDetL){
       G4AnalysisManager::Instance()->FillNtupleDColumn(4,0,x/1000); //ID, column,value
       G4AnalysisManager::Instance()->FillNtupleDColumn(4,1,y/1000); //ID, column,value
       G4AnalysisManager::Instance()->FillNtupleDColumn(4,2,z/1000); //ID, column,value
       G4AnalysisManager::Instance()->FillNtupleDColumn(4,3,ekin); 
       G4AnalysisManager::Instance()->AddNtupleRow(5);
    }
    
    //gamma leaving the tank
    if(preLogical == fDetector->tankL &&
       postLogical == fDetector->roomL){
      G4AnalysisManager::Instance()->FillH1(1,ekin);
      G4AnalysisManager::Instance()->FillNtupleDColumn(1,0,x/1000); //ID, column,value
      G4AnalysisManager::Instance()->FillNtupleDColumn(1,1,y/1000); //ID, column,value
      G4AnalysisManager::Instance()->FillNtupleDColumn(1,2,z/1000); //ID, column,value
      G4AnalysisManager::Instance()->FillNtupleDColumn(1,3,ekin); 
      G4AnalysisManager::Instance()->AddNtupleRow(1);
    }
    //gammas leaving concrete
    if(preLogical == fDetector->slabL && postLogical == fDetector->roomL){
      G4AnalysisManager::Instance()->FillNtupleDColumn(3,0,x/1000); //ID, column,value
      G4AnalysisManager::Instance()->FillNtupleDColumn(3,1,y/1000); //ID, column,value
      G4AnalysisManager::Instance()->FillNtupleDColumn(3,2,z/1000); //ID, column,value
      G4AnalysisManager::Instance()->FillNtupleDColumn(3,3,ekin); 
      G4AnalysisManager::Instance()->AddNtupleRow(3);
    }
    if(preLogical == fDetector->roomL && postLogical == fDetector->workL){
      G4AnalysisManager::Instance()->FillH1(2,ekin);
    }
    if(preLogical == fDetector->roomL && postLogical == fDetector->door1L){
      G4AnalysisManager::Instance()->FillH1(3,ekin);
    }
    if(preLogical == fDetector->roomL && postLogical == fDetector->door2L){
      G4AnalysisManager::Instance()->FillH1(4,ekin);
    }
    if(preLogical == fDetector->roomL && postLogical == fDetector->ceilingL){
      G4AnalysisManager::Instance()->FillH1(5,ekin);
    }
    if(preLogical == fDetector->roomL && postLogical == fDetector->beamsideL){
      G4AnalysisManager::Instance()->FillH1(10,ekin);
    }
    if(preLogical == fDetector->roomL && postLogical == fDetector->backL){
      G4AnalysisManager::Instance()->FillH1(12,ekin);
    }
    if(preLogical == fDetector->roomL && postLogical == fDetector->topL){
      G4AnalysisManager::Instance()->FillH1(14,ekin);
    }
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


