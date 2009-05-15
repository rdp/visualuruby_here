/*
###################################
#
# swin.h
# Programmed by nyasu <nyasu@osk.3web.ne.jp>
# Copyright 1999-2005  Nishikawa,Yasuhiro
#
# More information at http://vruby.sourceforge.net/index.html
#
###################################
*/

#ifndef __SWIN_H__
#define __SWIN_H__

#define SWIN_VERSION "090207"

#include "ruby.h"

#ifdef UNICODE
#  define SWIN_FOR_RUBY19_M17N
#  include "ruby/encoding.h"
#endif

#include <windows.h>
#include <commctrl.h>
#include <stdlib.h>
#include "larray.h"

/* Thanks, Nakada-san */
#define NOTHING /* nothing */ 
#define MYMALLOC(name,size,failval)  do{ \
	name = malloc(size); \
	if(!name){ \
		rb_raise(rb_eRuntimeError,"failed to allocate memory"); \
		return failval; \
	} \
} while(0)

#define SWIN_CNAME "simplewin"
#define SWIN_W_CNAME L"simplewin"    /* must be the same except encoding */

#ifdef UNICODE
#define SWIN_T_CNAME SWIN_W_CNAME
#else
#define SWIN_T_CNAME SWIN_CNAME
#endif


#define SWIN_MSGHANDLERNAME "msghandler"

#define SWIN_TEMPWINDOW_CREATE_LPARAM   ((void*)1)


/*   Classes and Modules  */
extern VALUE mSwin;
extern VALUE cSwin;
extern VALUE cSwinMsg;
extern VALUE cSwinFactory;
extern VALUE eSwinResourceNotExistError;
extern VALUE eSwinWindowNotExistError;
extern VALUE eSwinFontNotExistError;
extern VALUE eSwinCursorNotExistError;
extern VALUE eSwinImagelistNotExistError;
extern VALUE mSwinApp;


extern VALUE cSwinBitmap;
extern VALUE cSwinMenu;
extern VALUE mSwinComDlg;
extern VALUE cSwinCanvas;

extern VALUE cSwinFont;
extern VALUE cSwinCursor;
extern VALUE cSwinIcon;

extern VALUE cSwinImagelist;

extern VALUE cSwinDialog;

extern VALUE cSwinPrinter;

/*   Structures   */

struct SwinMsg{
	HWND hWnd;
	UINT msg;
	WPARAM wParam;
	LPARAM lParam;

	UINT retval;
};

struct SwinFactory{
	unsigned long hInstance;
};

struct SwinFilter{
	unsigned msg;
	struct SwinFilter* next;
};



#define SWIN_OBJ_PEN     1
#define SWIN_OBJ_BRUSH   2
#define SWIN_OBJ_FONT    4


struct SwinGDIStruct{
	HDC hdc;
	HPEN hpenOrg; 
	HBRUSH hbrushOrg;
	HFONT hfontOrg;
	int selectedObjFlag;  /* for Pen,Brush,... */
};


struct Swin{
	VALUE me;   /* thisself */
	unsigned long hInst;
	HWND hParent;
	HWND hWnd;
	HANDLE hEtc;
	TCHAR* classname;  
	DWORD dwStyle;
	DWORD dwExStyle;
	TCHAR* caption;
	int x,y,h,w;
	BOOL enabled;
	int painting;
	PAINTSTRUCT ps;
	struct SwinGDIStruct gs;
#ifdef NT   /* Thanks, Suketa-san and Shirakura-san */
	LRESULT (CALLBACK* OrigWndProc) (HWND, UINT, WPARAM, LPARAM);
#else  
	LRESULT CALLBACK (*OrigWndProc)(HWND, UINT, WPARAM, LPARAM);
#endif
	struct SwinFilter* filter;
    DWORD nthread_id;
};

struct SwinDialog{
	struct Swin sw;
	DLGTEMPLATE* template;
	BOOL modal;
};

struct SwinBitmap{
	BITMAPINFO *info;
	unsigned    infosize;
	unsigned char* bmp;
	unsigned    bmpsize;  /* length of bmp */
};

struct SwinMenu{
	BOOL  f_delete;
	HMENU hmenu;
	HWND  hwnd;   /* not NULL while window menu */
};

struct SwinCanvas{
	HBITMAP hbmp;
	HBITMAP horigbmp;
	struct SwinGDIStruct gs;
};

#define SWINFONT_ITALIC 1
#define SWINFONT_ULINE  2
#define SWINFONT_STRIKE 4

struct SwinFont{
	HFONT hfont;
	TCHAR* fontname;
	short pitchfamily;
	unsigned char style;
	int weight;
	int width;
	int height;
	int escape;
	int orient;
	unsigned char charset;
};

struct SwinCursor{
	HCURSOR  hcursor;
	BOOL     sysflag;
	BOOL	 shared;
	unsigned hotspot;
};

struct SwinImagelist{
	HIMAGELIST himagelist;
	UINT flags;
};

struct SwinIcon{
	HICON hicon;
	BOOL shared;
};

struct SwinPrinter{
	BOOL openeddoc;
	BOOL openedpage;
	TCHAR* printername;
	DOCINFO docinfo;
	struct SwinGDIStruct gs;
};

struct KeyAccItem{
	HWND hwnd;
	BOOL original;
	HACCEL haccel;
};


/*   Utils   */

#define SET_BIT(orig,ib)	orig=(orig | ib)
#define RESET_BIT(orig,ib)	orig=(orig & ~ib)

#define SWIN_WINEXISTCHECK(sw) \
  if((sw)->hWnd==NULL){ \
    rb_raise(eSwinWindowNotExistError,"Window Not Exist");   \
    return Qfalse;    \
  } 
#define SWIN_PAINTINGCHECK(sw) \
  if(!(sw)->painting){   \
    rb_raise(rb_eStandardError,"Not in WM_PAINT event handling");  \
    return Qfalse;   \
    } 


#define SWIN_FONTALIVECHECK(sf) \
  if((sf)->hfont==0){   \
    rb_raise(rb_eStandardError,"Font already deleted");  \
    return Qfalse;   \
    } 

#define SWIN_CURSORALIVECHECK(sc) \
  if((sc)->hcursor==0){   \
    rb_raise(rb_eStandardError,"Cursor already deleted");  \
    return Qfalse;   \
    } 

#define SWIN_IMAGELISTALIVECHECK(si) \
  if((si)->himagelist==0){   \
    rb_raise(rb_eStandardError,"Imagelist already deleted");  \
    return Qfalse;   \
    } 

#define SWIN_PRINTERCHECK(sp) \
  if(!(sp)->openedpage){   \
    rb_raise(rb_eStandardError,"Page not opened.");  \
    return Qfalse;   \
    } 

#ifdef UNICODE
#define swin_strncmp    wcsncmp
#else
#define swin_strncmp    strncmp
#endif

#define SWIN_ORIGINALWNDCLASS(classname) \
  (swin_strncmp(classname,SWIN_T_CNAME,sizeof(SWIN_CNAME)-1) == 0) 


/*   Prototypes   */

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
extern VALUE swin_avoid_new(int,VALUE*,VALUE);
extern VALUE swinaa_application(VALUE);

void increment_opened_window();
void decrement_opened_window();

#define MYMAXPATH 512

#ifdef SWIN_FOR_RUBY19_M17N
#define path_conv_to_win path_conv_to_win_wchar
#else
#define path_conv_to_win path_conv_to_win_ansi
#endif

void path_conv_to_win(TCHAR*s,TCHAR*d);

BOOL message_handling(struct Swin* ,struct SwinMsg*,VALUE);

VALUE swin_t_messageBox(int argc,VALUE *argv,HWND parent);

void swingdi_init(struct SwinGDIStruct*);
void swingdi_release(struct SwinGDIStruct*);

extern int swin_handling_ctrlc;

extern char* onebytezero;


/*  swin hookwindow list */
extern LArray* swlist;
#define Register_sw(hWnd,sw)	LArray_set(swlist,(int)hWnd,(long)sw)
#define Unregister_sw(hWnd)		LArray_delete(swlist,(int)hWnd,FALSE)
#define Get_sw(hWnd)			((struct Swin*)LArray_get(swlist,(int)hWnd))

extern HWND current_dialog;
extern HINSTANCE hInstance;
extern BOOL threadcritical;

void Init_swinWindow();
void Init_swinGDI();
void Init_swinBitmap();
void Init_swinMenu();
void Init_CDlg();
void Init_swinCanvas();
void Init_swinRsc();
void Init_swinDialog();
void Init_swinPrinter();
void Init_swinolednd();


#ifdef SWIN_FOR_RUBY19_M17N

extern VALUE apiEncoding, outputEncoding;
extern ID id_encode;

// madorokkosii

// change encoding of string obj 
#define SWIN_ENC_STR(value,enc) rb_funcall(value,id_encode,1,enc)

// change encoding for win32api input data
#define SWIN_API_STR(str) rb_str_buf_cat(SWIN_ENC_STR(str, apiEncoding), onebytezero, 2)
// change encoding for win32api output data
#define SWIN_OUT_STR(str) SWIN_ENC_STR(str, outputEncoding)
// new string for win32api
#define SWIN_API_STR_NEW(str,len) \
             rb_enc_str_new((char*)str, len*sizeof(TCHAR), \
                            rb_to_encoding(apiEncoding))
// new string from win32api output
#define SWIN_OUTAPI_STR_NEW(str,len) \
    SWIN_ENC_STR(SWIN_API_STR_NEW(str,len),outputEncoding)
#define SWIN_OUTAPI_STR_NEW2(str) \
    SWIN_ENC_STR(SWIN_API_STR_NEW(str,lstrlen(str)),outputEncoding)
// get usascii string pointer
#define SWIN_ASCII_STR_PTR(value) \
            RSTRING_PTR(SWIN_ENC_STR(value,rb_enc_from_encoding(rb_ascii8bit_encoding())))
// convert const char* to w/a win32api codingsystem
#define SWIN_API_STR_CHAR(buffer) \
          ((TCHAR*)RSTRING_PTR(SWIN_API_STR(rb_str_new2(buffer))))
#else

#define SWIN_API_STR(value) StringValue(value)
#define SWIN_OUT_STR(value) StringValue(value)
#define SWIN_API_STR_NEW(str,len) rb_str_new(str,len)
#define SWIN_OUTAPI_STR_NEW(str,len) SWIN_API_STR_NEW(str,len)
#define SWIN_OUTAPI_STR_NEW2(str) rb_str_new2(str)
#define SWIN_ASCII_STR_PTR(value) StringValuePtr(value)
#define SWIN_API_STR_CHAR(buffer) (buffer)

#endif  // SWIN_FOR_RUBY19_M17N

#define SWIN_API_STR_PTR(value) ((TCHAR*)RSTRING_PTR(SWIN_API_STR(value)))
#define SWIN_API_STR_LEN(value) RSTRING_LEN(SWIN_API_STR(value))


/******************************************
*     for version compatibility
*/

/**  Thanks, Yoshida-san **/
#ifndef UINT2NUM
VALUE rb_uint2inum _((unsigned long));
#define UINT2NUM(v) rb_uint2inum(v)
#endif

/* this undef is necessary while StringValue() in ruby.h is if-clause. */
#undef StringValue

/** Introduced by Tamura-san **/
#ifndef RB_STRING_VALUE
#define RB_STRING_VALUE(s) (TYPE(s) == T_STRING? (s) : (*(volatile VALUE *)&(s) = rb_str_to_str(s)))
#endif
#ifndef StringValue
#define StringValue(s) RB_STRING_VALUE(s)
#endif
#ifndef StringValuePtr
#define StringValuePtr(s) (RSTRING_PTR(RB_STRING_VALUE(s)))
#endif
#ifndef StringValueLen
#define StringValueLen(s) RSTRING_LEN(RB_STRING_VALUE(s))
#endif

/* for native thread in ruby-1.9 */

#define INMESSAGESYM "swin_in_messageloop"
#define OPENEDWINSYM "swin_opened_window_count"

 // th_id, num, proc, args..
DWORD swin_call_threadblocking(DWORD thid, int num, FARPROC, ...);



/*  for very old ruby */
#ifndef RSTRING_LEN
#  define RSTRING_LEN(s) RSTRING(s)->len
#endif
#ifndef RSTRING_PTR
#  define RSTRING_PTR(s) RSTRING(s)->ptr
#endif

#endif  /* __SWIN_H__ */
