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
// Class Description
// Manager of NetronHP
// 
// 121031 First implementation done by T. Koi (SLAC/PPA)
// P. Arce, June-2014 Conversion neutron_hp to particle_hp
//
#include "G4ParticleHPManager.hh"
#include "G4HadronicException.hh"

G4ThreadLocal G4ParticleHPManager* G4ParticleHPManager::instance = NULL;

G4ParticleHPManager::G4ParticleHPManager()
:RWB(NULL),verboseLevel(1)
{
;
}
G4ParticleHPManager::~G4ParticleHPManager()
{
;
}
void G4ParticleHPManager::OpenReactionWhiteBoard()
{
   if ( RWB != NULL ) {
      G4cout << "Warning: G4ParticleHPReactionWhiteBoard is tried doubly opening" << G4endl;
      RWB = new G4ParticleHPReactionWhiteBoard();
   }
   
   RWB = new G4ParticleHPReactionWhiteBoard();
}
G4ParticleHPReactionWhiteBoard* G4ParticleHPManager::GetReactionWhiteBoard()
{
   if ( RWB == NULL ) {
      G4cout << "Warning: try to access G4ParticleHPReactionWhiteBoard before opening" << G4endl;
      RWB = new G4ParticleHPReactionWhiteBoard();
   }
   return RWB; 
}
#include "zlib.h"
#include <fstream>
void G4ParticleHPManager::GetDataStream( G4String filename , std::istringstream& iss ) 
{
   G4String* data=NULL;
   G4String compfilename(filename);
   compfilename += ".z";
   std::ifstream* in = new std::ifstream ( compfilename , std::ios::binary | std::ios::ate );
   if ( in->good() )
   {
// Use the compressed file 
      G4int file_size = in->tellg();
      in->seekg( 0 , std::ios::beg );
      Bytef* compdata = new Bytef[ file_size ];

      while ( *in ) {
         in->read( (char*)compdata , file_size );
      }

      uLongf complen = (uLongf) ( file_size*4 );
      Bytef* uncompdata = new Bytef[complen];

      while ( Z_OK != uncompress ( uncompdata , &complen , compdata , file_size ) ) {
         //G4cout << "Too small, retry 2 times bigger size." << G4endl;
         delete[] uncompdata;
         complen *= 2;
         uncompdata = new Bytef[complen];
      }
      delete [] compdata;
      //                                 Now "complen" has uncomplessed size
      data = new G4String ( (char*)uncompdata , (G4long)complen );
      delete [] uncompdata;
   }
   else {
// Use regular text file 
      std::ifstream thefData( filename , std::ios::in | std::ios::ate );
      if ( thefData.good() ) {
         G4int file_size = thefData.tellg();
         thefData.seekg( 0 , std::ios::beg );
         char* filedata = new char[ file_size ];
         while ( thefData ) {
            thefData.read( filedata , file_size );
         }
         thefData.close();
         data = new G4String ( filedata , file_size );
         delete [] filedata;
      } else {
// found no data file
//                 set error bit to the stream   
         iss.setstate( std::ios::badbit ); 
      }
   }
   if (data != NULL) iss.str(*data);
   //G4cout << iss.rdbuf()->in_avail() << G4endl;
   in->close(); delete in;
   delete data;
}

void G4ParticleHPManager::SetVerboseLevel( G4int newValue )
{
   G4cout << "You are setting a new verbose level for neutron HP package." << G4endl;
   G4cout << "the new value will be used in whole of the neutron HP package, i.e., models and cross sections for Capture, Elastic, Fission and Inelastic interaction." << G4endl;
   verboseLevel = newValue;
}
