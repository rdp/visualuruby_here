/*
###################################
#
# swinbmp.c
# Programmed by nyasu <nyasu@osk.3web.ne.jp>
# Copyright 1999-2005  Nishikawa,Yasuhiro
#
# More information at http://vruby.sourceforge.net/index.html
#
###################################
*/


#include "swin.h"

#define NOTTHREE ((long)~3) 


VALUE cSwinBitmap;

void release_bmp(struct SwinBitmap *sb){
	if(sb->info) free(sb->info);
	if(sb->bmp) free(sb->bmp);
	free(sb);
}

static VALUE
swinbmp_initialize(int argc, VALUE* argv,VALUE self){
	return self;
}

static VALUE
swinbmp_loadbitmap(VALUE klass,VALUE fname){
	VALUE robj;
	struct SwinBitmap* sb;
	BITMAPFILEHEADER bmphead;
    HANDLE hFile;
    DWORD sz;
	TCHAR pathbuf[MYMAXPATH*2*sizeof(TCHAR)];
	long size;
	
	if(TYPE(fname)!=T_STRING){
		rb_raise(rb_eTypeError,"filename illegal.");
		return Qfalse;
	}
	
    robj = Data_Make_Struct(klass, struct SwinBitmap, 0, release_bmp, sb);
	sb->bmp=NULL;
	sb->bmpsize=0;
	
	path_conv_to_win(SWIN_API_STR_PTR(fname),pathbuf);
	hFile = CreateFile(pathbuf, GENERIC_READ, 0, NULL, 
                OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if(hFile==INVALID_HANDLE_VALUE){
		rb_raise(rb_eIOError,"%s: file not found.",StringValuePtr(fname));
		return Qfalse;
    }
    ReadFile(hFile,&bmphead,sizeof(BITMAPFILEHEADER),&sz,NULL);

	if(bmphead.bfType!=('B'+('M'<<8))){  /* Thanks,Nakada-san */
		rb_raise(rb_eRuntimeError,"%s: file not bmpfile.",StringValuePtr(fname));
		return Qfalse;
	}

	size=bmphead.bfOffBits-sizeof(BITMAPFILEHEADER);
	MYMALLOC(sb->info,size,Qfalse);
	sb->infosize=size;

    ReadFile(hFile, sb->info, size, &sz, NULL);

	sb->bmpsize=bmphead.bfSize-bmphead.bfOffBits/*-sizeof(BITMAPFILEHEADER)*/;
	MYMALLOC(sb->bmp,sb->bmpsize,Qfalse);
    ReadFile(hFile, sb->bmp, sb->bmpsize, &sz, NULL);

	CloseHandle(hFile);
	rb_obj_call_init(robj,1,&fname);
	return robj;
}

static VALUE
swinbmp__load(VALUE klass,VALUE lstr){
	VALUE robj;
	struct SwinBitmap* sb;
	BITMAPFILEHEADER bmphead;
	char* loaddata;
	long size;
	
    robj = Data_Make_Struct(klass, struct SwinBitmap, 0, release_bmp, sb);
	sb->bmp=NULL;
	sb->bmpsize=0;
	
	loaddata = StringValuePtr(lstr);
	
	memcpy(&bmphead,loaddata,sizeof(BITMAPFILEHEADER));
	loaddata+=sizeof(BITMAPFILEHEADER);
	
	if(bmphead.bfType!=('B'+('M'<<8))){
		rb_raise(rb_eRuntimeError,"load data illegal.");
		return Qfalse;
	}

	size=bmphead.bfOffBits-sizeof(BITMAPFILEHEADER);
	MYMALLOC(sb->info,size,Qfalse);
	sb->infosize=size;

	memcpy(sb->info,loaddata,size);
	loaddata+=size;

	sb->bmpsize=bmphead.bfSize-bmphead.bfOffBits/*-sizeof(BITMAPFILEHEADER)*/;
	MYMALLOC(sb->bmp,sb->bmpsize,Qfalse);

	memcpy(sb->bmp,loaddata,sb->bmpsize);
	return robj;
}

static VALUE
swinbmp_savebitmap(VALUE obj,VALUE fname){
	struct SwinBitmap* sb;
	BITMAPFILEHEADER bmphead;
	TCHAR pathbuf[MYMAXPATH*2*sizeof(TCHAR)];
    HANDLE hFile;
    DWORD sz;

	if(TYPE(fname)!=T_STRING){
		rb_raise(rb_eTypeError,"Filename illegal.");
		return Qfalse;
	}
	
	Data_Get_Struct(obj, struct SwinBitmap, sb);

	path_conv_to_win(SWIN_API_STR_PTR(fname),pathbuf);
    hFile = CreateFile(pathbuf, GENERIC_WRITE, 0, NULL, 
                CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	if(hFile==INVALID_HANDLE_VALUE){
		rb_raise(rb_eIOError,"%s: Cannot create file.",StringValuePtr(fname));
		return Qfalse;
    }

	bmphead.bfType='B'+('M'<<8);
	bmphead.bfSize=sizeof(BITMAPFILEHEADER)+sb->infosize+sb->bmpsize;
	bmphead.bfReserved1=bmphead.bfReserved2=0;
	bmphead.bfOffBits=sizeof(BITMAPFILEHEADER)+sb->infosize;

    WriteFile(hFile,&bmphead,sizeof(BITMAPFILEHEADER),&sz,NULL);
    WriteFile(hFile,sb->info,sb->infosize,&sz,NULL);
    WriteFile(hFile,sb->bmp,sb->bmpsize,&sz,NULL);
    CloseHandle(hFile);
	return obj;
}

static VALUE
swinbmp__dump(VALUE obj,VALUE limit){
	struct SwinBitmap* sb;
	VALUE robj;
	BITMAPFILEHEADER bmphead;

	if(limit<1) ;  /* ? */
	Data_Get_Struct(obj, struct SwinBitmap, sb);
	
	bmphead.bfType='B'+('M'<<8);
	bmphead.bfSize=sizeof(BITMAPFILEHEADER)+sb->infosize+sb->bmpsize;
	bmphead.bfReserved1=bmphead.bfReserved2=0;
	bmphead.bfOffBits=sizeof(BITMAPFILEHEADER)+sb->infosize;

	robj = rb_str_new((char*)&bmphead,sizeof(BITMAPFILEHEADER));
	rb_str_cat(robj, (char*)sb->info,sb->infosize);
	rb_str_cat(robj, (char*)sb->bmp, sb->bmpsize );
	
	return robj;
}


static VALUE
swinbmp_createbitmap(int argc, VALUE* argv, VALUE klass){
	struct SwinBitmap* sb;
	VALUE robj;
	VALUE info,bmp;
	long len;

	if(argc<2){
		rb_raise(rb_eArgError,"infostr and bmpstr is required as arguments.");
		return Qfalse;
	}
	info = StringValue(argv[0]); bmp = StringValue(argv[1]);

	if(TYPE(info)!=T_STRING || TYPE(bmp)!=T_STRING){
		rb_raise(rb_eTypeError,"argument should be String.");
		return Qfalse;
	}

	
    robj = Data_Make_Struct(klass, struct SwinBitmap, 0, release_bmp, sb);
	
	len=RSTRING_LEN(info);
	MYMALLOC(sb->info,len,Qfalse);
	memcpy(sb->info,RSTRING_PTR(info),len);
	sb->infosize=len;

	len=RSTRING_LEN(bmp);
	MYMALLOC(sb->bmp,len,Qfalse);
	memcpy(sb->bmp,RSTRING_PTR(bmp),len);
	sb->bmpsize=len;
	rb_obj_call_init(robj,argc,argv);
	return robj;
}

VALUE*
swinbmp_new(VALUE argv[2],unsigned width, unsigned height){
	VALUE info;
	VALUE bmp;
	VALUE vtmp;
	VALUE c;

	BITMAPINFOHEADER* binfo;
	unsigned wlen;
	BYTE* buffer;
	unsigned bmplen;

	wlen = ((width*3+3)&NOTTHREE) ;

	info = rb_str_new(NULL, sizeof(BITMAPINFOHEADER));
	binfo = (BITMAPINFOHEADER*) RSTRING_PTR(info);

	binfo->biSize=sizeof(BITMAPINFOHEADER);
	binfo->biHeight=height;
	binfo->biWidth = width;
	binfo->biPlanes=1;
	binfo->biBitCount=24;
	binfo->biCompression=0;
	binfo->biSizeImage= wlen * height;
	binfo->biXPelsPerMeter=binfo->biYPelsPerMeter=144;  /* tekitou */
	binfo->biClrImportant=0;
	binfo->biClrUsed = 0;

	bmplen = wlen * binfo->biHeight;
	bmp = rb_str_new(NULL, bmplen); 

	buffer = (BYTE*) RSTRING_PTR(bmp);
	memset(buffer,0,bmplen);
	argv[0] = info;
	argv[1] = bmp;
	return argv;
}

static VALUE
swinbmp_newbitmap(VALUE klass,VALUE vwidth, VALUE vheight){
	VALUE argv[2];
	
	swinbmp_new(argv,NUM2UINT(vwidth),NUM2UINT(vheight));
	return swinbmp_createbitmap(2,argv,klass);
}

VALUE
swinbmp_4to24(struct SwinBitmap* sb){
	VALUE newbmp[2];
	unsigned width,height,x,y,l4width,l24width,l4ptr,l24ptr;
	int colors=0,cused;
	RGBQUAD* pal;
	unsigned int c4;
	BYTE* bmparray;
	
	colors = 1 << (sb->info->bmiHeader.biBitCount-1);
	cused = sb->info->bmiHeader.biClrUsed;
	width  = sb->info->bmiHeader.biWidth;
	height = sb->info->bmiHeader.biHeight;
	l4width = ((width+1)/2+3)&NOTTHREE;
	l24width = (width*3+3)&NOTTHREE;
	pal = (RGBQUAD*) ((BYTE*)(sb->info) + sizeof(BITMAPINFOHEADER));

	swinbmp_new(newbmp,width,height);
	bmparray = (BYTE*) RSTRING_PTR(newbmp[1]);
	for(y=0;y<height;y++){
		l4ptr  = l4width*y;
		l24ptr = l24width*y;
		for(x=0;x<width;x++){
			c4 = (unsigned char)sb->bmp[l4ptr+x/2];
			if((x&1)==0){
				c4 = (c4>>4) & 0xf;
			} else {
				c4 = c4 & 0xf;
			}

			if(c4>cused){
				rb_raise(rb_eRuntimeError,"no such a palette color");
				return Qfalse;
			}
			bmparray[l24ptr + x*3  ] = pal[c4].rgbBlue;
			bmparray[l24ptr + x*3+1] = pal[c4].rgbGreen;
			bmparray[l24ptr + x*3+2] = pal[c4].rgbRed;
		}
	}

	return swinbmp_createbitmap(2,newbmp,cSwinBitmap);
}


VALUE
swinbmp_8to24(struct SwinBitmap* sb){
	VALUE newbmp[2];
	unsigned width,height,x,y,l8width,l24width,l8ptr,l24ptr;
	int colors=0,cused;
	RGBQUAD* pal;
	unsigned int c8;
	BYTE* bmparray;
	
	colors = 1 << (sb->info->bmiHeader.biBitCount-1);
	cused = sb->info->bmiHeader.biClrUsed;
	width  = sb->info->bmiHeader.biWidth;
	height = sb->info->bmiHeader.biHeight;
	l8width = (width+3)&NOTTHREE;
	l24width = (width*3+3)&NOTTHREE;
	pal = (RGBQUAD*) ((BYTE*)(sb->info) + sizeof(BITMAPINFOHEADER));

	swinbmp_new(newbmp,width,height);
	bmparray = (BYTE*) RSTRING_PTR(newbmp[1]);
	for(y=0;y<height;y++){
		l8ptr  = l8width*y;
		l24ptr = l24width*y;
		for(x=0;x<width;x++){
			c8 = sb->bmp[l8ptr+x];
			if(c8>cused){
				rb_raise(rb_eRuntimeError,"no such a palette color");
				return Qfalse;
			}
			bmparray[l24ptr + x*3  ] = pal[c8].rgbBlue;
			bmparray[l24ptr + x*3+1] = pal[c8].rgbGreen;
			bmparray[l24ptr + x*3+2] = pal[c8].rgbRed;
		}
	}

	return swinbmp_createbitmap(2,newbmp,cSwinBitmap);
}


static VALUE
swinbmp_tofullcolor(VALUE obj){
	struct SwinBitmap* sb;

	Data_Get_Struct(obj, struct SwinBitmap, sb);

	if(sb->info->bmiHeader.biCompression!=0){
		rb_raise(rb_eRuntimeError,"Compressed bmp is not supported");
		return Qfalse;
	}
	switch(sb->info->bmiHeader.biBitCount){
		case 4:
			return swinbmp_4to24(sb);
			break;
		case 8:
			return swinbmp_8to24(sb);
			break;
		case 24:
			return obj;
			break;
		case 2:
		case 16:
		default:
			rb_raise(rb_eRuntimeError,"This color format is not supported");
	}
	return Qfalse;
}

unsigned*
swinbmp_getcolor4(struct SwinBitmap* sb,unsigned x,unsigned y,unsigned rgb[3]){
	unsigned l4width,width,height,ptr;
	unsigned c4;
	RGBQUAD* pal;

	width  = sb->info->bmiHeader.biWidth;
	height = sb->info->bmiHeader.biHeight;
	l4width = ((width+1)/2+3)&NOTTHREE;
	pal = (RGBQUAD*) ((BYTE*)(sb->info) + sizeof(BITMAPINFOHEADER));

	if(x>=width || y>=height){
		rb_raise(rb_eRuntimeError,"Illegal point");
	}
	ptr = l4width*y + x/2;
	c4 = sb->bmp[ptr];
	if((x&1)==0){
		c4 = (c4>>4) & 0xf;
	} else {
		c4 = c4 & 0xf;
	}

	rgb[0]=pal[c4].rgbRed;
	rgb[1]=pal[c4].rgbGreen;
	rgb[2]=pal[c4].rgbBlue;

	return rgb;
}
unsigned*
swinbmp_getcolor8(struct SwinBitmap* sb,unsigned x,unsigned y,unsigned rgb[3]){
	unsigned l8width,width,height,ptr;
	unsigned c8;
	RGBQUAD* pal;

	width  = sb->info->bmiHeader.biWidth;
	height = sb->info->bmiHeader.biHeight;
	l8width = (width+3)&NOTTHREE;
	pal = (RGBQUAD*) ((BYTE*)(sb->info) + sizeof(BITMAPINFOHEADER));

	if(x>=width || y>=height){
		rb_raise(rb_eRuntimeError,"Illegal point");
	}
	ptr = l8width*y + x;
	c8 = sb->bmp[ptr];

	rgb[0]=pal[c8].rgbRed;
	rgb[1]=pal[c8].rgbGreen;
	rgb[2]=pal[c8].rgbBlue;

	return rgb;
}
unsigned*
swinbmp_getcolor24(struct SwinBitmap* sb,unsigned x,unsigned y,unsigned rgb[3]){
	unsigned l24width,width,height,ptr;

	width  = sb->info->bmiHeader.biWidth;
	height = sb->info->bmiHeader.biHeight;
	l24width = (width*3+3)&NOTTHREE;

	if(x>=width || y>=height){
		rb_raise(rb_eRuntimeError,"Illegal point");
	}
	ptr = l24width*y + x*3;

	rgb[0] = sb->bmp[ptr+2];
	rgb[1] = sb->bmp[ptr+1];
	rgb[2] = sb->bmp[ptr+0];
	
	return rgb;
}

static VALUE
swinbmp_getcolor(VALUE obj,VALUE vx,VALUE vy){
	VALUE robj;
	struct SwinBitmap* sb;
	unsigned x,y;
	unsigned rgb[3];

	x = NUM2UINT(vx);  y = NUM2UINT(vy);
	Data_Get_Struct(obj, struct SwinBitmap, sb);

	if(sb->info->bmiHeader.biCompression!=0){
		rb_raise(rb_eRuntimeError,"Compressed bmp is not supported");
		return Qfalse;
	}
	switch(sb->info->bmiHeader.biBitCount){
		case 4:
			swinbmp_getcolor4(sb,x,y,rgb);
			break;
		case 8:
			swinbmp_getcolor8(sb,x,y,rgb);
			break;
		case 24:
			swinbmp_getcolor24(sb,x,y,rgb);
			break;
		case 2:
		case 16:
		default:
			rb_raise(rb_eRuntimeError,"This color format is not supported");
			return Qfalse;
	}

	robj = rb_ary_new2(3);
	rb_ary_push(robj,INT2NUM(rgb[0]));
	rb_ary_push(robj,INT2NUM(rgb[1]));
	rb_ary_push(robj,INT2NUM(rgb[2]));
	return robj;
}

VALUE
swinbmp_setcolor24(struct SwinBitmap* sb,unsigned x,unsigned y,VALUE rgb){
	unsigned l24width,width,height,ptr;
	unsigned r,g,b;

	width  = sb->info->bmiHeader.biWidth;
	height = sb->info->bmiHeader.biHeight;
	l24width = (width*3+3)&NOTTHREE;

	if(x>=width || y>=height){
		rb_raise(rb_eRuntimeError,"Illegal point");
		return Qfalse;
	}
	ptr = l24width*y + x*3;

	r = NUM2UINT( rb_ary_entry(rgb,0) );
	g = NUM2UINT( rb_ary_entry(rgb,1) );
	b = NUM2UINT( rb_ary_entry(rgb,2) );
	
	sb->bmp[ptr+2] = r;
	sb->bmp[ptr+1] = g;
	sb->bmp[ptr+0] = b;
	
	return rgb;
}

static VALUE
swinbmp_setcolor(VALUE obj,VALUE vx,VALUE vy,VALUE rgb){
	struct SwinBitmap* sb;
	unsigned x,y;

	if(TYPE(rgb)!=T_ARRAY){
		rb_raise(rb_eArgError,"color must be Array");
		return Qfalse;
	}
	if(RARRAY_LEN(rgb)<3){
		rb_raise(rb_eArgError,"Array have less than 3 elements");
		return Qfalse;
	}

	x = NUM2UINT(vx);  y = NUM2UINT(vy);
	Data_Get_Struct(obj, struct SwinBitmap, sb);

	if(sb->info->bmiHeader.biCompression!=0){
		rb_raise(rb_eRuntimeError,"Compressed bmp is not supported");
		return Qfalse;
	}
	switch(sb->info->bmiHeader.biBitCount){
		case 24:
			return swinbmp_setcolor24(sb,x,y,rgb);
			break;
		case 2:
		case 4:
		case 8:
		case 16:
		default:
			rb_raise(rb_eRuntimeError,"This color format is not supported");
	}
	return Qfalse;
}



static VALUE
swinbmp_getcolorbit(VALUE obj){
	struct SwinBitmap* sb;
	Data_Get_Struct(obj, struct SwinBitmap, sb);
	return INT2NUM(sb->info->bmiHeader.biBitCount);
}

static VALUE
swinbmp_getwidth(VALUE obj){
	struct SwinBitmap* sb;
	Data_Get_Struct(obj, struct SwinBitmap, sb);
	return INT2NUM(sb->info->bmiHeader.biWidth);
}
static VALUE
swinbmp_getheight(VALUE obj){
	struct SwinBitmap* sb;
	Data_Get_Struct(obj, struct SwinBitmap, sb);
	return INT2NUM(sb->info->bmiHeader.biHeight);
}
static VALUE
swinbmp_getinfo(VALUE obj){
	struct SwinBitmap* sb;
	Data_Get_Struct(obj, struct SwinBitmap, sb);
	return rb_str_new((LPSTR)sb->info,sb->infosize);
}
static VALUE
swinbmp_getbmp(VALUE obj){
	struct SwinBitmap* sb;
	Data_Get_Struct(obj, struct SwinBitmap, sb);
	return rb_str_new(sb->bmp,sb->bmpsize);
}

static VALUE
swinbmp_getInfoAndBmp(VALUE obj){
	struct SwinBitmap* sb;
	Data_Get_Struct(obj, struct SwinBitmap, sb);
	return rb_ary_new3(2,rb_str_new((LPSTR)sb->info,sb->infosize),
                         rb_str_new(sb->bmp,sb->bmpsize) );
}

void Init_swinBitmap(){
	cSwinBitmap=rb_define_class_under(mSwin,"Bitmap",rb_cObject);

	rb_define_singleton_method(cSwinBitmap,"loadFile",swinbmp_loadbitmap,1);
	rb_define_singleton_method(cSwinBitmap,"createBitmap",swinbmp_createbitmap,-1);
	rb_define_singleton_method(cSwinBitmap,"newBitmap",swinbmp_newbitmap,2);
	rb_define_singleton_method(cSwinBitmap,"new",swinbmp_newbitmap,2);

	rb_define_method(cSwinBitmap,"initialize",swinbmp_initialize,-1);
	rb_define_method(cSwinBitmap,"bitcount",swinbmp_getcolorbit,0);
	rb_define_method(cSwinBitmap,"width",swinbmp_getwidth,0);
	rb_define_method(cSwinBitmap,"height",swinbmp_getheight,0);
	rb_define_method(cSwinBitmap,"infoStr",swinbmp_getinfo,0);
	rb_define_method(cSwinBitmap,"bmpStr",swinbmp_getbmp,0);
	rb_define_method(cSwinBitmap,"infoandbmp",swinbmp_getInfoAndBmp,0);

	rb_define_method(cSwinBitmap,"saveFile",swinbmp_savebitmap,1);

	rb_define_singleton_method(cSwinBitmap,"_load",swinbmp__load,1);
	rb_define_method(cSwinBitmap,"_dump",swinbmp__dump,1);

	rb_define_method(cSwinBitmap,"to_fullcolor",swinbmp_tofullcolor,0);
	rb_define_method(cSwinBitmap,"[]",swinbmp_getcolor,2);
	rb_define_method(cSwinBitmap,"[]=",swinbmp_setcolor,3);
}

