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

//************************************************************************
int read_svg_info(TCHAR *fname)
{
   sprintf(fpath, _T("%s%s"), base_path, fname) ;
   int fsize = read_into_dbuffer(fpath) ;
   if (fsize < 0) {
      printf("unreadable SVG: %d\n", fsize) ;
      return 1 ;
   } 
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
   printf("filesize: %6d, dimens: %4u x %5u, %s\n", fsize, width, height, fpath);
   
// filesize: 365046, dimens: 4257 x  7265, D:\SourceCode\Git\svg_hacker\files\E08_SQ01.svg
// filesize: 354188, dimens: 6423 x  5589, D:\SourceCode\Git\svg_hacker\files\E08_SQ01_S2.svg
// filesize: 366388, dimens: 7138 x  4782, D:\SourceCode\Git\svg_hacker\files\E08_SQ01_S3.svg
   //  now, we can search through the buffer for whatever strings we are interested in.
   //  strcasestr() may be of use here
   
   return 0;
}

