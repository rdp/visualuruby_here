/*
###################################
#
# swingdi.c
# Programmed by nyasu <nyasu@osk.3web.ne.jp>
# Copyright 1999-2005  Nishikawa,Yasuhiro
#
# More information at http://vruby.sourceforge.net/index.html
#
###################################
*/

#include "swin.h"
#include "swingdi.h"

/*******************************************
*
*  Common GDI Functions for Swin::Window and ...
*
********************************************/

void swingdi_init(struct SwinGDIStruct *gs){
	gs->selectedObjFlag=0;
	gs->hdc=NULL;
}

void swingdi_release(struct SwinGDIStruct *gs){
	HPEN hpen;
	HBRUSH hbr;
	HDC hdc=gs->hdc;

	/* Release Objects */
	if(gs->selectedObjFlag){
		if(gs->selectedObjFlag & SWIN_OBJ_PEN){
			hpen=SelectObject(hdc,gs->hpenOrg);
			DeleteObject(hpen);
			gs->hpenOrg=NULL;
			RESET_BIT(gs->selectedObjFlag,SWIN_OBJ_PEN);
		}
		if(gs->selectedObjFlag & SWIN_OBJ_BRUSH){
			hbr=SelectObject(hdc,gs->hbrushOrg);
			DeleteObject(hbr);
			gs->hbrushOrg=NULL;
			RESET_BIT(gs->selectedObjFlag,SWIN_OBJ_BRUSH);
		}
		if(gs->selectedObjFlag & SWIN_OBJ_FONT){
			SelectObject(hdc,gs->hfontOrg);
			gs->hfontOrg=NULL;
			RESET_BIT(gs->selectedObjFlag,SWIN_OBJ_FONT);
		}

    }
}

VALUE
swingdi_setPen(int argc,VALUE * argv,struct SwinGDIStruct *gs){
	int color,width,style;
	HPEN hpen;

	if(argc<1){
		rb_raise(rb_eArgError,"need color");
		return Qfalse;
	}
	color=NUM2INT(argv[0]);
	width=(argc>1 && argv[1]!=Qnil)? NUM2INT(argv[1]) : 0;
	style=(argc>2 && argv[2]!=Qnil)? NUM2INT(argv[2]) : PS_SOLID;

	hpen=CreatePen(style,width,(COLORREF)color);
	hpen=SelectObject(gs->hdc,hpen);

	if(gs->selectedObjFlag & SWIN_OBJ_PEN){
		DeleteObject(hpen);
	} else {
		gs->hpenOrg=hpen;
	}
	SET_BIT(gs->selectedObjFlag, SWIN_OBJ_PEN);

	return Qtrue;
}

VALUE
swingdi_setBrush(int argc,VALUE * argv, struct SwinGDIStruct *gs){
	LOGBRUSH lb;
	HBRUSH hbr;

	if(argc<1){
		rb_raise(rb_eArgError,"need color");
		return Qfalse;
	}
	lb.lbColor = NUM2INT(argv[0]);
	lb.lbStyle =(argc>1 && argv[1]!=Qnil)? NUM2INT(argv[1]) : BS_SOLID;
	lb.lbHatch =(argc>2 && argv[2]!=Qnil)? NUM2INT(argv[2]) : HS_DIAGCROSS;


	hbr=CreateBrushIndirect(&lb);
	hbr=SelectObject(gs->hdc,hbr);

	if(gs->selectedObjFlag & SWIN_OBJ_BRUSH){
		DeleteObject(hbr);
	} else {
		gs->hbrushOrg=hbr;
	}
	SET_BIT(gs->selectedObjFlag, SWIN_OBJ_BRUSH);

	return Qtrue;
}

VALUE swingdi_setFont(struct SwinGDIStruct *gs, VALUE font){
	struct SwinFont* sf;
	HFONT ofont;

	Data_Get_Struct(font, struct SwinFont, sf);

	ofont=SelectObject(gs->hdc,sf->hfont);

	if(gs->selectedObjFlag & SWIN_OBJ_FONT){
		/**/
	} else {
		gs->hfontOrg=ofont;
	}
	SET_BIT(gs->selectedObjFlag, SWIN_OBJ_FONT);
	return Qtrue;
}


VALUE
swingdi_getPixel(struct SwinGDIStruct *gs,VALUE x,VALUE y){
	return INT2NUM( GetPixel(gs->hdc,NUM2INT(x),NUM2INT(y)) );
}

VALUE
swingdi_setPixel(struct SwinGDIStruct *gs,VALUE x,VALUE y,VALUE color){
	return INT2NUM( SetPixel(gs->hdc,NUM2INT(x),NUM2INT(y),NUM2UINT(color)) );
}



VALUE
swingdi_Rectangle(struct SwinGDIStruct *gs,VALUE xs,VALUE ys,VALUE xe,VALUE ye){
	Rectangle( gs->hdc,NUM2INT(xs),NUM2INT(ys),NUM2INT(xe),NUM2INT(ye) );
	return Qtrue;
}


VALUE
swingdi_RoundRectangle(struct SwinGDIStruct *gs,VALUE xs,VALUE ys,VALUE xe,VALUE ye,VALUE rx,VALUE ry){
	RoundRect( gs->hdc,
		NUM2INT(xs),NUM2INT(ys),NUM2INT(xe),NUM2INT(ye),
		NUM2INT(rx),NUM2INT(ry) );
	return Qtrue;
}

VALUE
swingdi_Chord(struct SwinGDIStruct *gs,
                     VALUE rxs, VALUE rys, VALUE rxe, VALUE rye,
                     VALUE lxs,VALUE lys,VALUE lxe, VALUE lye){
	Chord(gs->hdc,NUM2INT(rxs),NUM2INT(rys),NUM2INT(rxe),NUM2INT(rye),
	              NUM2INT(lxs),NUM2INT(lys),NUM2INT(lxe),NUM2INT(lye));
	return Qtrue;
}

VALUE
swingdi_Arc(struct SwinGDIStruct *gs,
                     VALUE rxs, VALUE rys, VALUE rxe, VALUE rye,
                     VALUE lxs,VALUE lys,VALUE lxe, VALUE lye){
	Arc(gs->hdc,NUM2INT(rxs),NUM2INT(rys),NUM2INT(rxe),NUM2INT(rye),
	            NUM2INT(lxs),NUM2INT(lys),NUM2INT(lxe),NUM2INT(lye));
	return Qtrue;
}

VALUE
swingdi_Ellipse(struct SwinGDIStruct *gs,VALUE rxs, VALUE rys, VALUE rxe, VALUE rye){
	Ellipse(gs->hdc,NUM2INT(rxs),NUM2INT(rys),NUM2INT(rxe),NUM2INT(rye) );
	return Qtrue;
}

VALUE
swingdi_Line(struct SwinGDIStruct *gs,VALUE xs,VALUE ys,VALUE xe,VALUE ye){
	MoveToEx( gs->hdc, NUM2INT(xs),NUM2INT(ys) ,NULL);
	LineTo(   gs->hdc, NUM2INT(xe),NUM2INT(ye) );
	
	return Qtrue;
}

VALUE
swingdi_MoveTo(struct SwinGDIStruct *gs,VALUE x,VALUE y){
	MoveToEx( gs->hdc, NUM2INT(x),NUM2INT(y) ,NULL);
	return Qtrue;
}
VALUE
swingdi_LineTo(struct SwinGDIStruct *gs,VALUE x,VALUE y){
	LineTo( gs->hdc, NUM2INT(x),NUM2INT(y) );
	return Qtrue;
}

VALUE
swingdi_drawText(int argc, VALUE* argv, struct SwinGDIStruct *gs){
    TCHAR* str;
    RECT rect;
    int len;
    unsigned int style;
    VALUE text;

		if(argc<5){
		rb_raise(rb_eArgError,"arguments not enough");
		return Qfalse;
	}

	text = SWIN_API_STR(argv[0]);

	str= (TCHAR*)RSTRING_PTR(text);
	rect.left = NUM2INT(argv[1]);  rect.top    = NUM2INT(argv[2]);
	rect.right= NUM2INT(argv[3]);  rect.bottom = NUM2INT(argv[4]);
	
	len  =(argc>5 && argv[5]!=Qnil)? NUM2INT(argv[5]) : lstrlen(str);
	style=(argc>6 && argv[6]!=Qnil)? NUM2INT(argv[6]) : DT_LEFT;

	DrawText(gs->hdc, str,len, &rect,style);
	return Qtrue;
}

VALUE
swingdi_FloodFill(int argc, VALUE* argv, struct SwinGDIStruct *gs){
	int x,y,color,type;
	BOOL r;

	if(argc<3) return Qfalse;
	
	x=NUM2INT(argv[0]);   y=NUM2INT(argv[1]);
	color=NUM2UINT(argv[2]);
	type= (argc>3 && argv[3]==Qtrue)? FLOODFILLSURFACE : FLOODFILLBORDER;

	r=ExtFloodFill( gs->hdc, x,y,color,type );
	if(!r) return Qfalse;
	return Qtrue;
}

VALUE
swingdi_setbkcolor(struct SwinGDIStruct *gs, VALUE color){
	SetBkColor(gs->hdc, NUM2UINT(color));
	return color;
}

VALUE
swingdi_getbkcolor(struct SwinGDIStruct *gs){
	return UINT2NUM( GetBkColor(gs->hdc) );
}

VALUE
swingdi_settextcolor(struct SwinGDIStruct *gs, VALUE color){
	SetTextColor(gs->hdc, NUM2UINT(color));
	return color;
}

VALUE
swingdi_gettextcolor(struct SwinGDIStruct *gs){
	return UINT2NUM( GetTextColor(gs->hdc) );
}


VALUE
swingdi_setbkmode(struct SwinGDIStruct *gs, VALUE mode){
	int opaque;
	if(mode) opaque=OPAQUE; else opaque=TRANSPARENT;
	opaque=SetBkMode(gs->hdc, opaque);
	if(opaque) return Qtrue;
	return Qfalse;
}

VALUE
swingdi_getbkmode(struct SwinGDIStruct *gs){
	int opaque;
	opaque=GetBkMode(gs->hdc);
	if(opaque) return Qtrue;
	return Qfalse;
}



VALUE
swingdi_drawDIBitmap(int argc,VALUE* argv,struct SwinGDIStruct *gs){
	int dx,dy,dw,dh,sx,sy,sscan,sline;
	struct SwinBitmap *sb;

	if(argc<1 || !rb_obj_is_kind_of(argv[0],cSwinBitmap)){
		rb_raise(rb_eArgError,"SWin::Bitmap required.");
		return Qfalse;
	}

	Data_Get_Struct(argv[0], struct SwinBitmap, sb);

	dx= (argc>1 && argv[1]!=Qnil)? NUM2INT(argv[1]) : 0;
	dy= (argc>2 && argv[2]!=Qnil)? NUM2INT(argv[2]) : 0;
	dw= (argc>3 && argv[3]!=Qnil)? NUM2INT(argv[3]) : sb->info->bmiHeader.biWidth;
	dh= (argc>4 && argv[4]!=Qnil)? NUM2INT(argv[4]) : sb->info->bmiHeader.biHeight;
	sx= (argc>5 && argv[5]!=Qnil)? NUM2INT(argv[5]) : 0;
	sy= (argc>6 && argv[6]!=Qnil)? NUM2INT(argv[6]) : 0;
	sscan = (argc>7 && argv[7]!=Qnil)? NUM2INT(argv[7]) : 0;
	sline = (argc>8 && argv[8]!=Qnil)? NUM2INT(argv[8]) : sb->info->bmiHeader.biHeight;

	SetDIBitsToDevice(gs->hdc,dx,dy,dw,dh,sx,sy,sscan,sline,
                      sb->bmp,sb->info, DIB_RGB_COLORS);
    return argv[0];
}

VALUE
swingdi_drawDDBitmap(int argc,VALUE* argv,struct SwinGDIStruct *gs){
	int dx,dy,dw,dh,sx,sy,rot;
	struct SwinCanvas *sc;
	BITMAP bmp;

	if(argc<1 || !rb_obj_is_kind_of(argv[0],cSwinCanvas)){
		rb_raise(rb_eArgError,"SWin::Canvas required.");
		return Qfalse;
	}

	Data_Get_Struct(argv[0], struct SwinCanvas, sc);
	GetObject(sc->hbmp,sizeof(BITMAP),&bmp);

	dx= (argc>1 && argv[1]!=Qnil)? NUM2INT(argv[1]) : 0;
	dy= (argc>2 && argv[2]!=Qnil)? NUM2INT(argv[2]) : 0;
	dw= (argc>3 && argv[3]!=Qnil)? NUM2INT(argv[3]) : bmp.bmWidth;
	dh= (argc>4 && argv[4]!=Qnil)? NUM2INT(argv[4]) : bmp.bmHeight;
	sx= (argc>5 && argv[5]!=Qnil)? NUM2INT(argv[5]) : 0;
	sy= (argc>6 && argv[6]!=Qnil)? NUM2INT(argv[6]) : 0;
	rot = (argc>7 && argv[7]!=Qnil)? NUM2INT(argv[7]) : SRCCOPY;

	BitBlt(gs->hdc,dx,dy,dw,dh,sc->gs.hdc,sx,sy,rot);
    return argv[0];
}

VALUE
swingdi_StretchDIBlt(int argc, VALUE* argv, struct SwinGDIStruct* gs){
	int dx,dy,dw,dh,sx,sy,sw,sh;
	int dwope;
	VALUE sourcebmp;
	enum {t_dib,t_ddb} stype;
	struct SwinCanvas *sc;
	struct SwinBitmap *sb;
	BITMAP bmp;
	int r;

	if(argc<5){
		rb_raise(rb_eArgError,"not enough arguments specified");
		return Qfalse;
	}
	dx = NUM2INT(argv[0]);  dy = NUM2INT(argv[1]);
	dw = NUM2INT(argv[2]);  dh = NUM2INT(argv[3]);

	sourcebmp=argv[4];
	if(rb_obj_is_kind_of(sourcebmp,cSwinBitmap)){
		Data_Get_Struct(sourcebmp, struct SwinBitmap, sb);
		sx = 0;   sy = 0;
		sw = sb->info->bmiHeader.biWidth;
		sh = sb->info->bmiHeader.biHeight;
		stype = t_dib;
	} else if(rb_obj_is_kind_of(sourcebmp,cSwinCanvas)){
		Data_Get_Struct(sourcebmp, struct SwinCanvas, sc);
		GetObject(sc->hbmp,sizeof(BITMAP),&bmp);
		sx = 0;   sy = 0;
		sw = bmp.bmWidth;
		sh = bmp.bmHeight;
		stype = t_ddb;
	} else {
		rb_raise(rb_eArgError,"Source bitmap not available");
		return Qfalse;
	}

	if(argc>8){
		sx = NUM2INT(argv[5]);   sy = NUM2INT(argv[6]);
		sw = NUM2INT(argv[7]);   sh = NUM2INT(argv[8]);
	}
	dwope = (argc>9)? NUM2INT(argv[9]) : SRCCOPY;

	switch(stype){
		case t_dib:
			r = StretchDIBits(gs->hdc,dx,dy,dw,dh,
						sx,sy,sw,sh,sb->bmp,sb->info,
						DIB_RGB_COLORS,dwope);
			break;
		case t_ddb:
			r = StretchBlt(gs->hdc,dx,dy,dw,dh,sc->gs.hdc,sx,sy,sw,sh,dwope);
			break;
		default:
			return Qfalse;
	}
	return INT2NUM(r);
}

VALUE
swingdi_getstretchmode(struct SwinGDIStruct* gs){
	return INT2NUM(GetStretchBltMode(gs->hdc));
}
VALUE
swingdi_setstretchmode(struct SwinGDIStruct* gs, VALUE mode){
	return INT2NUM(SetStretchBltMode(gs->hdc,NUM2INT(mode)));
}


VALUE
swingdi_devicecap(struct SwinGDIStruct *gs, VALUE code){
	return INT2NUM(GetDeviceCaps(gs->hdc,NUM2INT(code)));
}

VALUE
swingdi_size(struct SwinGDIStruct *gs){
	VALUE robj;
	robj = rb_ary_new2(2);
	rb_ary_push(robj,INT2NUM(GetDeviceCaps(gs->hdc,HORZRES)));
	rb_ary_push(robj,INT2NUM(GetDeviceCaps(gs->hdc,VERTRES)));
	return robj;
}
VALUE
swingdi_sizebymm(struct SwinGDIStruct *gs){
	VALUE robj;
	robj = rb_ary_new2(2);
	rb_ary_push(robj,INT2NUM(GetDeviceCaps(gs->hdc,HORZSIZE)));
	rb_ary_push(robj,INT2NUM(GetDeviceCaps(gs->hdc,VERTSIZE)));
	return robj;
}
VALUE
swingdi_getdpi(struct SwinGDIStruct *gs){
	VALUE robj;
	robj = rb_ary_new2(2);
	rb_ary_push(robj,INT2NUM(GetDeviceCaps(gs->hdc,LOGPIXELSX)));
	rb_ary_push(robj,INT2NUM(GetDeviceCaps(gs->hdc,LOGPIXELSY)));
	return robj;
}


VALUE
swingdi_gettextextent(struct SwinGDIStruct* gs,VALUE text){
	VALUE robj;
	VALUE textstr = SWIN_API_STR(text);
	SIZE sz;
	
	GetTextExtentPoint(gs->hdc,
		(TCHAR*)RSTRING_PTR(textstr),RSTRING_LEN(textstr)/sizeof(TCHAR),&sz);
	
	robj = rb_ary_new2(2);
	rb_ary_push(robj,INT2NUM(sz.cx));
	rb_ary_push(robj,INT2NUM(sz.cy));
	return robj;
}

VALUE
swingdi_gethdc(struct SwinGDIStruct* gs){
	return UINT2NUM( (unsigned)gs->hdc );
}


#define SwinGDIFuncCheck \
	struct Swin *sw; \
	Data_Get_Struct(obj,struct Swin,sw); \
	SWIN_WINEXISTCHECK(sw); \
	SWIN_PAINTINGCHECK(sw); \
	sw->gs.hdc = sw->ps.hdc;


DEFINE_GDI_ALL_FUNCTIONS_MACRO(swin_,SwinGDIFuncCheck)


/*********************
*
*/

VALUE mSwinPenStyle;
VALUE mSwinBrushStyle;
VALUE mSwinHatchStyle;
void Init_Consts(){
	mSwinPenStyle = rb_define_module_under(mSwin,"PEN");
	rb_define_const(mSwinPenStyle,"SOLID",UINT2NUM(PS_SOLID));
	rb_define_const(mSwinPenStyle,"DASH",UINT2NUM(PS_DASH));
	rb_define_const(mSwinPenStyle,"DOT",UINT2NUM(PS_DOT));
	rb_define_const(mSwinPenStyle,"DASHDOT",UINT2NUM(PS_DASHDOT));
	rb_define_const(mSwinPenStyle,"DASHDOTDOT",UINT2NUM(PS_DASHDOTDOT));
	rb_define_const(mSwinPenStyle,"NULL",UINT2NUM(PS_NULL));
	rb_define_const(mSwinPenStyle,"INSIDEFRAME",UINT2NUM(PS_INSIDEFRAME));

	mSwinBrushStyle = rb_define_module_under(mSwin,"BRUSH");
	rb_define_const(mSwinBrushStyle,"DIBPATTERN",UINT2NUM(BS_DIBPATTERN));
	rb_define_const(mSwinBrushStyle,"HATCHED",UINT2NUM(BS_HATCHED));
	rb_define_const(mSwinBrushStyle,"HOLLOW",UINT2NUM(BS_HOLLOW));
	rb_define_const(mSwinBrushStyle,"PATTERN",UINT2NUM(BS_PATTERN));
	rb_define_const(mSwinBrushStyle,"NULL",UINT2NUM(BS_NULL));
	rb_define_const(mSwinBrushStyle,"SOLID",UINT2NUM(BS_SOLID));

	mSwinHatchStyle = rb_define_module_under(mSwin,"HATCH");
	rb_define_const(mSwinHatchStyle,"BDIAGONAL",UINT2NUM(HS_BDIAGONAL));
	rb_define_const(mSwinHatchStyle,"CROSS",UINT2NUM(HS_CROSS));
	rb_define_const(mSwinHatchStyle,"DIAGCROSS",UINT2NUM(HS_DIAGCROSS));
	rb_define_const(mSwinHatchStyle,"FDIAGONAL",UINT2NUM(HS_FDIAGONAL));
	rb_define_const(mSwinHatchStyle,"HORIZONTAL",UINT2NUM(HS_HORIZONTAL));
	rb_define_const(mSwinHatchStyle,"VERTICAL",UINT2NUM(HS_VERTICAL));
}



void Init_swinGDI(){
	DEFINE_RUBY_GDIFUNCS(cSwin,swin_)
	Init_Consts();
}
