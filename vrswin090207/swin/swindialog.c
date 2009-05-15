/*
###################################
#
# swindialog.c
# Programmed by nyasu <nyasu@osk.3web.ne.jp>
# Copyright 1999-2005  Nishikawa,Yasuhiro
#
# More information at http://vruby.sourceforge.net/index.html
#
###################################
*/

#include "swin.h"

#define DIALOG_TIMERID 0xfffe
#define DIALOG_TIMESPAN 20

VALUE cSwinDialog;
int dialog_timespan=DIALOG_TIMESPAN;

static VALUE
swindlg_gettimespan(VALUE obj){
	return INT2NUM(dialog_timespan);
}
static VALUE
swindlg_settimespan(VALUE obj,VALUE span){
	dialog_timespan = NUM2INT(span);
	return span;
}

BOOL DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam){
	struct SwinDialog *sd;
	struct Swin *sw;
	VALUE  ymsg;
	struct SwinMsg *sm;

	if(msg==WM_INITDIALOG){
		sd = (struct SwinDialog*)lParam;
		sd->sw.hWnd=hWnd;
		Register_sw(hWnd,&(sd->sw));
		if(sd->sw.hParent==0 && sd->modal==0) increment_opened_window();
		if(dialog_timespan>0)
			SetTimer(hWnd,DIALOG_TIMERID,dialog_timespan,NULL);
	} else if(msg==WM_ACTIVATE){
		if(LOWORD(wParam)==WA_INACTIVE){
			current_dialog = NULL;
		} else {
			current_dialog = hWnd;
		}
	}

	if(msg==WM_TIMER && wParam==DIALOG_TIMERID){
		rb_thread_schedule();
		return FALSE;
	}
	sw=Get_sw(hWnd);
	if(sw){
		ymsg=Data_Make_Struct(cSwinMsg, struct SwinMsg, 0, free, sm);
		sm->hWnd=hWnd;        sm->msg=msg;
		sm->wParam=wParam;    sm->lParam=lParam;
		sm->retval = 0;
		if(message_handling(sw,sm,ymsg)==FALSE){
			return sm->retval;
		};
		switch(msg){
		case WM_CLOSE:
			Data_Get_Struct(sw->me,struct SwinDialog,sd);
			if(sd->modal){
				EndDialog(hWnd,(int)Qfalse);
			} else {
				DestroyWindow(hWnd);
			}
			return TRUE;
			break;
		case WM_DESTROY:
			Data_Get_Struct(sw->me,struct SwinDialog,sd);
			if(sd->sw.hParent==0 && sd->modal==0) decrement_opened_window();
			KillTimer(hWnd,DIALOG_TIMERID);
			sw->hWnd=NULL;
			Unregister_sw(hWnd); 
			return TRUE;
			break;
		}
	}
	return FALSE; 
}

void shutdown_dialog(struct SwinDialog* sd){
	if(sd->template) free(sd->template);
	free(sd);
}

static VALUE
swinfac_newdialog(int argc, VALUE* argv, VALUE obj){
/* newdialog(template,class) */

	VALUE robj;
	VALUE template;
	VALUE klass;

	DLGTEMPLATE* tpltbuffer;
	unsigned tpltsize;

	struct SwinDialog  *sd;
	struct Swin        *sw;
	struct SwinFactory *sf;

	if(argc==0){
		rb_raise(rb_eArgError,"not specify template");
		return Qfalse;
	}
	template = StringValue(argv[0]);

	klass = cSwinDialog;
	if(argc>1){
		if(ancestor_check(argv[1],cSwinDialog)){
			klass = argv[1];
			--argc;
			++argv;
		} else {
			rb_raise(rb_eArgError,"Class is not a descendant of SWin::Dialog");
			return Qfalse;
		}
	}

	Data_Get_Struct(obj, struct SwinFactory, sf);
	robj = Data_Make_Struct(klass, struct SwinDialog, 0, shutdown_dialog, sd);
	if(!rb_obj_is_kind_of(template,rb_cString)){
		rb_raise(rb_eArgError,"template must be String");
		return Qfalse;
	}
	tpltbuffer = (DLGTEMPLATE*)RSTRING_PTR(template);
	tpltsize   = RSTRING_LEN(template);

	MYMALLOC(sd->template,tpltsize,Qfalse);
	memcpy(sd->template,tpltbuffer,tpltsize);
	sw = &(sd->sw);

	sw->hWnd=NULL;
	sw->hParent=NULL;
	sw->hInst= sf->hInstance;
	sw->caption=TEXT("");

/*	sw->dwStyle=WS_POPUP | WS_SYSMENU | WS_CAPTION | WS_VISIBLE | 
	            DS_MODALFRAME | DS_SETFONT;
*/
	sw->dwStyle=tpltbuffer->style;
	sw->dwExStyle=0;
	sw->x = tpltbuffer->x;
	sw->y = tpltbuffer->y;
	sw->w = tpltbuffer->cx;
	sw->h = tpltbuffer->cy;
	sw->classname=TEXT("swindialog");

	sw->hEtc=0;
	sw->enabled=TRUE;
	sw->filter=NULL;
	sw->OrigWndProc=NULL; 
	sw->me=robj;
	swingdi_init(&sw->gs);

	rb_obj_call_init(robj,argc-1,argv+1);
	return robj;
}

static VALUE
swindlg_initialize(int argc, VALUE* argv, VALUE self){
	return self;
}

/*  create(parent=nil, modal=true) */
static VALUE
swindlg_create(int argc, VALUE* argv, VALUE obj){
	struct Swin *psw;
	struct SwinDialog *sd;
	DLGTEMPLATE* template;
	HWND parent;
	int r;
	int modal_flag=1;

	if(argc<1 || argv[0] == Qnil){
		parent = NULL;
	} else {
		Data_Get_Struct(argv[0], struct Swin, psw);
		parent = psw->hWnd;
	}

	if(argc>1 && (argv[1]==Qnil || argv[1]==Qfalse) ){
		modal_flag = 0;
	} else {
		modal_flag = 1;
	}

    Data_Get_Struct(obj, struct SwinDialog, sd);
	/* Need to check! below description is obsolete */
	/* no need to check double creation because this is modaldialog */
	sd->sw.hParent=parent;
    sd->sw.nthread_id = GetCurrentThreadId();
	template = sd->template;
	sd->modal = modal_flag;

	template->style = sd->sw.dwStyle;
	template->dwExtendedStyle=sd->sw.dwExStyle;
	template->x = sd->sw.x;
	template->y = sd->sw.y;
	template->cx = sd->sw.w;
	template->cy = sd->sw.h;
	
	if(modal_flag==1){
		r = DialogBoxIndirectParam(hInstance,template,parent,
		                            (DLGPROC)DlgProc,(long)sd);
	} else {
		CreateDialogIndirectParam(hInstance,template,parent,
		                            (DLGPROC)DlgProc,(long)sd);
		r = (int)Qnil;
	}
	if(r==-1){
		rb_raise(rb_eRuntimeError,"create dialog failed");
		return Qfalse;
	}
	return (VALUE)r; 
}

static VALUE
swindlg_close(VALUE obj,VALUE retval){
	struct SwinDialog* sd;
	Data_Get_Struct(obj,struct SwinDialog,sd);
	SWIN_WINEXISTCHECK(&(sd->sw));
    if(sd->modal){
        swin_call_threadblocking(sd->sw.nthread_id,2,
                EndDialog,sd->sw.hWnd,(int)retval);
    } else {
        swin_call_threadblocking(sd->sw.nthread_id,1,
                DestroyWindow,sd->sw.hWnd);
	}
	return obj;
}

static VALUE
swindlg_getitemtext(VALUE obj,VALUE itemid){
	TCHAR buffer[4096];
	int len;

	struct SwinDialog* sd;
	Data_Get_Struct(obj,struct SwinDialog,sd);
	
	SWIN_WINEXISTCHECK(&(sd->sw));
	len=swin_call_threadblocking(sd->sw.nthread_id,4,(FARPROC)
        GetDlgItemText,sd->sw.hWnd,NUM2UINT(itemid),buffer,sizeof(buffer));
	return SWIN_OUTAPI_STR_NEW(buffer,len);
}

static VALUE
swindlg_setitemtext(VALUE obj,VALUE itemid,VALUE text){
	BOOL r;

	struct SwinDialog* sd;
	Data_Get_Struct(obj,struct SwinDialog,sd);
	
	SWIN_WINEXISTCHECK(&(sd->sw));
	r=swin_call_threadblocking(sd->sw.nthread_id,3,(FARPROC)
        SetDlgItemText,sd->sw.hWnd,NUM2UINT(itemid),SWIN_API_STR_PTR(text));
	if(r) return Qtrue;
	return Qfalse;
}

static VALUE
swindlg_getcheckstate(VALUE obj,VALUE itemid){
	unsigned int r;
	struct SwinDialog* sd;
	Data_Get_Struct(obj,struct SwinDialog,sd);
	
	SWIN_WINEXISTCHECK(&(sd->sw));
	r=swin_call_threadblocking(sd->sw.nthread_id,2,(FARPROC)
            IsDlgButtonChecked,sd->sw.hWnd,NUM2UINT(itemid));
	switch(r){
	case 0:
		return Qfalse;
	case 1:
		return Qtrue;
	case 2:
		return Qnil;
	}
	return Qnil;  /* dummy */
}
static VALUE
swindlg_setcheckstate(VALUE obj,VALUE itemid, VALUE stt){
	unsigned int r;
	struct SwinDialog* sd;
	Data_Get_Struct(obj,struct SwinDialog,sd);
	
	SWIN_WINEXISTCHECK(&(sd->sw));

	switch(stt){
	case Qfalse:
		r=0;
		break;
	case Qtrue:
		r=1;
		break;
	case Qnil:
		r=2;
		break;
	}
	r = swin_call_threadblocking(sd->sw.nthread_id,3,
            CheckDlgButton,sd->sw.hWnd,NUM2UINT(itemid),r);

	if(r) return Qtrue;
	return Qfalse;
}

static VALUE
swindlg_senddlgmsg(VALUE obj,VALUE id,VALUE msg,VALUE wparam,VALUE lparam){
	struct SwinDialog* sd;
	int lp;
	int r;

	Data_Get_Struct(obj,struct SwinDialog,sd);
	
	SWIN_WINEXISTCHECK(&(sd->sw));

	switch(TYPE(lparam)){
		case T_FIXNUM:
			lp=NUM2UINT(lparam);
			break;
		case T_STRING:
			lp=(unsigned long)StringValuePtr(lparam);
			break;
		default:
			rb_warn("lparam is neither Integer nor String");
			lp=(unsigned long)StringValuePtr(lparam);
		break;
	}

	r=swin_call_threadblocking(sd->sw.nthread_id,5,(FARPROC)
            SendDlgItemMessage,sd->sw.hWnd, NUM2UINT(id),
	                      NUM2UINT(msg),(WPARAM)NUM2UINT(wparam),(LPARAM)lp);

	return INT2NUM(r);
}

static VALUE
swindlg_hooked_q(VALUE obj){
	return Qtrue;
}

void
Init_swinDialog(){
	cSwinDialog = rb_define_class_under(mSwin,"Dialog",cSwin);
	rb_define_method(cSwinFactory,"newdialog",swinfac_newdialog,-1);
	rb_define_singleton_method(cSwinDialog,"new",swin_avoid_new,-1);
	rb_define_method(cSwinDialog,"initialize",swindlg_initialize,-1);
	rb_define_method(cSwinDialog,"create",swindlg_create,-1);
	rb_define_method(cSwinDialog,"open",swindlg_create,-1);
	rb_define_method(cSwinDialog,"close",swindlg_close,1);
	rb_define_method(cSwinDialog,"enddialog",swindlg_close,1);

	rb_define_method(cSwinDialog,"hookwndproc",swindlg_hooked_q,0);
	rb_define_method(cSwinDialog,"unhookwndproc",swindlg_hooked_q,0);
	rb_define_method(cSwinDialog,"hookedwndproc?",swindlg_hooked_q,0);


	rb_define_method(cSwinDialog,"getItemTextOf",swindlg_getitemtext,1);
	rb_define_method(cSwinDialog,"setItemTextOf",swindlg_setitemtext,2);

	rb_define_method(cSwinDialog,"getCheckOf",swindlg_getcheckstate,1);
	rb_define_method(cSwinDialog,"setCheckOf",swindlg_setcheckstate,2);

	rb_define_method(cSwinDialog, "sendItemMessage",swindlg_senddlgmsg,4);

	rb_define_singleton_method(cSwinDialog,"block_timeout",swindlg_gettimespan,0);
	rb_define_singleton_method(cSwinDialog,"block_timeout=",swindlg_settimespan,1);

}

