/*
###################################
#
# swinrsc.c
# Programmed by nyasu <nyasu@osk.3web.ne.jp>
# Copyright 1999-2005  Nishikawa,Yasuhiro
#
# More information at http://vruby.sourceforge.net/index.html
#
###################################
*/

#include "swin.h"

/* prototypes */
void release_icon(struct SwinIcon*);


/*********************************
    Font    
**********************************/

VALUE cSwinFont;
VALUE eSwinFontNotExistError;

void release_font(struct SwinFont* sf){
	if(sf->hfont==0) return;
	DeleteObject(sf->hfont); 
	sf->hfont=0;
	free(sf->fontname);
	free(sf);
}


static VALUE
swinfont_new(int argc,VALUE * argv, VALUE klass){
/* argv:  fontname,height [,style,weight,width,escape,orient,pitchfamily] */
/*
pitch:  0 as default,1 as fixed, 2 as variable, 0x4 for truetype
family: 0x0 as dontcare,0x10 as roman, 0x20 as swiss,0x30 as modern,0x40 as script
        0x50 as decorative
weight: 0-9
*/

	struct SwinFont* sf;
	VALUE robj;

	int   height,width,escape,orient,weight;
	DWORD italic,underline,strike; /*,charset,oprec,cprec,quality; */ 
	DWORD pitchfamily;
	unsigned char charset;
	VALUE faceobj;
	TCHAR* face;
	int facelen;

	if(argc<2){
		rb_raise(rb_eArgError,"Need font face name and height");
		return Qfalse;
	}

    robj = Data_Make_Struct(cSwinFont, struct SwinFont, 0, release_font, sf);
	faceobj = SWIN_API_STR(argv[0]);
	facelen = RSTRING_LEN(faceobj);
/*	face = sf->fontname = malloc(facelen+1);*/
	MYMALLOC(sf->fontname,facelen+1,Qfalse);
	face = sf->fontname;
	lstrcpyn(face, (TCHAR*)RSTRING_PTR(faceobj),facelen/sizeof(TCHAR)+1);

    sf->height = height = NUM2INT(argv[1]);

	sf->style = (argc>2 && argv[2]!=Qnil)? NUM2UINT(argv[2]) : 0;
	italic = (sf->style) & SWINFONT_ITALIC;
	underline = (sf->style) & SWINFONT_ULINE;
	strike = (sf->style) & SWINFONT_STRIKE;

	sf->weight = weight = ((argc>3 && argv[3]!=Qnil)? NUM2INT(argv[3]) : 0)*100;
	sf->width  = width = (argc>4 && argv[4]!=Qnil)? NUM2INT(argv[4]) : 0;
	sf->escape = escape = (argc>5 && argv[5]!=Qnil)? NUM2INT(argv[5]) : 0;
	sf->orient = orient = (argc>6 && argv[6]!=Qnil)? NUM2INT(argv[6]) : 0;
	sf->pitchfamily = pitchfamily = (argc>7 && argv[7]!=Qnil)? NUM2INT(argv[7]) : 0x04;
	sf->charset = charset = (argc>8 && argv[8]!=Qnil)? NUM2INT(argv[8]) : DEFAULT_CHARSET;

	sf->hfont = CreateFont(height,width,escape,orient,weight,
	                  italic,underline,strike,charset,
	                  OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
	                  pitchfamily,face);
	if(!sf->hfont){
		rb_raise(rb_eRuntimeError,"failed to create font");
		return Qfalse;
	}
	return robj;
}

static VALUE
swinfont_delete(VALUE obj){
	struct SwinFont *sf;
	Data_Get_Struct(obj, struct SwinFont, sf);
	SWIN_FONTALIVECHECK(sf);
	DeleteObject(sf->hfont);
	sf->hfont=0;
	return Qtrue;
}

static VALUE
swinfont_fname(VALUE obj){
	struct SwinFont *sf;
	Data_Get_Struct(obj, struct SwinFont, sf);
	SWIN_FONTALIVECHECK(sf);
	return SWIN_OUTAPI_STR_NEW2(sf->fontname);
}

static VALUE
swinfont_height(VALUE obj){
	struct SwinFont *sf;
	Data_Get_Struct(obj, struct SwinFont, sf);
	SWIN_FONTALIVECHECK(sf);
	return UINT2NUM(sf->height);
}

static VALUE
swinfont_hfont(VALUE obj){
	struct SwinFont *sf;
	Data_Get_Struct(obj, struct SwinFont, sf);
	SWIN_FONTALIVECHECK(sf);
	return UINT2NUM((unsigned)sf->hfont);
}

static VALUE
swinfont_array(VALUE obj){
	struct SwinFont *sf;
	VALUE robj;

	Data_Get_Struct(obj, struct SwinFont, sf);
	SWIN_FONTALIVECHECK(sf);

	robj = rb_ary_new();
	rb_ary_push(robj,SWIN_OUTAPI_STR_NEW2(sf->fontname));
	rb_ary_push(robj,INT2NUM(sf->height));
	rb_ary_push(robj,INT2NUM(sf->style));
	rb_ary_push(robj,INT2NUM(sf->weight));
	rb_ary_push(robj,INT2NUM(sf->width));
	rb_ary_push(robj,INT2NUM(sf->escape));
	rb_ary_push(robj,INT2NUM(sf->orient));
	rb_ary_push(robj,INT2NUM(sf->pitchfamily));
	rb_ary_push(robj,INT2NUM(sf->charset));

	return robj;
}

/*********************************
    Cursor    
**********************************/

VALUE cSwinCursor;
VALUE eSwinCursorNotExistError;

void release_cursor(struct SwinCursor *sc){
	if(sc->hcursor==0) return;
	if(!sc->shared) DestroyCursor(sc->hcursor);
	sc->hcursor=0;
	free(sc);
}

static VALUE
swincursor_new(VALUE klass,VALUE hotspot,VALUE size,VALUE aplane,VALUE xplane){
	int hx,hy;
	int sx,sy,ssx,ssy;
	int psize;
	struct SwinCursor* sc;
	VALUE tmp,robj;

	if(TYPE(aplane)!=T_STRING || TYPE(xplane)!=T_STRING) {
		rb_raise(rb_eArgError,"and-plane and xor-plane must be String");
		return Qfalse;
	}
	ssx=GetSystemMetrics(SM_CXCURSOR);
	ssy=GetSystemMetrics(SM_CYCURSOR);

	tmp = rb_ary_entry(size,0); sx = NUM2INT(tmp);
	tmp = rb_ary_entry(size,1); sy = NUM2INT(tmp);

	if(sx!=ssx || sy!=ssy){
		rb_raise(rb_eRuntimeError,"cursor size mismatch [%d,%d] for [%d,%d]",
		         sx,sy,ssx,ssy);
		return Qfalse;
	}

	tmp = rb_ary_entry(hotspot,0); hx = NUM2INT(tmp);
	tmp = rb_ary_entry(hotspot,1); hy = NUM2INT(tmp);
	
	psize = sx*sy/8;

	if( (psize>RSTRING_LEN(aplane)) || (psize>RSTRING_LEN(xplane)) ){	
		rb_raise(rb_eRuntimeError,"short plane size for %d",psize);
		return Qfalse;
	}

	robj = Data_Make_Struct(cSwinCursor,struct SwinCursor,0,release_cursor,sc);
	sc->sysflag = FALSE;
	sc->hotspot = (hx<<16) + hy;
	sc->hcursor = CreateCursor(hInstance,hx,hy,sx,sy,
	                RSTRING_PTR(aplane),RSTRING_PTR(xplane) );
	sc->shared = FALSE;
	if(sc->hcursor==NULL){
		rb_raise(rb_eRuntimeError,"fail to create cursor");
		return Qfalse;
	}
	return robj;
}

static VALUE
swincursor_load(VALUE klass,VALUE fname){
	struct SwinCursor* sc;
	VALUE robj;
	TCHAR pathbuffer[MYMAXPATH*2*sizeof(TCHAR)];

	robj = Data_Make_Struct(cSwinCursor,struct SwinCursor,0,release_cursor,sc);
	sc->sysflag=FALSE;
	sc->hotspot=0;

	path_conv_to_win(SWIN_API_STR_PTR(fname),pathbuffer);

	sc->hcursor=LoadCursorFromFile(pathbuffer);
	if(sc->hcursor==NULL){
		rb_raise(rb_eRuntimeError,"fail to create cursor");
		return Qfalse;
	}
	sc->shared = TRUE;

	return robj;
}

static VALUE
swincursor_delete(VALUE obj){
	struct SwinCursor *sc;
	Data_Get_Struct(obj, struct SwinCursor, sc);
	SWIN_CURSORALIVECHECK(sc);
	DestroyCursor(sc->hcursor);
	sc->hcursor=0;
	return Qtrue;
}

static VALUE
swincursor_hotspot(VALUE obj){
	struct SwinCursor *sc;
	VALUE robj;
	Data_Get_Struct(obj, struct SwinCursor, sc);
	SWIN_CURSORALIVECHECK(sc);
	
	robj = rb_ary_new3(2,
		     INT2NUM( (sc->hotspot)>>16 ),INT2NUM( (sc->hotspot)&0xffff) ); 
	return robj;
}

static VALUE
swincursor_hcursor(VALUE obj){
	struct SwinCursor *sc;
	Data_Get_Struct(obj, struct SwinCursor, sc);
	SWIN_CURSORALIVECHECK(sc);
	return UINT2NUM((unsigned)sc->hcursor);
}

static VALUE
swincursor_sysf(VALUE obj){
	struct SwinCursor *sc;
	Data_Get_Struct(obj, struct SwinCursor, sc);
	SWIN_CURSORALIVECHECK(sc);
	if(sc->sysflag){
		return Qtrue;
	}
	return Qfalse;
}


static VALUE
swinapp_setcursor(VALUE obj,VALUE cur){
	HCURSOR hc;
	struct SwinCursor *sc;

	if(TYPE(cur)==T_FIXNUM){
		hc = (HCURSOR)NUM2UINT(cur);
	} else {
		Data_Get_Struct(cur, struct SwinCursor, sc);
		SWIN_CURSORALIVECHECK(sc);
		hc = sc->hcursor;
	}
	rb_iv_set(obj,"cursor",cur);
	SetCursor(hc);
	return UINT2NUM((unsigned)hc);
}

static VALUE
swinapp_getcursor(VALUE obj){
	struct SwinCursor *sc;
	Data_Get_Struct(obj, struct SwinCursor, sc);
	
	return UINT2NUM((unsigned)GetCursor());
}

static VALUE
swinapp_showcursor(VALUE obj,VALUE flag){
	if(!flag){
		ShowCursor(TRUE);
	} else {
		ShowCursor(FALSE);
	}
	return flag;
}

VALUE
systemcursor(void* ctype){
	struct SwinCursor *sc;
	VALUE robj;

	robj = Data_Make_Struct(cSwinCursor,struct SwinCursor,0,release_cursor,sc);
	sc->hotspot=0;
	sc->sysflag=TRUE;
	sc->hcursor=LoadCursor(NULL,ctype);
	return robj;
}

/*****
*  System Cursors
******/
VALUE mSwinSysCur;

#define SYSCURSOR_GET(n) \
static VALUE \
swinapp_getsysc##n(){ \
	return systemcursor(IDC_##n); \
}

SYSCURSOR_GET(ARROW)
SYSCURSOR_GET(CROSS)
SYSCURSOR_GET(IBEAM)
SYSCURSOR_GET(ICON)
SYSCURSOR_GET(SIZE)
SYSCURSOR_GET(SIZENESW)
SYSCURSOR_GET(SIZENS)
SYSCURSOR_GET(SIZENWSE)
SYSCURSOR_GET(SIZEWE)
SYSCURSOR_GET(UPARROW)
SYSCURSOR_GET(WAIT)


/************************
* ImageList
*************************/

VALUE cSwinImagelist;
VALUE eSwinImagelistNotExistError;

void release_ilist(struct SwinImagelist* si){
	if(si->himagelist) ImageList_Destroy(si->himagelist);
	free(si);
}

static VALUE
swinilist_create(int argc,VALUE* argv, VALUE klass){
	struct SwinImagelist *si;
	VALUE robj;
	int cx,cy;
	UINT flag;
	int initial,grow;

	cx   = (argc>0 && argv[0]!=Qnil)? NUM2UINT(argv[0]) : 32;
	cy   = (argc>1 && argv[1]!=Qnil)? NUM2UINT(argv[1]) : 32;
	flag = (argc>2 && argv[2]!=Qnil)? NUM2UINT(argv[2]) : ILC_COLOR;

	initial = (argc>3 && argv[3]!=Qnil)? NUM2UINT(argv[3]) : 0;
	grow   =  (argc>4 && argv[4]!=Qnil)? NUM2UINT(argv[4]) : 1;

	robj = Data_Make_Struct(cSwinImagelist, 
	                        struct SwinImagelist,0,release_ilist, si);
	si->himagelist = ImageList_Create(cx,cy,flag,initial,grow);
	si->flags = flag;
	if(si->himagelist==NULL){
		rb_raise(rb_eRuntimeError,"ImageList_Create failed");
		return Qfalse;
	}

	return robj;
}

static VALUE
swinilist_delete(VALUE obj){
	struct SwinImagelist* si;
	Data_Get_Struct(obj,struct SwinImagelist,si);
	SWIN_IMAGELISTALIVECHECK(si)
	release_ilist(si);
	si->himagelist=NULL;
	return obj;
}

HBITMAP swl_copyimage(struct SwinBitmap* sb){
	HDC ohdc,hdc;
	HBITMAP img,obmp;
	int w,h;

	w = sb->info->bmiHeader.biWidth;
	h = sb->info->bmiHeader.biHeight;

	ohdc = GetDC(NULL);
	img = CreateCompatibleBitmap(ohdc,w,h);
	hdc = CreateCompatibleDC(ohdc);
	ReleaseDC(NULL,ohdc);

	obmp = SelectObject(hdc,img);
	SetDIBitsToDevice(hdc,0,0,w,h,0,0,0,h,sb->bmp,sb->info,DIB_RGB_COLORS);
	SelectObject(hdc,obmp);
	DeleteDC(hdc);
	return img;
}

static VALUE
swinilist_add(int argc, VALUE* argv, VALUE obj){
	struct SwinBitmap* sb;
	struct SwinImagelist* si;
	HBITMAP img,mask;
	int r;

	if(argc<1){
		rb_raise(rb_eArgError,"no argument of image");
		return Qfalse;
	}

	if(rb_obj_is_kind_of(argv[0],cSwinBitmap)){
		Data_Get_Struct(argv[0],struct SwinBitmap,sb);
		img = swl_copyimage(sb);
	} else {
		rb_raise(rb_eArgError,"Not a bitmap");
		return Qfalse;
	}

	if(argc<2 || argv[1]==Qnil) {
		mask = NULL;
	} else if(rb_obj_is_kind_of(argv[1],cSwinBitmap)){
		Data_Get_Struct(argv[1],struct SwinBitmap,sb);
		mask = swl_copyimage(sb);
	} else {
		rb_raise(rb_eArgError,"Not a bitmap for mask");
		return Qfalse;
	}
	Data_Get_Struct(obj,struct SwinImagelist,si);
	SWIN_IMAGELISTALIVECHECK(si)
	r=ImageList_Add(si->himagelist,img,mask);

	DeleteObject(img);
	if(mask)DeleteObject(mask);

	if(r<0){
		rb_raise(rb_eRuntimeError,"Image not added");
		return Qfalse;
	}
	return INT2NUM(r);
}

static VALUE
swinilist_addmasked(VALUE obj,VALUE bitmap,VALUE maskcolor){
	struct SwinBitmap* sb;
	struct SwinImagelist* si;
	HBITMAP img;
	int r;

	if(rb_obj_is_kind_of(bitmap,cSwinBitmap)){
		Data_Get_Struct(bitmap,struct SwinBitmap,sb);
		img = swl_copyimage(sb);
	} else {
		rb_raise(rb_eArgError,"Not a bitmap for arg1");
		return Qfalse;
	}

	Data_Get_Struct(obj,struct SwinImagelist,si);
	SWIN_IMAGELISTALIVECHECK(si)
	r=ImageList_AddMasked(si->himagelist,img,NUM2UINT(maskcolor));
	DeleteObject(img);

	if(r<0){
		rb_raise(rb_eRuntimeError,"Image not added");
		return Qfalse;
	}
	return INT2NUM(r);
}

static VALUE
swinilist_removeimage(VALUE obj,VALUE idx){
	struct SwinImagelist* si;
	int r;
	Data_Get_Struct(obj,struct SwinImagelist,si);
	r=ImageList_Remove(si->himagelist,NUM2INT(idx));
	if(r) return Qtrue;
	return Qfalse;
}

static VALUE
swinilist_removeallimage(VALUE obj){
	struct SwinImagelist* si;
	int r;
	Data_Get_Struct(obj,struct SwinImagelist,si);
	SWIN_IMAGELISTALIVECHECK(si)
	r=ImageList_RemoveAll(si->himagelist);
	if(r) return Qtrue;
	return Qfalse;
}
static VALUE
swinilist_countimage(VALUE obj){
	struct SwinImagelist* si;
	int r;
	Data_Get_Struct(obj,struct SwinImagelist,si);
	SWIN_IMAGELISTALIVECHECK(si)
	r=ImageList_GetImageCount(si->himagelist);
	return INT2NUM(r);
}

static VALUE
swinilist_hilist(VALUE obj){
	struct SwinImagelist* si;
	Data_Get_Struct(obj,struct SwinImagelist,si);
	SWIN_IMAGELISTALIVECHECK(si)
	return UINT2NUM((unsigned int)si->himagelist);
}

static VALUE
swinilist_geticon(int argc, VALUE* argv, VALUE obj){
	struct SwinImagelist* si;
	struct SwinIcon* sico;
	int i;
	UINT flags;
	HICON hicon;
	VALUE robj;

	if(argc<1){
		rb_raise(rb_eArgError,"Need index for imagelist");
		return Qfalse;
	}
	i = NUM2UINT(argv[0]);
	flags = (argc>=2)? NUM2UINT(argv[1]) : ILD_NORMAL;

	Data_Get_Struct(obj,struct SwinImagelist,si);
	SWIN_IMAGELISTALIVECHECK(si)
	hicon=ImageList_GetIcon(si->himagelist,i,flags);
	if(hicon==NULL){
		rb_raise(rb_eRuntimeError,"ImageList_GetIcon failed for %d\n",i);
		return Qfalse;
	}
	robj = Data_Make_Struct(cSwinIcon,struct SwinIcon,0,release_icon,sico);
	sico->hicon = hicon;
	sico->shared = FALSE;
	return robj;
}

static VALUE
swinilist_getimageinfo(VALUE obj, VALUE idx){
	struct SwinImagelist* si;
	IMAGEINFO imi;
	int index;
	VALUE robj,rect;

	index = NUM2INT(idx);

	Data_Get_Struct(obj,struct SwinImagelist,si);
	SWIN_IMAGELISTALIVECHECK(si)
	if(ImageList_GetImageInfo(si->himagelist,index,&imi)==0){
		rb_raise(rb_eRuntimeError,
			"ImageList_GetImageInfo failed for index:%d",index);
		return Qfalse;
	}
	robj = rb_ary_new2(3);
	rb_ary_push(robj,UINT2NUM((unsigned)imi.hbmImage));
	rb_ary_push(robj,UINT2NUM((unsigned)imi.hbmMask));
	rect = rb_ary_new2(4);
	rb_ary_push(rect,INT2NUM(imi.rcImage.left));
	rb_ary_push(rect,INT2NUM(imi.rcImage.top));
	rb_ary_push(rect,INT2NUM(imi.rcImage.right));
	rb_ary_push(rect,INT2NUM(imi.rcImage.bottom));
	rb_ary_push(robj,rect);
	return robj;
}

static VALUE
swinilist_getbkcolor(VALUE obj){
	struct SwinImagelist* si;
	COLORREF c;
	Data_Get_Struct(obj,struct SwinImagelist,si);
	SWIN_IMAGELISTALIVECHECK(si)
	c = ImageList_GetBkColor(si->himagelist);
	if(c==CLR_NONE) return Qnil;
	return UINT2NUM((unsigned)c);
}

static VALUE
swinilist_setbkcolor(VALUE obj, VALUE bkc){
	struct SwinImagelist* si;
	COLORREF c;
	Data_Get_Struct(obj,struct SwinImagelist,si);
	SWIN_IMAGELISTALIVECHECK(si)

	if(NIL_P(bkc)) c=CLR_NONE; else c = (COLORREF)NUM2UINT(bkc);

	c = ImageList_SetBkColor(si->himagelist,c);
	return obj;
}

static VALUE
swinilist_addbmp(VALUE obj, VALUE bmpfname, VALUE maskfname){
	struct SwinImagelist* si;
	HBITMAP bitmap, mask;
	TCHAR pathbuffer[MYMAXPATH*2*sizeof(TCHAR)];
	int r;

	path_conv_to_win(SWIN_API_STR_PTR(bmpfname),pathbuffer);
	bitmap = LoadImage(NULL,pathbuffer,IMAGE_BITMAP,0,0,
		LR_DEFAULTCOLOR | LR_DEFAULTSIZE | LR_LOADFROMFILE);
	if(bitmap==NULL){
		rb_raise(rb_eRuntimeError,"bitmap load failed(%s)",pathbuffer);
		return Qfalse;
	}

	if(! NIL_P(maskfname)){
		path_conv_to_win(SWIN_API_STR_PTR(maskfname),pathbuffer);
			mask = LoadImage(NULL,pathbuffer,IMAGE_BITMAP,0,0,
			LR_MONOCHROME | LR_DEFAULTSIZE | LR_LOADFROMFILE);
		if(mask==NULL){
			DeleteObject(bitmap);
			rb_raise(rb_eRuntimeError,"mask load failed(%s)",pathbuffer);	
			return Qfalse;
		}
	} else {
		mask = NULL;
	}

	Data_Get_Struct(obj,struct SwinImagelist,si);
	SWIN_IMAGELISTALIVECHECK(si)

	r = ImageList_Add(si->himagelist,bitmap,mask);
	DeleteObject(bitmap);
	if(mask) DeleteObject(mask);

	if(r<0){
		rb_raise(rb_eRuntimeError,"Imagelist_Add failed");
		return Qfalse;
	}
	return INT2NUM(r);
}

static VALUE
swinilist_addicon(VALUE obj, VALUE iconfile){
	struct SwinImagelist* si;
	HICON icon;
	TCHAR pathbuffer[MYMAXPATH*2*sizeof(TCHAR)];
	int r;

	path_conv_to_win(SWIN_API_STR_PTR(iconfile),pathbuffer);
	icon = LoadImage(NULL,pathbuffer,IMAGE_ICON,0,0,
		LR_DEFAULTCOLOR  | LR_DEFAULTSIZE | LR_LOADFROMFILE);
	if(icon==NULL){
		rb_raise(rb_eRuntimeError,"icon load failed(%s)",pathbuffer);
		return Qfalse;
	}

	Data_Get_Struct(obj,struct SwinImagelist,si);
	SWIN_IMAGELISTALIVECHECK(si)

	r = ImageList_AddIcon(si->himagelist,icon); 
	DeleteObject(icon);

	if(r<0){
		rb_raise(rb_eRuntimeError,"Imagelist_AddIcon failed(hicon=%d)",icon);
		return Qfalse;
	}
	return INT2NUM(r);
}

static VALUE
swinilist_getflags(VALUE obj){
	struct SwinImagelist* si;
	Data_Get_Struct(obj,struct SwinImagelist,si);
	SWIN_IMAGELISTALIVECHECK(si)
	return UINT2NUM(si->flags);
}
static VALUE
swinilist_masked_p(VALUE obj){
	struct SwinImagelist* si;
	Data_Get_Struct(obj,struct SwinImagelist,si);
	SWIN_IMAGELISTALIVECHECK(si)
	if(si->flags & ILC_MASK>0){
		return Qtrue;
	}
	return Qfalse;
}


/************************
* Icon
*************************/
VALUE cSwinIcon;

void release_icon(struct SwinIcon* si){
	if(!si->shared) DestroyIcon(si->hicon);
	free(si);
}

static VALUE
swinfac_iconfromfile(VALUE fac,VALUE fname){
	struct SwinIcon* si;
	TCHAR pathbuffer[MYMAXPATH*2*sizeof(TCHAR)];
	VALUE robj;

	robj = Data_Make_Struct(cSwinIcon,struct SwinIcon,0,release_icon,si);
	
	path_conv_to_win(SWIN_API_STR_PTR(fname),pathbuffer);
	si->hicon = LoadImage(NULL,pathbuffer,IMAGE_ICON,0,0,
	    LR_DEFAULTSIZE | LR_LOADFROMFILE);
	si->shared=FALSE;
	if(si->hicon==NULL){
		rb_raise(rb_eRuntimeError,"Icon load failed(%s).",
			StringValuePtr(fname));
		return Qfalse;
	}
	return robj;
}

static VALUE
swinico_hicon(VALUE obj){
	struct SwinIcon* si;
	Data_Get_Struct(obj,struct SwinIcon,si);
	return UINT2NUM((unsigned int)si->hicon);
}

void
Init_swinRsc(){
	cSwinFont = rb_define_class_under(mSwin,"Font",rb_cObject);

	rb_define_method(cSwinFactory,"newfont",swinfont_new,-1);
	rb_define_singleton_method(cSwinFont,"new",swin_avoid_new,-1);
	rb_define_method(cSwinFont,"params",swinfont_array,0);
	rb_define_method(cSwinFont,"fontname",swinfont_fname,0);
	rb_define_method(cSwinFont,"height",swinfont_height,0);
	rb_define_method(cSwinFont,"hfont",swinfont_hfont,0);
	rb_define_method(cSwinFont,"handle",swinfont_hfont,0);
	rb_define_method(cSwinFont,"delete",swinfont_delete,0);
	rb_define_method(cSwinFont,"application",swinaa_application,0);
	eSwinFontNotExistError = rb_define_class_under(mSwin,
        "FontDeletedError",eSwinResourceNotExistError);

	cSwinCursor = rb_define_class_under(mSwin,"Cursor",rb_cObject);
	rb_define_method(cSwinFactory,"newcursor",swincursor_new,4);
	rb_define_method(cSwinFactory,"cursorfromfile",swincursor_load,1);
	rb_define_singleton_method(cSwinCursor,"new",swin_avoid_new,-1);
	rb_define_method(cSwinCursor,"hotspot",swincursor_hotspot,0);
	rb_define_method(cSwinCursor,"hcursor",swincursor_hcursor,0);
	rb_define_method(cSwinCursor,"handle",swincursor_hcursor,0);
	rb_define_method(cSwinCursor,"delete",swincursor_delete,0);
	rb_define_method(cSwinCursor,"systemcursor?",swincursor_sysf,0);
	rb_define_method(cSwinCursor,"application",swinaa_application,0);
	eSwinCursorNotExistError = rb_define_class_under(mSwin,
        "CursorDeletedError",eSwinResourceNotExistError);
	
	rb_define_singleton_method(mSwinApp,"setCursor",swinapp_setcursor,1);
	rb_define_singleton_method(mSwinApp,"getCursor",swinapp_getcursor,0);
	rb_define_singleton_method(mSwinApp,"showCursor",swinapp_showcursor,1);

    mSwinSysCur = rb_define_module_under(mSwinApp,"SysCursors");

	rb_define_module_function(mSwinSysCur,"Arrow",swinapp_getsyscARROW,0);
	rb_define_module_function(mSwinSysCur,"Cross",swinapp_getsyscCROSS,0);
	rb_define_module_function(mSwinSysCur,"IBeam",swinapp_getsyscIBEAM,0);
	rb_define_module_function(mSwinSysCur,"Icon",swinapp_getsyscICON,0);
	rb_define_module_function(mSwinSysCur,"Size",swinapp_getsyscSIZE,0);
	rb_define_module_function(mSwinSysCur,"SizeNESW",swinapp_getsyscSIZENESW,0);
	rb_define_module_function(mSwinSysCur,"SizeNS",swinapp_getsyscSIZENS,0);
	rb_define_module_function(mSwinSysCur,"SizeNWSE",swinapp_getsyscSIZENWSE,0);
	rb_define_module_function(mSwinSysCur,"SizeWE",swinapp_getsyscSIZEWE,0);
	rb_define_module_function(mSwinSysCur,"UpArrow",swinapp_getsyscUPARROW,0);
	rb_define_module_function(mSwinSysCur,"Wait",swinapp_getsyscWAIT,0);

	cSwinImagelist = rb_define_class_under(mSwin,"Imagelist",rb_cObject);
	rb_define_method(cSwinFactory,"newimagelist",swinilist_create,-1);
	rb_define_singleton_method(cSwinImagelist,"new",swin_avoid_new,-1);
	rb_define_method(cSwinImagelist,"deletelist",swinilist_delete,0);
	rb_define_method(cSwinImagelist,"add",swinilist_add,-1);
	rb_define_method(cSwinImagelist,"addmasked",swinilist_addmasked,2);
	rb_define_method(cSwinImagelist,"delete",swinilist_removeimage,1);
	rb_define_method(cSwinImagelist,"clear",swinilist_removeallimage,0);
	rb_define_method(cSwinImagelist,"count",swinilist_countimage,0);
	rb_define_method(cSwinImagelist,"himagelist",swinilist_hilist,0);
	rb_define_method(cSwinImagelist,"handle",swinilist_hilist,0);
	rb_define_method(cSwinImagelist,"application",swinaa_application,0);
	rb_define_method(cSwinImagelist,"geticon",swinilist_geticon,-1);
	rb_define_method(cSwinImagelist,"getimageinfo",swinilist_getimageinfo,1);
	rb_define_method(cSwinImagelist,"bkcolor",swinilist_getbkcolor,0);
	rb_define_method(cSwinImagelist,"bkcolor=",swinilist_setbkcolor,1);
	rb_define_method(cSwinImagelist,"addbitmaps",swinilist_addbmp,2);
	rb_define_method(cSwinImagelist,"addicon",swinilist_addicon,1);
	rb_define_method(cSwinImagelist,"flags",swinilist_getflags,0);
	rb_define_method(cSwinImagelist,"masked?",swinilist_masked_p,0);

	eSwinImagelistNotExistError = rb_define_class_under(mSwin,
        "ImagelistDeletedError",eSwinResourceNotExistError);

	cSwinIcon = rb_define_class_under(mSwin,"Icon",rb_cObject);
	rb_define_method(cSwinFactory,"iconfromfile",swinfac_iconfromfile,1);
	rb_define_singleton_method(cSwinIcon,"new",swin_avoid_new,-1);
	rb_define_method(cSwinIcon,"hicon",swinico_hicon,0);
	rb_define_method(cSwinIcon,"handle",swinico_hicon,0);

/*	to_be_deleted(); */
}
