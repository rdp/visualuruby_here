/*
###################################
#
# swinprn.c
# Programmed by nyasu <nyasu@osk.3web.ne.jp>
# Copyright 1999-2005  Nishikawa,Yasuhiro
#
# More information at http://vruby.sourceforge.net/index.html
#
###################################
*/

#include "swin.h"
#include "swingdi.h"

VALUE cSwinPrinter;
VALUE mSwinEnumPrinters;

void
shutdown_printer(struct SwinPrinter* sp){
	free(sp->printername);
	if(sp->openeddoc==TRUE){
		EndDoc(sp->gs.hdc);
    }
	if(sp->gs.hdc){
		DeleteDC(sp->gs.hdc);
		swingdi_release(&sp->gs);
	}
	free(sp);
}

static VALUE
swinprinters_enumprinter(VALUE obj){
	PRINTER_INFO_5 *pinfo;
	DWORD needed,returned;
	unsigned size;
	int i;

	size = sizeof(PRINTER_INFO_5)*10;

	MYMALLOC(pinfo,size,Qfalse);

	if(! EnumPrinters(PRINTER_ENUM_LOCAL, NULL, 5, 
			(LPBYTE)pinfo, size, &needed, &returned) ){
		size = needed;
		pinfo = realloc(pinfo,size);
		if(pinfo==NULL){
			rb_raise(rb_eRuntimeError,"failed to allocate memory");
			return Qfalse;
		}
		if(! EnumPrinters(PRINTER_ENUM_LOCAL, NULL,5,
			(LPBYTE)pinfo,size, &needed, &returned)){
				rb_raise(rb_eRuntimeError,"failed to enum printers");
				return Qfalse;
		}
	}

	for(i=0;i<returned;i++){
		rb_yield(SWIN_OUTAPI_STR_NEW2(pinfo[i].pPrinterName));
	}
	free(pinfo);
	return INT2NUM(returned);
}

static VALUE
swinprinters_defaultprintername(VALUE obj){
	PRINTER_INFO_5 *pinfo;
	DWORD needed,returned;
	unsigned size;

	size = sizeof(PRINTER_INFO_5)*4;   
	/* ??? required 61bytes instead of 20=sizeof(PRINTER_INFO_5) */
	MYMALLOC(pinfo,size,Qfalse);

	if(! EnumPrinters(PRINTER_ENUM_DEFAULT, NULL, 5, 
			(LPBYTE)pinfo, size, &needed, &returned) ){
		size = needed;
		pinfo = realloc(pinfo,size);
		if(pinfo==NULL){
			rb_raise(rb_eRuntimeError,"failed to allocate memory");
			return Qfalse;
		}
		if(! EnumPrinters(PRINTER_ENUM_DEFAULT, NULL,5,
			(LPBYTE)pinfo,size, &needed, &returned)){
				rb_raise(rb_eRuntimeError,"failed to enum printers");
				return Qfalse;
		}
	}

	return SWIN_OUTAPI_STR_NEW2(pinfo[0].pPrinterName);
}

static VALUE
swinfac_newprinter(int argc, VALUE* argv, VALUE obj){
	VALUE robj;
	struct SwinPrinter* sp;
	DEVMODE* devmode;
  static TCHAR* defaultdocname;
	VALUE printername,devmodestr;
  
	defaultdocname=TEXT("printer document");

	if(argc<1){
		rb_raise(rb_eArgError,"printer name must be specified");
		return Qfalse;
	}

	printername = SWIN_API_STR(argv[0]);

	devmode = NULL;
	if(argc>1){
		if(TYPE(argv[1])==T_STRING){
			if(RSTRING_LEN(argv[1])==sizeof(DEVMODE)){
				devmode = (DEVMODE*)StringValuePtr(argv[1]);
			} else {
				rb_raise(rb_eArgError,"Illegal devmode string.");
				return Qfalse;
			}
		}
	}

	robj=Data_Make_Struct(cSwinPrinter,struct SwinPrinter,0,shutdown_printer,sp);
	memset(sp,0,sizeof(struct SwinPrinter));
	memset(&sp->docinfo,0,sizeof(DOCINFO));
	sp->docinfo.cbSize = sizeof(DOCINFO);
	sp->docinfo.lpszDocName = defaultdocname;

	MYMALLOC(sp->printername,RSTRING_LEN(printername)+1,Qfalse);
	memcpy(sp->printername,
		RSTRING_PTR(printername),RSTRING_LEN(printername));

	sp->openeddoc = FALSE;
	sp->openedpage=FALSE;
	sp->gs.hdc = CreateDC(NULL,sp->printername,NULL,devmode);
	sp->gs.selectedObjFlag=0;
	if(sp->gs.hdc==NULL){
		return Qfalse;
	}

	return robj;
}

static VALUE
swinprn_pname(VALUE obj){
	struct SwinPrinter * sp;
	Data_Get_Struct(obj, struct SwinPrinter, sp);
	return SWIN_OUTAPI_STR_NEW2(sp->printername);
}

static VALUE
swinprn_startdoc(VALUE obj){
	struct SwinPrinter * sp;
	Data_Get_Struct(obj, struct SwinPrinter, sp);
	sp->openeddoc = TRUE;
	return UINT2NUM(StartDoc(sp->gs.hdc,&sp->docinfo));
}

static VALUE
swinprn_startpage(VALUE obj){
	struct SwinPrinter * sp;
	Data_Get_Struct(obj, struct SwinPrinter, sp);
	
	if(sp->openeddoc==FALSE){
		rb_raise(rb_eRuntimeError,"Document not started");
		return Qfalse;
	}
	sp->openedpage=TRUE;

	if(rb_iterator_p()){
		StartPage(sp->gs.hdc);
		rb_yield(obj);
		EndPage(sp->gs.hdc);
		return obj;
	}
	sp->openedpage=FALSE;
	return UINT2NUM(StartPage(sp->gs.hdc));
}

static VALUE
swinprn_endpage(VALUE obj){
	struct SwinPrinter * sp;
	Data_Get_Struct(obj, struct SwinPrinter, sp);
	return UINT2NUM(EndPage(sp->gs.hdc));
}

static VALUE
swinprn_enddoc(VALUE obj){
	struct SwinPrinter * sp;
	Data_Get_Struct(obj, struct SwinPrinter, sp);
	sp->openeddoc = FALSE;
	return UINT2NUM(EndDoc(sp->gs.hdc));
}

static VALUE
swinprn_abortdoc(VALUE obj){
	struct SwinPrinter * sp;
	Data_Get_Struct(obj, struct SwinPrinter, sp);
	return UINT2NUM(AbortDoc(sp->gs.hdc));
}

static VALUE
swinprinter_props(VALUE obj,VALUE pwin,VALUE pname){
	struct Swin* sw;
	HWND hwnd;
	HANDLE phandle;

	hwnd = NULL;
	if(pwin!=Qnil){
		if(rb_obj_is_kind_of(pwin,cSwin)){
			Data_Get_Struct(pwin, struct Swin, sw);
			hwnd = sw->hWnd;
		}
	}

	OpenPrinter(SWIN_API_STR_PTR(pname),&phandle,NULL);
	PrinterProperties(hwnd,phandle);
	ClosePrinter(phandle);
	return obj;
}


#define SwinPrinterGDIFuncInit \
	struct SwinPrinter *sw; \
	Data_Get_Struct(obj,struct SwinPrinter,sw); \
	SWIN_PRINTERCHECK(sw)

#define SwinPrinterGDINoChkFuncInit \
	struct SwinPrinter *sw; \
	Data_Get_Struct(obj,struct SwinPrinter,sw); \

DEFINE_GDI_FUNCTIONS_MACRO(swinprn_,SwinPrinterGDIFuncInit)
DEFINE_GDI_DEVICECAPS_MACRO(swinprn_,SwinPrinterGDINoChkFuncInit)


void Init_swinPrinter(){
	cSwinPrinter = rb_define_class_under(mSwin,"PrintDoc",rb_cObject);
	mSwinEnumPrinters = rb_define_class_under(mSwinApp,"Printers",rb_cObject);

	rb_define_singleton_method(mSwinEnumPrinters,"eachprinters",
			swinprinters_enumprinter,0);
	rb_define_singleton_method(mSwinEnumPrinters,"each",
			swinprinters_enumprinter,0);
	rb_define_singleton_method(mSwinEnumPrinters,"default",
			swinprinters_defaultprintername,0);
	rb_define_singleton_method(mSwinEnumPrinters,"propdialog",
			swinprinter_props,2);
	
	rb_define_method(cSwinFactory,"openprinter",swinfac_newprinter,-1);

	rb_define_method(cSwinPrinter,"printername",swinprn_pname,0);
	rb_define_method(cSwinPrinter,"startdoc",swinprn_startdoc,0);
	rb_define_method(cSwinPrinter,"startpage",swinprn_startpage,0);
	rb_define_method(cSwinPrinter,"endpage",swinprn_endpage,0);
	rb_define_method(cSwinPrinter,"enddoc",swinprn_enddoc,0);
	rb_define_method(cSwinPrinter,"abortdoc",swinprn_abortdoc,0);

/* for GDI functions */
	DEFINE_RUBY_GDIFUNCS(cSwinPrinter,swinprn_)

}

