//************************************************************************
//  SVG is actually a vector file format, with image-drawing options.
//  As a first pass, I will search for the following string:
//  style="width:4257px;height:7265px;background:#222222;"
//  I don't know if all svg files have this field, 
//  so more work may be required in the future.
//************************************************************************
// <svg xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" 
// contentStyleType="text/css" 
// height="7265px" preserveAspectRatio="none" 
// style="width:4257px;height:7265px;background:#222222;" 
// version="1.1" viewBox="0 0 4257 7265" 
// width="4257px" zoomAndPan="magnify"><defs/><g><rect fill="#222222" 
//************************************************************************
#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <tchar.h>

#include "common.h"
#include "svg_hacker.h"

static TCHAR fpath[MAX_FILE_LEN+2] ;

#define  DBUFFER_LEN    (1024 * 1024)
static char dbuffer[DBUFFER_LEN] ;
static uint dbuf_data_size = 0 ;

//************************************************************************
//  return file size on success, negative error code on error
//************************************************************************
static int read_into_dbuffer(TCHAR *flpath)
{
   int hdl, rbytes ;
   struct _stat buf;

   hdl = _topen(flpath, O_BINARY | O_RDONLY) ;
   if (hdl < 0) {
      return -errno;
   }
   
   int result = _fstat(hdl, &buf );
   if (result != 0) {
      close(hdl) ;
      return -errno;
   }
   uint filesize = (uint) buf.st_size;
   if (filesize > DBUFFER_LEN) {
      return -ERANGE ;
   }
   
   //  read header data
   rbytes = _read(hdl, dbuffer, filesize) ;
   if (rbytes < 0) {
      close(hdl) ;
      return -errno;
   }
   _close(hdl) ;  //lint !e534
   return (int) filesize;
}  //lint !e818

//****************************************************************************************
#define  MAX_ID_STR_LEN    128

// static char const start_tag[] = "id=\"" ;
static char const start_tag[] = ">E08_SQ" ;
// static char const end_tag = '"' ;
static char const end_tag = '<' ;
static int parse_id_str(char *idtemp, char const * instr)
{
   instr += 1 ;  //  bypass leaders before target string
   int idlen = 0 ;
   while (*instr != end_tag) {
      *idtemp++ = *instr++ ;
      idlen++ ;
   }
   *idtemp = 0 ;  //  NULL-term the output string
   return idlen ;
}

//****************************************************************************************
//  now, we can search through the buffer for whatever strings we are interested in.
//  strcasestr() may be of use here
//  id="elem_E08_SQ01_ChangeRelationships_Dvupalov_Pin_0"
//  >E08_SQ01_TakenQuest</text>
//****************************************************************************************
static int parse_svg_string(void)
{
   static char idtemp[MAX_ID_STR_LEN+1] ;
   char *hd = &dbuffer[0] ;
   // printf("address of dbuffer: %08X\n", (uint) &dbuffer[0]);
   while (LOOP_FOREVER) {
      char *idptr = strstr(hd, start_tag);
      if (idptr == NULL) {
         break ;
      }
      // printf("%08X (%6u), %08X: ", (uint) idptr, (uint) (idptr - &dbuffer[0]), (uint) hd);
      int result = parse_id_str(idtemp, idptr);
      if (result < 0) {
         return result ;
      }
      // printf("%u: ", (uint) result);
      printf("%s\n", idtemp);
      hd = idptr + result + 4 ;
   }
   
   return 0 ;
}

//************************************************************************
int read_svg_info(TCHAR *fname)
{
   sprintf(fpath, _T("%s%s"), base_path, fname) ;
   int result = read_into_dbuffer(fpath) ;
   if (result < 0) {
      printf("unreadable SVG: %d\n", result) ;
      return 1 ;
   } 
   dbuf_data_size = (uint) read_into_dbuffer(fpath) ;
   //  first, search for the "fmt" string
   if (strncmp((char *)  dbuffer,    "<svg", 4) != 0) {
      printf("unknown svg format 1") ;
      return 1 ;
   }

   //  style="width:4257px;height:7265px;background:#222222;"
   char *hd = strstr((char *) dbuffer, "style=\"width:");   
   if (hd == NULL) {
      printf("unknown svg format 2") ;
      return 1 ;
   }
   char *tl = (hd + 13);
   uint width = (uint) atoi(tl) ;
   tl = strstr(hd, ";height:");
   if (tl == NULL) {
      printf("unknown svg format 3") ;
      return 1 ;
   }
   tl += 8 ;
   uint height = (uint) atoi(tl);
   printf("\nfilesize: %6d, dimens: %4u x %5u, %s\n", dbuf_data_size, width, height, fpath); //lint !e705
   
// filesize: 365046, dimens: 4257 x  7265, D:\SourceCode\Git\svg_hacker\files\E08_SQ01.svg
// filesize: 354188, dimens: 6423 x  5589, D:\SourceCode\Git\svg_hacker\files\E08_SQ01_S2.svg
// filesize: 366388, dimens: 7138 x  4782, D:\SourceCode\Git\svg_hacker\files\E08_SQ01_S3.svg
   result = parse_svg_string();
   if (result < 0) {
      return result ;
   }
   
   return 0;
}

