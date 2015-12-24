/*===================================================================
  This holds the software version number string for boot firmware.
  
  NOTE:  this file is parsed by an application creating the MOCF.
  This application will find the first quoted string and use that
  as the version number string. 
  
  A) A product ID field   (2 character alpha)
  B) A software ID field (6 digit numeric)
  C) A build site location field (1 character alpha)
  D) A use case field (1 character Alpha)
  E) Component Field (1 Character Alpha)
  F) Release Candidate Number (2 digit numeric)  Temporary and not part of official number  

	#define SOFTWARE_VERSION		"BF000003AAC"
  
=====================================================================
   $Source: db_boot/SoftwareVersionBoot.h $
   $Date: 2011/04/19 09:50:11EDT $
   $Revision: 1.14 $


===================================================================*/
#ifndef _SoftwareVersionBoot_h_
#define _SoftwareVersionBoot_h_


#define SOFTWARE_VERSION	"BF000023AAD"


#endif // endof #ifndef _SoftwareVersionBoot_h_
