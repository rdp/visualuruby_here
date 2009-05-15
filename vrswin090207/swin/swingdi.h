/*
###################################
#
# swingdi.h
# Programmed by nyasu <nyasu@osk.3web.ne.jp>
# Copyright 2001-2005  Nishikawa,Yasuhiro
#
# More information at http://vruby.sourceforge.net/index.html
#
###################################
*/




/* The art by my stupid brain. */

#define DefineObjGDIARG0Func(c,x,startfunc) \
static VALUE \
c##x(VALUE obj){ \
  startfunc \
  return swingdi_##x(&(sw->gs)); \
}

#define DefineObjGDIARG1Func(c,x,startfunc) \
static VALUE \
c##x(VALUE obj,VALUE arg1){ \
  startfunc \
  return swingdi_##x(&(sw->gs),arg1); \
}

#define DefineObjGDIARG2Func(c,x,startfunc) \
static VALUE \
c##x(VALUE obj,VALUE arg1,VALUE arg2){ \
  startfunc \
  return swingdi_##x(&(sw->gs),arg1,arg2); \
}


#define DefineObjGDIARG3Func(c,x,startfunc) \
static VALUE \
c##x(VALUE obj,VALUE arg1,VALUE arg2,VALUE arg3){ \
  startfunc \
  return swingdi_##x(&(sw->gs),arg1,arg2,arg3); \
}


#define DefineObjGDIARG4Func(c,x,startfunc) \
static VALUE \
c##x(VALUE obj,VALUE arg1,VALUE arg2,VALUE arg3, VALUE arg4){ \
  startfunc \
  return swingdi_##x(&(sw->gs),arg1,arg2,arg3,arg4); \
}

#define DefineObjGDIARG5Func(c,x,startfunc) \
static VALUE \
c##x(VALUE obj,VALUE arg1,VALUE arg2,VALUE arg3, VALUE arg4,VALUE arg5){ \
  startfunc \
  return swingdi_##x(&(sw->gs),arg1,arg2,arg3,arg4,arg5); \
}


#define DefineObjGDIARG6Func(c,x,startfunc) \
static VALUE \
c##x(VALUE obj,VALUE arg1,VALUE arg2,VALUE arg3, VALUE arg4,VALUE arg5,VALUE arg6){ \
  startfunc \
  return swingdi_##x(&(sw->gs),arg1,arg2,arg3,arg4,arg5,arg6); \
}


#define DefineObjGDIARG7Func(c,x,startfunc) \
static VALUE \
c##x(VALUE obj,VALUE arg1,VALUE arg2,VALUE arg3, VALUE arg4,VALUE arg5,VALUE arg6,VALUE arg7){ \
  startfunc \
  return swingdi_##x(&(sw->gs),arg1,arg2,arg3,arg4,arg5,arg6,arg7); \
}


#define DefineObjGDIARG8Func(c,x,startfunc) \
static VALUE \
c##x(VALUE obj,VALUE arg1,VALUE arg2,VALUE arg3, VALUE arg4,VALUE arg5,VALUE arg6,VALUE arg7,VALUE arg8){ \
  startfunc \
  return swingdi_##x(&(sw->gs),arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8); \
}

#define DefineObjGDIARGVFunc(c,x,startfunc) \
static VALUE \
c##x(int argc,VALUE* argv,VALUE obj){ \
	startfunc \
	return swingdi_##x(argc,argv,&(sw->gs)); \
}


/*

    GDI Functions definition

*/

#define DEFINE_RUBY_GDIFUNCS(obj,prename) \
	rb_define_method(obj,"setPen",prename##setPen,-1); \
	rb_define_method(obj,"setBrush",prename##setBrush,-1); \
	rb_define_method(obj,"fillRect",prename##Rectangle,4); \
	rb_define_method(obj,"fillRoundRect",prename##RoundRectangle,6); \
	rb_define_method(obj,"fillEllipse",prename##Ellipse,4); \
	rb_define_method(obj,"fillArc",prename##Arc,8); \
\
	rb_define_method(obj,"grMoveTo",prename##MoveTo,2); \
	rb_define_method(obj,"grLineTo",prename##LineTo,2); \
	rb_define_method(obj,"drawLine",prename##Line,4); \
	rb_define_method(obj,"drawChord",prename##Chord,8); \
	rb_define_method(obj,"drawText",prename##drawText,-1); \
	rb_define_method(obj,"floodfill",prename##FloodFill,-1); \
\
	rb_define_method(obj,"drawBitmap",prename##drawDIBitmap,-1); \
	rb_define_method(obj,"bitblt",    prename##drawDDBitmap,-1); \
\
	rb_define_method(obj,"drawBkColor=",prename##setbkcolor,1); \
	rb_define_method(obj,"drawBkColor",prename##getbkcolor,0); \
	rb_define_method(obj,"textColor=",prename##settextcolor,1); \
	rb_define_method(obj,"textColor",prename##gettextcolor,0); \
\
	rb_define_method(obj,"getPixel",prename##getPixel,2); \
	rb_define_method(obj,"setPixel",prename##setPixel,3); \
\
	rb_define_method(obj,"setFont",prename##setFont,1); \
\
	rb_define_method(obj,"opaque=",prename##setbkmode,1); \
	rb_define_method(obj,"opaque?",prename##getbkmode,0); \
\
	rb_define_method(obj,"hdc",prename##gethdc,0); \
\
	rb_define_method(obj,"devicecap",prename##devicecap,1); \
	rb_define_method(obj,"size",prename##size,0); \
	rb_define_method(obj,"physicalsize",prename##sizebymm,0); \
	rb_define_method(obj,"dpi",prename##getdpi,0); \
	rb_define_method(obj,"textExtent",prename##gettextextent,1); \
\
	rb_define_method(obj,"stretch",prename##StretchDIBlt,-1); /* old name */\
	rb_define_method(obj,"stretchBitmap",prename##StretchDIBlt,-1); \
	rb_define_method(obj,"stretchmode",prename##getstretchmode,0); \
	rb_define_method(obj,"stretchmode=",prename##setstretchmode,1); \
	rb_define_method(obj,"application",swinaa_application,0); \
/* */


#define DEFINE_GDI_DEVICECAPS_MACRO(prename,prefunc)\
\
DefineObjGDIARG1Func(prename,devicecap,prefunc)\
DefineObjGDIARG0Func(prename,size,prefunc)\
DefineObjGDIARG0Func(prename,sizebymm,prefunc)\
DefineObjGDIARG0Func(prename,getdpi,prefunc)\
DefineObjGDIARG1Func(prename,gettextextent,prefunc)\
/* */

#define DEFINE_GDI_FUNCTIONS_MACRO(prename,prefunc) \
\
DefineObjGDIARGVFunc(prename,setPen,prefunc)\
DefineObjGDIARGVFunc(prename,setBrush,prefunc)\
DefineObjGDIARG4Func(prename,Rectangle,prefunc)\
DefineObjGDIARG6Func(prename,RoundRectangle,prefunc)\
DefineObjGDIARG8Func(prename,Arc,prefunc)\
DefineObjGDIARG8Func(prename,Chord,prefunc)\
DefineObjGDIARG4Func(prename,Ellipse,prefunc)\
DefineObjGDIARG4Func(prename,Line,prefunc)\
DefineObjGDIARG2Func(prename,MoveTo,prefunc)\
DefineObjGDIARG2Func(prename,LineTo,prefunc)\
DefineObjGDIARGVFunc(prename,drawText,prefunc)\
DefineObjGDIARGVFunc(prename,FloodFill,prefunc)\
DefineObjGDIARG1Func(prename,setbkcolor,prefunc)\
DefineObjGDIARG0Func(prename,getbkcolor,prefunc)\
\
DefineObjGDIARG1Func(prename,settextcolor,prefunc)\
DefineObjGDIARG0Func(prename,gettextcolor,prefunc)\
\
DefineObjGDIARGVFunc(prename,drawDIBitmap,prefunc)\
DefineObjGDIARGVFunc(prename,drawDDBitmap,prefunc)\
\
DefineObjGDIARG2Func(prename,getPixel,prefunc)\
DefineObjGDIARG3Func(prename,setPixel,prefunc)\
\
DefineObjGDIARG1Func(prename,setFont,prefunc)\
\
DefineObjGDIARG0Func(prename,gethdc,prefunc)\
\
DefineObjGDIARG1Func(prename,setbkmode,prefunc)\
DefineObjGDIARG0Func(prename,getbkmode,prefunc)\
\
DefineObjGDIARGVFunc(prename,StretchDIBlt,prefunc)\
DefineObjGDIARG0Func(prename,getstretchmode,prefunc)\
\
DefineObjGDIARG1Func(prename,setstretchmode,prefunc)  \
/* */

#define DEFINE_GDI_ALL_FUNCTIONS_MACRO(prename,prefunc) \
\
    DEFINE_GDI_FUNCTIONS_MACRO(prename,prefunc)\
    DEFINE_GDI_DEVICECAPS_MACRO(prename,prefunc)\
/* */


