/*
###################################
#
# swinole.c
# Programmed by  nyasu <nyasu@osk.3web.ne.jp>
# (C) 2003-2005  Nishikawa,Yasuhiro 
#
# More information at http://vruby.sourceforge.net/index.html
#
# Note: 
#    The features implemented by this file are very unstable.
#
###################################
*/
#ifdef UNICODE
#  define _UNICODE
#endif

#define WITH_FAKEFUNCTION_WIN32OLE

#define ACCEPTABLE_WIN32OLE "1.1.4"

static int WM_ATLGETCONTROL=0;
#define ATLWINCLASS "AtlAxWin"

#ifdef _MSC_VER
#  include <tchar.h>  /* for VC++ */
#endif

#include "ruby.h"
#include "swin.h"

#if __CYGWIN__ | __MINGW32__
#include <ole2.h>   /* my cygwin needs this */
#endif

#include <olectl.h>


static VALUE cWIN32OLE;
static VALUE (*win32ole_create_win32ole_object )() = 0;



#ifdef WITH_FAKEFUNCTION_WIN32OLE

#define ACCEPTABLE_WIN32OLE_VERSION_VAR "_acceptable_win32ole_ver"

struct oledata {
   	IDispatch *pDispatch;
};

void ole_free(struct oledata* pole){
	pole->pDispatch->lpVtbl->Release(pole->pDispatch);
	CoFreeUnusedLibraries();
}

static VALUE
swin_fake_create_win32ole(VALUE klass,IDispatch *pDisp,int argc,VALUE* argv){
	struct oledata* pole;
	VALUE robj;
	robj = Data_Make_Struct(klass,struct oledata,0,ole_free,pole);
	pole->pDispatch=pDisp;
	return robj;
}
#endif /* WITH_FAKEFUNCTION_WIN32OLE */

static VALUE
swin_get_win32ole_function(){
	VALUE funct_ptr_val;
	VALUE win32ole_version;
	VALUE acceptable_win32ole;

	if(win32ole_create_win32ole_object!=0) return Qtrue;

	cWIN32OLE = rb_const_get(rb_mKernel,rb_intern("WIN32OLE"));
	if(cWIN32OLE==Qnil){
		rb_raise(rb_eRuntimeError,"win32ole is not loaded.");
		return Qfalse;
	}

#ifndef WITH_FAKEFUNCTION_WIN32OLE
	funct_ptr_val = rb_iv_get(cWIN32OLE,"fp CREATE_WIN32OLE_OBJECT");
	if(funct_ptr_val==Qnil){
		rb_raise(rb_eNotImpError,
			"You can't use this feature. This feature is secret feature and "
			"enabled with special version of win32ole."
			);
		return Qfalse;
	}
	win32ole_create_win32ole_object = (VALUE (*)()) NUM2UINT(funct_ptr_val);
#else
	win32ole_version = rb_const_get(cWIN32OLE,rb_intern("VERSION"));
	acceptable_win32ole = rb_iv_get(mSwin,ACCEPTABLE_WIN32OLE_VERSION_VAR);

	if(strcmp(StringValuePtr(acceptable_win32ole),
	          StringValuePtr(win32ole_version))
	   < 0){
      
	   rb_raise(rb_eNotImpError,
	            "This win32ole(%s) is not acceptable(version mismatch).",
	            StringValuePtr(win32ole_version) );
	   return Qfalse;
	}
	win32ole_create_win32ole_object = swin_fake_create_win32ole;
#endif
	return Qtrue;
}

static VALUE
swin_create_win32ole_object(VALUE mod, VALUE iunk){
    IUnknown *pUnknown;
	HRESULT hr;
	IDispatch *pDispatch;

	if(swin_get_win32ole_function()!=Qtrue){
		return Qfalse;
	}

	pUnknown = (IUnknown*) NUM2UINT(iunk);

	if(pUnknown==0) return Qnil;;
	hr = pUnknown->lpVtbl->QueryInterface(pUnknown, &IID_IDispatch,
                                             (void **)&pDispatch);
    pUnknown->lpVtbl->Release(pUnknown);
	
	if(hr){
		rb_raise(rb_eRuntimeError,"failed to get IDispatch");
		return Qfalse;
	}

	return win32ole_create_win32ole_object(cWIN32OLE, pDispatch, 0,0);

}

static VALUE
swin_win32ole_loadpicture(VALUE obj, VALUE str){
	char *buffer;
	VALUE fstring;
	HGLOBAL gmem;
	LPSTREAM pstrm;
	IDispatch* disp;
	long len;
	VALUE robj;

	fstring = StringValue(str);
	len = RSTRING_LEN(fstring);
	
	gmem = GlobalAlloc(GPTR,len);
	if(gmem==NULL){
		rb_raise(rb_eRuntimeError,"can't allocate Global memory");
		return Qfalse;
	}
	if(CreateStreamOnHGlobal(gmem,TRUE,&pstrm)==S_OK){
		buffer = GlobalLock(gmem);
		memcpy(buffer,RSTRING_PTR(fstring),len);
		GlobalUnlock(gmem);
		if(OleLoadPicture(pstrm,len,TRUE,&IID_IPicture,(void**)&disp)==S_OK){
			robj=swin_create_win32ole_object(obj, UINT2NUM((long)disp));
			
		} else {
			rb_raise(rb_eRuntimeError,"failed OleLoadPicture()");
			return Qfalse;
		}
		pstrm->lpVtbl->Release(pstrm);  /* auto-release of gmem */
	} else {
		rb_raise(rb_eRuntimeError,"failed to create stream");
		return Qfalse;
	};
	return robj;
}


static VALUE
swin_win32ole_from_atlwin(VALUE obj){
	struct Swin *sw;
	VALUE cname;
	IUnknown* p;

	if(!WM_ATLGETCONTROL){
		WM_ATLGETCONTROL = RegisterWindowMessage(TEXT("WM_ATLGETCONTROL"));
	}
	Data_Get_Struct(obj, struct Swin, sw);
	SWIN_WINEXISTCHECK(sw);

    cname = SWIN_OUTAPI_STR_NEW2(sw->classname);
	if(strncmp(
      SWIN_ASCII_STR_PTR(cname),
      ATLWINCLASS,sizeof(ATLWINCLASS)))  { /* oops */
		rb_raise(rb_eRuntimeError,
			"This window is not %s (this is %s)",ATLWINCLASS,sw->classname);
		return Qfalse;
	}

	p = (IUnknown*)SendMessage(sw->hWnd,WM_ATLGETCONTROL,0,0);
	if(p==NULL){
		rb_raise(rb_eRuntimeError,"AtlGetControl failed");
		return Qfalse;
	}
	return swin_create_win32ole_object(obj,UINT2NUM((unsigned)p));
}

static VALUE
swin_set_acceptable_win32ole_version(VALUE obj, VALUE ver){
	VALUE robj;
	ID iid = rb_intern(ACCEPTABLE_WIN32OLE_VERSION_VAR);
	robj = rb_ivar_get(mSwin,iid);
	rb_ivar_set(mSwin,iid,StringValue(ver));
	return robj;
}

void Init_swinole(){
	rb_ivar_set(mSwin,
	    rb_intern(ACCEPTABLE_WIN32OLE_VERSION_VAR),rb_str_new2(ACCEPTABLE_WIN32OLE));

	rb_define_singleton_method(mSwinApp,"acceptable_win32ole",
	       swin_set_acceptable_win32ole_version,1);

	rb_define_method(cSwin,"get_oleinterface",swin_win32ole_from_atlwin,0);
	rb_define_singleton_method(mSwinApp,"loadpicture",
	       swin_win32ole_loadpicture,1);

/* obsolete */
	rb_define_singleton_method(mSwinApp,"iunknown_to_win32ole",
	       swin_create_win32ole_object,1);
}
