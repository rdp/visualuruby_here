/*
###################################
#
# swincvs.c
# Programmed by nyasu <nyasu@osk.3web.ne.jp>
# Copyright 1999-2005  Nishikawa,Yasuhiro
#
# More information at http://vruby.sourceforge.net/index.html
#
###################################
*/


#include "swin.h"
#include "swingdi.h"

/* Thanks Komatsu-san.
 *
 * Cygwin's definition
 *   #define LR_LOADREALSIZE 128
 * VC++'s definition
 *   #define LR_VGACOLOR 0x0080
 */
#ifndef LR_LOADREALSIZE
# define LR_LOADREALSIZE 0x0080
#endif

VALUE cSwinCanvas;

void release_canvas(struct SwinCanvas* sc){
	SelectObject(sc->gs.hdc,sc->horigbmp);  /* de-select */
	DeleteObject(sc->hbmp);
	DeleteDC(sc->gs.hdc);
	swingdi_release(&sc->gs);
	free(sc);
}

static VALUE
swinfac_newcanvas(VALUE klass,VALUE width,VALUE height){
	struct SwinCanvas* sc;
	unsigned int h,w;
	VALUE robj;
	HDC hdc;
	
    robj = Data_Make_Struct(cSwinCanvas,struct SwinCanvas,0,release_canvas,sc);

	h=NUM2UINT(height);
	w=NUM2UINT(width);

	hdc=GetDC(NULL); 
	sc->hbmp=CreateCompatibleBitmap(hdc,w,h);
	sc->gs.hdc=CreateCompatibleDC(hdc);
	sc->horigbmp=SelectObject(sc->gs.hdc,sc->hbmp);
	ReleaseDC(NULL,hdc); 
	sc->gs.selectedObjFlag=0;

	if(sc->hbmp==NULL || sc->gs.hdc==NULL){
		rb_raise(rb_eRuntimeError,"failed to create compatible handle");
		return Qfalse;
	}

	return robj;
}

static VALUE
swinfac_canvasfromfile(VALUE klass,VALUE fname){
	struct SwinCanvas* sc;
	VALUE robj;
	TCHAR pathbuffer[MYMAXPATH*2*sizeof(TCHAR)];
	HDC hdc;

	if(TYPE(fname)!=T_STRING){
		rb_raise(rb_eTypeError,"filename should be String.");
		return Qfalse;
	}
	
    robj = Data_Make_Struct(cSwinCanvas,struct SwinCanvas,0,release_canvas,sc);
	
	hdc=GetDC(NULL);
	sc->gs.hdc=CreateCompatibleDC(hdc);

	path_conv_to_win(SWIN_API_STR_PTR(fname),pathbuffer);

/* LR_LOADFROMFILE <= valid for only Win95/98 ? */
	sc->hbmp=LoadImage( NULL,pathbuffer,IMAGE_BITMAP,0,0,
	                    LR_LOADREALSIZE | LR_CREATEDIBSECTION | LR_LOADFROMFILE);

	sc->horigbmp=SelectObject(sc->gs.hdc,sc->hbmp);
	ReleaseDC(0,hdc);
	return robj;
}


static VALUE
swincanvas_getWidth(VALUE obj){
	struct SwinCanvas *sc; 
	BITMAP bmp;
	Data_Get_Struct(obj,struct SwinCanvas,sc); 
	GetObject(sc->hbmp,sizeof(BITMAP),&bmp);

	return UINT2NUM(bmp.bmWidth);
}
static VALUE
swincanvas_getHeight(VALUE obj){
	struct SwinCanvas *sc; 
	BITMAP bmp;
	Data_Get_Struct(obj,struct SwinCanvas,sc); 
	GetObject(sc->hbmp,sizeof(BITMAP),&bmp);

	return UINT2NUM(bmp.bmHeight);
}

void
setbmpinfo(BITMAPINFO *bi,BITMAP *bmp){
	bi->bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
	bi->bmiHeader.biWidth=bmp->bmWidth;
	bi->bmiHeader.biHeight=bmp->bmHeight;
	bi->bmiHeader.biPlanes=1;
	bi->bmiHeader.biBitCount=24;
	bi->bmiHeader.biCompression=BI_RGB;
	bi->bmiHeader.biSizeImage=0;
	bi->bmiHeader.biXPelsPerMeter=0;
	bi->bmiHeader.biYPelsPerMeter=0;
	bi->bmiHeader.biClrUsed=0;
	bi->bmiHeader.biClrImportant=0;
}

static VALUE
swincanvas_getInfo(VALUE obj){
	struct SwinCanvas *sc; 
	BITMAPINFO bi;
	BITMAP bmp;

	Data_Get_Struct(obj,struct SwinCanvas,sc); 
	GetObject(sc->hbmp,sizeof(BITMAP),&bmp);
	setbmpinfo(&bi,&bmp);
	return rb_str_new((char*)&bi,sizeof(BITMAPINFO));
}

static VALUE
swincanvas_getInfoAndBmp(VALUE obj){
	int r;
	struct SwinCanvas *sc; 
	BITMAPINFO bi;
	BITMAP bmp;
	HBITMAP tbmp;
	VALUE robj;
	char* bitmapstr;
	unsigned long bitmapsize;

	Data_Get_Struct(obj,struct SwinCanvas,sc); 

	GetObject(sc->hbmp,sizeof(BITMAP),&bmp);
	setbmpinfo(&bi,&bmp);   /* set info */

	tbmp=CreateCompatibleBitmap(sc->gs.hdc,8,1);
	SelectObject(sc->gs.hdc,tbmp);              /* de-select */

	bitmapsize=(bmp.bmWidth+8)*(bmp.bmHeight+8)*3;
/*	bitmapstr=malloc(bitmapsize); */
	MYMALLOC(bitmapstr,bitmapsize,Qfalse);
	r=GetDIBits(sc->gs.hdc,sc->hbmp,0,bmp.bmHeight,bitmapstr,&bi,DIB_RGB_COLORS);
	robj=rb_str_new(bitmapstr,bitmapsize);
	free(bitmapstr);
	if(r==0) 
		robj=Qnil;
	else{
		robj=rb_ary_new3(2,rb_str_new((char*)&bi,sizeof(BITMAPINFO)),robj);
	}
	SelectObject(sc->gs.hdc,sc->hbmp);          /* re-select */
	DeleteObject(tbmp);
	return robj;
}

static VALUE
swincanvas_hbitmap(VALUE obj) {
	struct SwinCanvas *sc; 
	Data_Get_Struct(obj,struct SwinCanvas,sc); 
	return UINT2NUM((unsigned)sc->hbmp);
}


/*************************************
*  for GDI functions
*************************************/

#define SwinCanvasGDIFuncInit \
	struct SwinCanvas *sw; \
	Data_Get_Struct(obj,struct SwinCanvas,sw); 

DEFINE_GDI_ALL_FUNCTIONS_MACRO(swincanvas_,SwinCanvasGDIFuncInit)


/**********************/

void
Init_swinCanvas(){
	cSwinCanvas=rb_define_class_under(mSwin,"Canvas",rb_cObject);
	rb_define_method(cSwinFactory,"newcanvas",swinfac_newcanvas,2);
	rb_define_method(cSwinFactory,"canvasfromfile",swinfac_canvasfromfile,1);
	rb_define_singleton_method(cSwinCanvas,"new",swin_avoid_new,-1);

	rb_define_method(cSwinCanvas,"infoStr",swincanvas_getInfo,0);
	rb_define_method(cSwinCanvas,"infoandbmp",swincanvas_getInfoAndBmp,0);
	rb_define_method(cSwinCanvas,"width",swincanvas_getWidth,0);
	rb_define_method(cSwinCanvas,"height",swincanvas_getHeight,0);

	rb_define_method(cSwinCanvas,"hbitmap",swincanvas_hbitmap,0);

	DEFINE_RUBY_GDIFUNCS(cSwinCanvas,swincanvas_)
}


