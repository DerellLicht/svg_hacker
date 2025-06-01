//**********************************************************************************
//  Copyright (c) 2023 Daniel D. Miller                       
//**********************************************************************************

//************************************************************
struct ffdata {
   uchar          attrib ;
   FILETIME       ft ;
   ULONGLONG      fsize ;
   char           *filename ;
   uchar          dirflag ;
   struct ffdata  *next ;
} ;

