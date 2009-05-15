/*
###################################
#
# swin.c
# Programmed by nyasu <nyasu@osk.3web.ne.jp>
# Copyright 1999-2005  Nishikawa,Yasuhiro
#
# More information at http://vruby.sourceforge.net/index.html
#
###################################
*/


#include <stdarg.h>
#include "swin.h"
#include "rubysig.h"


VALUE mSwin;
VALUE cSwin;
VALUE cSwinMsg;
VALUE cSwinFactory;
VALUE SwinFactoryInstance=(VALUE)NULL;

VALUE eSwinResourceNotExistError;
VALUE eSwinWindowNotExistError;
VALUE eSwinInterrupt;
VALUE mSwinApp;

#ifdef SWIN_FOR_RUBY19_M17N
VALUE apiEncoding, outputEncoding;
int apiEncodingIndex;
ID id_encode;
#endif

char* onebytezero = "\0";

HINSTANCE hInstance;
LArray* swlist;

LArray* keyacclist;

HWND current_dialog=NULL;

/* for Application.thread_critical */
BOOL threadcritical;

int swin_handling_ctrlc=FALSE;     /* TRUE if swin should handle ctrlc */

/* prototypes */
void shutdown_win(struct Swin *sw);




/********************************************************
*
*     Utils
*
*/

void path_conv_to_win_wchar(wchar_t *src, wchar_t *dst){
    static wchar_t* cygdrive=L"//cygdrive/";

    if(wcsncmp(cygdrive,src,sizeof(cygdrive))==0) {
        src+=lstrlenW(cygdrive);
        *dst++= *src++;
        *dst++=':';
    }
    
    while(*src!=0){
        if(*src=='/') {
            *dst++ = '\\';
            *src++;
        } else {
            *dst++ = *src++;
        }
    }
    *dst++ = 0;
}

void path_conv_to_win_ansi(char* src, char* dst){
#ifdef NT 
	int step;
	char *p;

	if(src[0]=='/' && src[1]=='/' && (src[3]=='/' || src[3]=='\0')){
        //parhaps no more needs now
		*dst++ = src[2];
		*dst++ = ':';
		src+=3;
	}

	while(*src!='\0'){
		step =(int) (CharNextA(src)-src);
		if(step==1 && *src=='/'){
			src++; *dst++ = '\\';
			step--;
		} 
		while(step>0){
			*dst++ = *src++;   /* natsukasii */
			step--;
		}
	}
	*dst = '\0';
#else
	cygwin32_conv_to_win32_path(src,dst);
#endif
}


void get_hInstance(){
	hInstance= (HINSTANCE)GetModuleHandle(NULL); /* Great Thanks, Yoshida-san */
}



BOOL RegisterWndClass(TCHAR* classname, HICON hIcon,UINT addstyle, HBRUSH hBackGnd, HCURSOR hCursor)
{
    WNDCLASSEX wc;
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW | addstyle;
    wc.lpfnWndProc = WndProc;    
    wc.cbClsExtra = 0;
    wc.cbWndExtra = DWL_USER;
    wc.hInstance = hInstance;
    wc.hIcon = hIcon; /* LoadIcon(NULL,IDI_APPLICATION); */
    wc.hCursor = hCursor;  /* LoadCursor(NULL, IDC_ARROW); */
    wc.hbrBackground = hBackGnd;
    wc.lpszMenuName = NULL;   
    wc.lpszClassName = classname;
    wc.hIconSm = 0; 
    return (RegisterClassEx(&wc));
}


static VALUE
app_arg2struct(int argc, VALUE* argv,VALUE someobject){
	char* format;
	char buffer[1024];   /* I believe this is enough. */
	long pointer;
	int i;
	
	if(argc<2) return Qnil;

	format=(char*) SWIN_ASCII_STR_PTR(argv[0]);
	pointer=(long)buffer;
	
	for(i=1;i<argc;i++){
		if(((int)pointer)-((int)buffer)>=sizeof(buffer)){
			rb_raise(rb_eRuntimeError,"too long struct");
			return Qfalse;
		}
		switch(format[i-1]){
		case 'S':
		case 'P':
		  *(char**)pointer= StringValuePtr(argv[i]);
		  pointer+=sizeof(char*);
		  break;
		case 'L':
		  *(long*)pointer= NUM2UINT(argv[i]);
		  pointer+=sizeof(long);
		  break;
		case 'I':
		case 'U':
		  *(unsigned*)pointer= NUM2UINT(argv[i]);
		  pointer+=sizeof(unsigned int);
		  break;
		case 'C':
		  *(char*)pointer=(long)NUM2UINT(argv[i]);
		  pointer+=sizeof(char);
		  break;
		default:
		  break;
		}
	}
	return rb_str_new(buffer,pointer-(long)buffer);
}




VALUE
struct2arraycore(VALUE point,char* format){
	VALUE robj;
	VALUE item;

	robj=rb_ary_new();
	
	while(*format){
		switch(*format){
		case 'S':
		  item= rb_str_new2( *(char**)point );
		  point += sizeof(char*);
		  break;
		case 'P':
		case 'L':
		  item= UINT2NUM( *(long*)point );
		  point += sizeof(long);
		  break;
		case 'I':
		  item= INT2NUM( *(int*)point );
		  point += sizeof(int);
		  break;
		case 'U':
		  item= UINT2NUM( *(unsigned int*)point );
		  point += sizeof(unsigned int);
		  break;
		case 'C':
		  item= INT2NUM( *(char*)point );
		  point += sizeof(char);
		  break;
		default:
		  item=Qnil;
		  break;
		}
		
		rb_ary_push(robj,item);
		format++;
	}
	return robj;
}

static VALUE
app_unpack(VALUE someobject,VALUE str,VALUE fmt){
	return struct2arraycore((VALUE)(StringValuePtr(str)),SWIN_ASCII_STR_PTR(fmt));
}

static VALUE
app_struct2array(VALUE someobject,VALUE vptr,VALUE fmt){
	VALUE point;
	char* format;
	
	point= (long)NUM2INT(vptr);    
	format= SWIN_ASCII_STR_PTR(fmt);
	
	return struct2arraycore(point,format);
}


static VALUE
app_pointer2string(int argc, VALUE* argv,VALUE someobject){
	char* point;
	VALUE robj;
    
	if(argc<1) return Qnil;

	point= (char*)NUM2UINT(argv[0]);
    if(argc>1) { // length specified, assuming binary
        robj = rb_str_new(point,NUM2INT(argv[1]));
    } else {
      robj = SWIN_OUTAPI_STR_NEW2((TCHAR*)point);
    }
	return robj;
}

static VALUE
app_pokeMemory(VALUE mod, VALUE addr, VALUE value, VALUE width){
	int w = NUM2INT(width);
	long val = NUM2LONG(value);
	void* ptr = (void*)NUM2LONG(addr);

	switch(w){
	case 1:
		*(BYTE*)ptr = (BYTE)val;
		break;
	case 2:
		*(WORD*)ptr = (WORD)val;
		break;
	case 4:
		*(DWORD*)ptr = (DWORD)val;
		break;
	default:
		rb_raise(rb_eArgError,"width[%d] is not supported.",w);
		break;
	}
	return Qnil;
}


BOOL ancestor_check(VALUE checkee, VALUE ancestor){
	VALUE ancests;
	VALUE inc;

	ancests=rb_mod_ancestors(checkee);
	inc = rb_ary_includes(ancests,ancestor);
	if(inc == Qtrue){
		return TRUE;
	} 
	return FALSE;
	
}

void increment_opened_window(){
    VALUE owin_val = 
        rb_thread_local_aref(rb_thread_current(),rb_intern(OPENEDWINSYM));
    int opened_window=0;
    if(! NIL_P(owin_val)) opened_window = NUM2INT(owin_val);
    opened_window++;
    rb_thread_local_aset(rb_thread_current(),
            rb_intern(OPENEDWINSYM),INT2NUM(opened_window));
}

void decrement_opened_window(){
    VALUE owin_val = 
        rb_thread_local_aref(rb_thread_current(),rb_intern(OPENEDWINSYM));
    int opened_window=0;
	if(! NIL_P(owin_val)) opened_window = NUM2INT(owin_val);
    
    opened_window--;
	if(opened_window<0){
		rb_bug("Confusing window management.");
        opened_window=0;
	}
    rb_thread_local_aset(rb_thread_current(),
            rb_intern(OPENEDWINSYM),INT2NUM(opened_window));
	if(opened_window==0) PostQuitMessage(0);
}

/*******************************************************
*
*    Swin Factory Class
*
*
*/

static VALUE
swinfac_new(int argc, VALUE* argv, VALUE klass){
	VALUE obj;
	struct SwinFactory *sf;
	unsigned hinst;

	if(argc<1){
		rb_raise(rb_eArgError,"hInstance needed\n");
		return Qfalse;
	}
	hinst = NUM2UINT(argv[0]);
	if(SwinFactoryInstance && argc>0 && (unsigned)hInstance==hinst){
		obj=SwinFactoryInstance;
	} else {
		obj = Data_Make_Struct(klass, struct SwinFactory, 0, free, sf);
		sf->hInstance = hinst;
		rb_obj_call_init(obj,argc,argv);
		if(hinst==(unsigned)hInstance){
			SwinFactoryInstance=obj;
		}
	}
	return obj;
}

static VALUE
swinfac_initialize(VALUE self,VALUE instance){
	struct SwinFactory *sf;
	return self;
}


static VALUE 
swinfac_newwindow(int argc, VALUE* argv, VALUE obj){
	VALUE robj;
	VALUE ancest;
	HWND parent;
	VALUE klass;
	struct SwinFactory *sf; 
	struct Swin *sw,*psw;

    if( argc<1 || argv[0]==Qnil){     /* handle of parent window */
	    parent=NULL;
	} else {
        Data_Get_Struct(argv[0], struct Swin, psw);
	    parent= psw->hWnd;
	}

	klass = cSwin;  
	if(argc>1){                      /* cSwin or another? */
		if(ancestor_check(argv[1],cSwin)){
			klass = argv[1];
			--argc;
			++argv;
		} else {
			rb_raise(rb_eArgError,"Class is not a descendant of SWin::Window");
			return Qfalse;
		}
	}

    Data_Get_Struct(obj, struct SwinFactory, sf);

    robj = Data_Make_Struct(klass, struct Swin, 0, shutdown_win, sw);

	sw->hWnd=NULL;
	sw->hParent=parent;
	sw->hInst= sf->hInstance;

	sw->caption =NULL;
	sw->dwStyle=WS_OVERLAPPEDWINDOW;
	sw->dwExStyle=0;
	sw->x=sw->y=sw->w=sw->h=CW_USEDEFAULT;
	sw->classname=NULL;
	sw->hEtc=0;
	sw->enabled=TRUE;

	sw->filter=NULL;
	sw->OrigWndProc=NULL; 
	sw->me=robj;
	swingdi_init(&sw->gs);
	rb_obj_call_init(robj,argc-1,argv+1);
	return robj;
}

unsigned getargnilvoid(int argc,VALUE* argv,int arg,unsigned defaultv){
	unsigned retv;
	if(argc>arg){
		retv= defaultv;
		if(argv[arg]!=Qnil) retv = NUM2UINT(argv[arg]);
	} else{
		retv=defaultv;
	}
	return retv;
}

static VALUE
swinfac_registerWndclass(int argc,VALUE* argv,VALUE klass){
	char buffer[1024];
	BOOL r;
	static int id=0;
    char  *classname;
	TCHAR classnamebuf[1024];
	unsigned int style,hIcon,hBrush,hCursor;

	classname = (argc==0)? "noname" : SWIN_ASCII_STR_PTR(argv[0]);

	hBrush= getargnilvoid(argc,argv,1,(unsigned)(1+COLOR_3DFACE));
	style = getargnilvoid(argc,argv,2,CS_HREDRAW | CS_VREDRAW);
	hIcon = getargnilvoid(argc,argv,3,(unsigned)LoadIcon(NULL,IDI_APPLICATION));
	hCursor=getargnilvoid(argc,argv,4,(unsigned)LoadCursor(NULL, IDC_ARROW));

    // confusing transcoding...
	sprintf(buffer,"%s:%014d:",
                    SWIN_CNAME,
//                    (HINSTANCE)GetModuleHandle(NULL),
                    id++);
    strncat(buffer,classname,sizeof(buffer));
	r = RegisterWndClass(SWIN_API_STR_CHAR(buffer),
                        (HICON)hIcon, style, 
                        (HBRUSH)hBrush,(HCURSOR)hCursor);
	return r? rb_str_new2(buffer) : Qfalse;
}


/*******************************************************
*
*    Swin Message Class
*
*/

#define DefineGetSwinMsgIntParam(x) static VALUE\
	swinmsg_get##x(VALUE obj){ \
	struct SwinMsg *sw; \
    Data_Get_Struct(obj, struct SwinMsg, sw);\
	return UINT2NUM((unsigned)sw->x); \
	}
#define DefineSetSwinMsgIntParam(x) static VALUE\
	swinmsg_set##x(VALUE obj,VALUE num){ \
	struct SwinMsg *sw; \
    Data_Get_Struct(obj, struct SwinMsg, sw);\
	sw->x = NUM2LONG(num); \
	return num; \
	}


DefineGetSwinMsgIntParam(hWnd)
DefineGetSwinMsgIntParam(msg)
DefineGetSwinMsgIntParam(wParam)
DefineGetSwinMsgIntParam(lParam)
DefineGetSwinMsgIntParam(retval)
DefineSetSwinMsgIntParam(msg)
DefineSetSwinMsgIntParam(wParam)
DefineSetSwinMsgIntParam(lParam)

static VALUE
swinmsg_setretval(VALUE obj,VALUE val){
	struct SwinMsg *sw; 
    Data_Get_Struct(obj, struct SwinMsg, sw);
	switch(TYPE(val)){
	  case T_FIXNUM:
	  case T_BIGNUM:
	    sw->retval = NUM2UINT(val);
	    break;
	  case T_TRUE:
	    sw->retval=1;
	    break;
	  case T_FALSE:
	    sw->retval=0;
	    break;
	  case T_STRING:
	    sw->retval=(unsigned int)SWIN_API_STR_PTR(val);
	    break;
	  default:
	    sw->retval=(int)val;
	    break;
	}
	return val;
}

/*******************************************************
*
*    SwinApplication Class
*
*
*/
static int swin_ctrlc_interrupted=FALSE;  /* TRUE if ctrl-c in messageloop */


BOOL WINAPI CTRLCHandler(DWORD dwCtrlType){
	if(dwCtrlType==CTRL_C_EVENT && swin_handling_ctrlc){
		PostThreadMessage(GetCurrentThreadId(),WM_NULL,0,0L);
	}
	return swin_ctrlc_interrupted=swin_handling_ctrlc;
}

void shutdown_swin(VALUE obj){
	LArrayItem* pt;

	LArray_destruct(swlist); 
	swlist=NULL;

	LArray_EACH_START(keyacclist,pt)
		if(((struct KeyAccItem*)pt->data)->original){
			DestroyAcceleratorTable( ((struct KeyAccItem*)(pt->data))->haccel);
		}
		free((struct KeyAccItem*)pt->data); 
	LArray_EACH_END
	
	LArray_destruct(keyacclist);
	keyacclist=NULL;
}


static VALUE  /* Thanks to Yukimi-sake san. */
/* swinapp_setaccel(int argc,VALUE* argv,VALUE obj){ */
swinapp_setaccel(VALUE mod, VALUE rwin, VALUE translator){
	ACCEL accl;
	LPACCEL lpaccl;
	VALUE *pt;
	int cEntries,lary,i;
	struct Swin* sw;
	struct Swin* sw2;
	struct KeyAccItem* ka;
	struct KeyAccItem* ka2;

	Data_Get_Struct(rwin, struct Swin, sw);
	SWIN_WINEXISTCHECK(sw);

	MYMALLOC(ka,sizeof(struct KeyAccItem),Qnil);

	if(TYPE(translator)==T_ARRAY){  /* creates new accelerator */
		pt = RARRAY_PTR(translator);
		lary = RARRAY_LEN(translator);
		cEntries = lary / 3;
		lpaccl = (LPACCEL) LocalAlloc(LPTR, cEntries * sizeof(ACCEL));
		for(i = 0; i < cEntries; i++ ){
			lpaccl[i].fVirt = (BYTE)(FIX2INT(pt[i*3]));
			lpaccl[i].key = (WORD)(FIX2INT(pt[i*3+1]));
		lpaccl[i].cmd = (WORD)(FIX2INT(pt[i*3+2]));
		}
		ka->hwnd = sw->hWnd;
		ka->original=TRUE;
		ka->haccel = CreateAcceleratorTable(lpaccl, cEntries);

	} else if(rb_obj_is_kind_of(translator,cSwin)) {/* forward to another win */
		Data_Get_Struct(translator, struct Swin, sw2);
		if((sw2)->hWnd==NULL){  /* SWINEXISTCHECK */
			free(ka);
			rb_raise(eSwinWindowNotExistError,"Window Not Exist");
			return Qfalse;
		}
		ka2 = (struct KeyAccItem*) LArray_get(keyacclist,(int)sw2->hWnd);
		if(ka2){
			ka->hwnd = sw2->hWnd;
			ka->original=FALSE;
			ka->haccel = ka2->haccel;
		} else {
			free(ka);
			rb_raise(rb_eArgError,"Not registered key-forwarding window.");
		}
	} else {
		free(ka);
		rb_raise(rb_eArgError,"translator must be a Window or an Array that defines accelerator.");
		return Qfalse;
	}

	LArray_set(keyacclist,(int)sw->hWnd,(LArrayDATA)ka);
	return translator;
}

VALUE
swinapp_delaccel(VALUE mod, VALUE rwin){
	struct Swin* sw;
	struct KeyAccItem* ka; 
	LArrayItem* pt;

	Data_Get_Struct(rwin, struct Swin, sw);

	LArray_EACH_START(keyacclist,pt)
		ka = (struct KeyAccItem*)pt->data;
		if(ka->original){
			DestroyAcceleratorTable(ka->haccel);
		}
		pt = pt->next; /* LArray_EACH_END 1/2 */
		LArray_delete(keyacclist,(int)ka->hwnd,TRUE);
	}  /* LArray_EACH_END 2/2 */

	return rwin;
}



static VALUE
swinapp_hinstance(){
	return INT2NUM((long)hInstance);
}

static void 
messageloop_core(MSG *pmsg){
    if(!IsDialogMessage(current_dialog,pmsg)){
        TranslateMessage(pmsg);
        DispatchMessage(pmsg);
    }
}

static VALUE  
swinapp_messageloop(){ 
	MSG msg;
	BOOL iterator_p;
	struct KeyAccItem* ka;
        VALUE opened_window;

    VALUE inloop = rb_thread_local_aref(rb_thread_current(),rb_intern(INMESSAGESYM));

	if(!(!inloop || NIL_P(inloop))){
		rb_raise(rb_eRuntimeError,"Double MessageLoop");
		return Qtrue;
	}
    rb_thread_local_aset(rb_thread_current(),rb_intern(INMESSAGESYM),Qtrue);

    opened_window = 
        rb_thread_local_aref(rb_thread_current(),rb_intern(OPENEDWINSYM));

    if(NIL_P(opened_window) || NUM2INT(opened_window)==0) return Qnil;


	swin_handling_ctrlc=TRUE;
	iterator_p = rb_iterator_p();

/* Thanks, Yoshida-san, Shirakura-san */
	while(1) {
		if (PeekMessage(&msg, NULL, 0,0, PM_REMOVE)){
			if (msg.message == WM_QUIT) break;
			ka = (struct KeyAccItem*) LArray_get(keyacclist,(int)msg.hwnd);
			if(ka){  /* Thanks Yukimisake-san. */
				if(! TranslateAccelerator( ka->hwnd, ka->haccel, &msg)){
					messageloop_core(&msg);
				}
			} else {
				messageloop_core(&msg);
			}
		} else {
			if(swin_ctrlc_interrupted){
				break;
			}
			if(iterator_p){
				swin_handling_ctrlc=FALSE;
				rb_yield(Qnil);
				swin_handling_ctrlc=TRUE;
			} else {
                if(GetQueueStatus(QS_ALLEVENTS)==0) {
                    swin_call_threadblocking(0,0,WaitMessage);
                }
            }
		}
	}

    rb_thread_local_aset(rb_thread_current(),rb_intern(INMESSAGESYM),Qfalse);
	swin_handling_ctrlc=FALSE;

	if(swin_ctrlc_interrupted){
		swin_ctrlc_interrupted=FALSE;
		rb_raise(eSwinInterrupt,"Interrupted by Ctrl-C"); 
	}
	return Qnil;
}

static VALUE 
swinapp_doevents(VALUE obj){
	MSG msg;
	while(PeekMessage(&msg, NULL, 0,0, PM_REMOVE)){
		if (msg.message == WM_QUIT){  /* re-send WM_QUIT to exit messageloop */
            PostMessage(msg.hwnd,msg.message,msg.wParam,msg.lParam);
			break;
		} 
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return obj;
}

static VALUE
swinapp_waitmessage(VALUE obj){
	swin_handling_ctrlc=TRUE;
    swin_call_threadblocking(0,0,WaitMessage);
	swin_handling_ctrlc=FALSE;
	return obj;
}

static VALUE
swinapp_getdesktop(){
	VALUE robj;
	RECT  rc;
	struct Swin *sw;

    robj = Data_Make_Struct(cSwin, struct Swin, 0, shutdown_win, sw);

	sw->hWnd=GetDesktopWindow();
	sw->hInst= 0;
    sw->hParent=NULL;
	sw->caption =NULL;
	sw->dwStyle=0;
	
	GetWindowRect(sw->hWnd, &rc);
	
	sw->x=sw->y=0;
	sw->w=rc.right;
	sw->h=rc.bottom;
	sw->classname=NULL;
	sw->hEtc=0;
	sw->enabled=TRUE;

	sw->filter=NULL;
	sw->me=robj;
	return robj;
}

static VALUE
swinapp_msgwin(){
	LArrayItem* pt;
	struct Swin *sw;
	int n=0;

	pt = swlist->items;
	while(pt->id != NULL_ID){
		sw = (struct Swin*) pt->data;
		rb_yield(sw->me);
		pt=pt->next;
		n++;
	}

	return INT2NUM(n);
}

static VALUE
swinapp_alivewindows(VALUE obj){
    return rb_thread_local_aref(rb_thread_current(),rb_intern(OPENEDWINSYM));
}

static VALUE
swinapp_setcritical(VALUE obj,VALUE b){
	if(b==Qnil || b== Qfalse){
		threadcritical = FALSE;
	} else {
		threadcritical = TRUE;
	}
	return b;
}

static VALUE
swinapp_getcritical(VALUE obj){
	if(threadcritical) return Qtrue;
	return Qfalse;
}

VALUE
get_systemmetrics(int sm){
	return INT2NUM(GetSystemMetrics(sm));
}
VALUE
get_systemmetrics2(int sm1,int sm2){
	VALUE robj;
	robj = rb_ary_new();
	rb_ary_push(robj,get_systemmetrics(sm1));
	rb_ary_push(robj,get_systemmetrics(sm2));
	return robj;
}


VALUE
swin_t_messageBox(int argc,VALUE *argv,HWND parent){
	int r;
    TCHAR *text,*title;
    int style;

    if(argc<1){
    	rb_raise(rb_eArgError,"message not given");
    	return Qfalse;
    }

	text = SWIN_API_STR_PTR(argv[0]);
	title= (argc>1)? SWIN_API_STR_PTR(argv[1]) : TEXT("MessageBox");
	style= (argc>2)? NUM2UINT(argv[2]) : 0;
	r = swin_call_threadblocking(0,4,MessageBox,
                parent,text,title,style);

	return INT2NUM(r);
}

static VALUE
swin_s_messageBox(int argc,VALUE* argv, VALUE klass){
	return swin_t_messageBox(argc,argv,NULL);
}

static VALUE
swinapp_getcborders(){ return get_systemmetrics2(SM_CXBORDER,SM_CYBORDER); }
static VALUE
swinapp_getcycaption(){ return get_systemmetrics(SM_CYCAPTION); }
static VALUE
swinapp_getccursors(){ return get_systemmetrics2(SM_CXCURSOR,SM_CYCURSOR); }
static VALUE
swinapp_getcdlgframes(){return get_systemmetrics2(SM_CXDLGFRAME,SM_CYDLGFRAME);}
static VALUE
swinapp_getcframes(){ return get_systemmetrics2(SM_CXFRAME,SM_CYFRAME); }
static VALUE
swinapp_getcicon(){ return get_systemmetrics2(SM_CXICON,SM_CYICON); }
static VALUE
swinapp_getcmin(){ return get_systemmetrics2(SM_CXMIN,SM_CYMIN); }

static VALUE
swinapp_swin(VALUE obj){
	return mSwin;
}

VALUE
swinaa_application(VALUE obj){
	return mSwinApp;
}

static VALUE
swinapp_winpath(VALUE mod,VALUE str){
	TCHAR buffer[1024];
	VALUE pathstr = SWIN_API_STR(str);
	if(RSTRING_LEN(pathstr)>1024){
		rb_raise(rb_eRuntimeError,"String too long");
		return Qfalse;
	}
	path_conv_to_win((TCHAR*)RSTRING_PTR(pathstr),buffer);
	return SWIN_OUTAPI_STR_NEW2(buffer);
}

static VALUE
swinapp_changebackbrush(VALUE mod, VALUE cname,VALUE val){
	HWND hwnd;
	TCHAR * classname;
	int r;

	if(cname==Qnil) { 
		classname = SWIN_T_CNAME;
	}
	else { 
		classname = SWIN_API_STR_PTR(cname); 

    	if(SWIN_ORIGINALWNDCLASS(SWIN_API_STR_PTR(cname))){
		    rb_raise(rb_eRuntimeError,
	    	        "attempting to modify class value of another library.(%s))",
		            classname);
	    	return Qfalse;
    	}
	}

	hwnd = CreateWindowEx(0,classname,TEXT(""),0, 0,0,0,0, 0,0, 
	                      hInstance, SWIN_TEMPWINDOW_CREATE_LPARAM);
	if(!hwnd){
		rb_raise(rb_eRuntimeError,
			"Window creation failed. classname[%s] valid?",classname);
		return Qfalse;
	}
	r = SetClassLong(hwnd,GCL_HBRBACKGROUND,NUM2UINT(val));

	DestroyWindow(hwnd);
	return UINT2NUM(r);
}

static VALUE
swinapp_lasterror(int argc,VALUE* argv, VALUE obj){
	DWORD ecode;
	TCHAR buffer[1024];
  int len;
	ecode = GetLastError();
	
	if(argc>0 && TYPE(argv[0])==T_FIXNUM){
		return UINT2NUM(ecode);
	}
	len=FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,0,
	  ecode, 0, buffer, sizeof(buffer), NULL);
	return SWIN_OUTAPI_STR_NEW(buffer, len); 
}

static VALUE
swinapp_setlasterror(VALUE obj, VALUE ecode){
	SetLastError(NUM2UINT(ecode));
	return obj;
}


/*-------------------------
    swin m17n for ruby19 */
static VALUE
swinapp_unicode(VALUE obj) {
#ifdef UNICODE
    return Qtrue;
#else
    return Qfalse;
#endif
}

#ifdef SWIN_FOR_RUBY19_M17N
static VALUE 
swinapp_outputencoding(VALUE obj, VALUE enc) {
    outputEncoding = enc; return enc;
}
static VALUE 
swinapp_getoutputencoding(VALUE obj) {
    return outputEncoding;
}
#endif
/*-------------------------*/



/*
***************************************************************:
*
*         Global functions
*
*/

typedef struct {
    int num;
    unsigned long params[16];
    FARPROC winapi_proc;
} WinAPI_CallData;

DWORD Swin_WinAPI_Call(WinAPI_CallData* arg) {
    switch (arg->num)
    {
    case 0:
        return (arg->winapi_proc)();
    case 1:
        return (arg->winapi_proc)(arg->params[0]);        
    case 2:
        return (arg->winapi_proc)(arg->params[0], arg->params[1]);        
    case 3:
        return (arg->winapi_proc)(arg->params[0], arg->params[1],
            arg->params[2]);                
    case 4:
        return (arg->winapi_proc)(arg->params[0], arg->params[1],
                                  arg->params[2], arg->params[3]);                
    case 5:
        return (arg->winapi_proc)(arg->params[0], arg->params[1],
                                  arg->params[2], arg->params[3],
                                  arg->params[4]);
    case 6:
        return (arg->winapi_proc)(arg->params[0], arg->params[1],
                                  arg->params[2], arg->params[3],
                                  arg->params[4], arg->params[5]);
    case 7:
        return (arg->winapi_proc)(arg->params[0], arg->params[1],
                                  arg->params[2], arg->params[3],
                                  arg->params[4], arg->params[5],
                                  arg->params[6]);
    case 8:
        return (arg->winapi_proc)(arg->params[0], arg->params[1],
                                  arg->params[2], arg->params[3],
                                  arg->params[4], arg->params[5],
                                  arg->params[6], arg->params[7]);
    default:
        rb_bug("[bug] args >= 9");
    }
}

DWORD
swin_call_threadblocking(DWORD chk_th, int num, FARPROC proc, ...) {
    WinAPI_CallData calldata;
    va_list args;
    int i;
    DWORD r;

    calldata.winapi_proc = proc;
    calldata.num = num;
    va_start(args,proc);
    for(i=0; i<num; i++) {
        calldata.params[i] = va_arg(args, unsigned long); 
    }
    va_end(args);

#ifdef HAVE_NATIVETHREAD
    if(chk_th!=GetCurrentThreadId()){  // another thread from current
        r = (DWORD)rb_thread_blocking_region((VALUE(*)())Swin_WinAPI_Call,
                        &calldata,RUBY_UBF_IO,0);
    } 
    else 
#endif /* HAVE_NATIVETHREAD */
    {
        r = Swin_WinAPI_Call(&calldata);
        fflush(stdout);
    }

    return r;
}


static VALUE
swin_obsolete_messageBox(int argc, VALUE* argv, VALUE klass){
	if(argc!=4){rb_raise(rb_eArgError,"argument missed");return Qfalse;}
	return swin_t_messageBox(argc-1,argv+1,(HWND)NUM2INT(argv[0]));
}

 /* Set Old type names. Some day this function will be deleted.  */
void ForCompatibility(){ 
	rb_define_global_const("SWIN",cSwin);
	rb_define_global_const("SWinMSG",cSwinMsg);
	rb_define_global_const("LWFactory",cSwinFactory);
	rb_define_global_const("WindowNotExistError",eSwinWindowNotExistError);
	rb_define_global_const("SWinMsgloopInterrupt",eSwinInterrupt);
	rb_define_global_const("SWinApplication",mSwinApp);

	rb_define_global_const("SWinBitmap",cSwinBitmap);
	rb_define_global_const("SWinMenu",cSwinMenu);

	rb_define_global_function("cstruct2array",app_struct2array,2);
	rb_define_global_function("arg2cstructStr",app_arg2struct,-1);

	rb_define_global_function("pointer2string",app_pointer2string,-1);

	rb_define_global_function("NullMsgBox",swin_obsolete_messageBox,-1);
}


void Init_swin(){
#ifdef SWIN_FOR_RUBY19_M17N
  UINT acp;
#endif
	swlist = LArray_new();
	keyacclist = LArray_new();

#ifdef SWIN_FOR_RUBY19_M17N
    acp = GetACP();  // not GetConsoleCP/GetConsoleOutputCP
    if(acp){
        char buf[16];  // for UINT
        sprintf(buf,"cp%u",acp);
        outputEncoding = rb_enc_from_encoding(rb_enc_find(buf));
    } else {
        outputEncoding = 
            rb_enc_from_encoding(rb_default_external_encoding());
    }
    apiEncoding = rb_enc_from_encoding(rb_enc_find("UTF-16LE"));
    apiEncodingIndex = rb_to_encoding_index(apiEncoding);
    id_encode = rb_intern("encode");
  
#endif

	rb_set_end_proc(shutdown_swin,Qnil);
	SetConsoleCtrlHandler(CTRLCHandler,TRUE); 

/* for Application.thread_critical */
#if 0 /* NT */
	threadcritical = TRUE;
#else 
	threadcritical = FALSE;
#endif

	get_hInstance();
	RegisterWndClass(SWIN_T_CNAME,
	                 LoadIcon(NULL,IDI_APPLICATION),0,(HBRUSH)(1+COLOR_3DFACE),
	                 LoadCursor(NULL, IDC_ARROW));

	mSwin    = rb_define_module("SWin");
    eSwinResourceNotExistError=
        rb_define_class_under(mSwin,"ResourceNotExistError",rb_eRuntimeError);

	cSwinMsg     = rb_define_class_under(mSwin,"MSG",rb_cObject);
	cSwinFactory = rb_define_class_under(mSwin,"LWFactory",rb_cObject);
    eSwinWindowNotExistError = rb_define_class_under(mSwin,
        "WindowNotExistError",eSwinResourceNotExistError);
    mSwinApp     = rb_define_module_under(mSwin,"Application");

	eSwinInterrupt=
	    rb_define_class_under(mSwin,"MsgloopInterrupt",rb_eInterrupt);

	rb_define_singleton_method(cSwinFactory,"new",swinfac_new,-1);
	rb_define_method(cSwinFactory,"initialize",swinfac_initialize,1);
	rb_define_method(cSwinFactory,"newwindow",swinfac_newwindow,-1);
	rb_define_method(cSwinFactory,"registerWinClass",swinfac_registerWndclass,-1);
	rb_define_method(cSwinFactory,"application",swinaa_application,0);

	rb_define_method(cSwinMsg,"hWnd",swinmsg_gethWnd,0);
	rb_define_method(cSwinMsg,"msg",swinmsg_getmsg,0);
	rb_define_method(cSwinMsg,"wParam",swinmsg_getwParam,0);
	rb_define_method(cSwinMsg,"lParam",swinmsg_getlParam,0);
	rb_define_method(cSwinMsg,"retval",swinmsg_getretval,0);
	rb_define_method(cSwinMsg,"retval=",swinmsg_setretval,1);

	rb_define_method(cSwinMsg,"msg=",swinmsg_setmsg,1);
	rb_define_method(cSwinMsg,"wParam=",swinmsg_setwParam,1);
	rb_define_method(cSwinMsg,"lParam=",swinmsg_setlParam,1);

	rb_define_singleton_method(mSwinApp,"setAccel",swinapp_setaccel,2);
	rb_define_singleton_method(mSwinApp,"removeAccel",swinapp_delaccel,2);
	rb_define_singleton_method(mSwinApp,"hInstance",swinapp_hinstance,0);
	rb_define_singleton_method(mSwinApp,"getDesktop",swinapp_getdesktop,0);
	rb_define_singleton_method(mSwinApp,"messageloop",swinapp_messageloop,-1);
	rb_define_singleton_method(mSwinApp,"doevents",swinapp_doevents,0);
	rb_define_singleton_method(mSwinApp,"waitmessage",swinapp_waitmessage,0);
	rb_define_singleton_method(mSwinApp,"eachMsghandled",swinapp_msgwin,0);
	rb_define_singleton_method(mSwinApp,"windowcount",swinapp_alivewindows,0);
	rb_define_singleton_method(mSwinApp,"messageBox",swin_s_messageBox,-1);

	rb_define_singleton_method(mSwinApp,"thread_critical",swinapp_getcritical,0);	rb_define_singleton_method(mSwinApp,"thread_critical=",swinapp_setcritical,1);

	rb_define_singleton_method(mSwinApp,"winpath",swinapp_winpath,1);
	rb_define_singleton_method(mSwinApp,"winborders",swinapp_getcborders,0);
	rb_define_singleton_method(mSwinApp,"captionheight",swinapp_getcycaption,0);
	rb_define_singleton_method(mSwinApp,"cursorsizes",swinapp_getccursors,0);
	rb_define_singleton_method(mSwinApp,"dlgborders",swinapp_getcdlgframes,0);
	rb_define_singleton_method(mSwinApp,"resizeborders",swinapp_getcframes,0);
	rb_define_singleton_method(mSwinApp,"iconsizes",swinapp_getcicon,0);
	rb_define_singleton_method(mSwinApp,"winminimums",swinapp_getcmin,0);

	rb_define_singleton_method(mSwinApp,"changebkbrush",swinapp_changebackbrush,2);

	rb_define_singleton_method(mSwinApp,"cstruct2array",app_struct2array,2);
	rb_define_singleton_method(mSwinApp,"unpack",app_unpack,2);
	rb_define_singleton_method(mSwinApp,"arg2cstructStr",app_arg2struct,-1);
	rb_define_singleton_method(mSwinApp,"pointer2string",app_pointer2string,-1);
	rb_define_singleton_method(mSwinApp,"pokeMemory",app_pokeMemory,3);

	rb_define_singleton_method(mSwinApp,"swinmodule",swinapp_swin,0);

	rb_define_singleton_method(mSwinApp,"lasterror",swinapp_lasterror,-1);
	rb_define_singleton_method(mSwinApp,"set_lasterror",swinapp_setlasterror,1);

	rb_define_singleton_method(mSwinApp,"unicode?",swinapp_unicode,0);

#ifdef SWIN_FOR_RUBY19_M17N
	rb_define_singleton_method(mSwinApp,"output_encoding=",swinapp_outputencoding,1);
	rb_define_singleton_method(mSwinApp,"output_encoding",swinapp_getoutputencoding,0);
#endif

	Init_swinWindow();
	Init_swinGDI();
	Init_swinBitmap();
	Init_swinMenu();
	Init_CDlg();
	Init_swinCanvas();
	Init_swinRsc();
	Init_swinDialog();
	Init_swinPrinter();
	Init_swinolednd();
	Init_swinole();

	rb_define_const(mSwin,"VERSION",rb_str_new2(SWIN_VERSION));

/*  for debugging  */
/*	rb_define_global_function("usecomctl",(VALUE)InitCommonControls,0); */
	ForCompatibility(); 
}
