/*
###################################
#
# swinwin.c
# Programmed by nyasu <nyasu@osk.3web.ne.jp>
# Copyright 1999-2005  Nishikawa,Yasuhiro
#
# More information at http://vruby.sourceforge.net/index.html
#
###################################
*/

#include "swin.h"
#include "rubysig.h"


/*******************************************************
*
*    Swin Class
*
*
*/

ID msghandler_id;

VALUE
swin_avoid_new(int argc,VALUE *argv,VALUE klass){
        rb_raise(rb_eStandardError,"Use factory class instead of ThisClass#new.");
        return Qfalse;
}


BOOL q_hitfilter(struct SwinFilter* filter, unsigned msg){
        while(filter!=NULL){
                if(filter->msg==msg) return TRUE;
                filter=filter->next;
        }
        return FALSE;
}

void freeall_filter(struct Swin *sw){
        struct SwinFilter *filter,*ff;

        filter=sw->filter;
        sw->filter=NULL;

        while(filter!=NULL){
                ff=filter;
                filter=filter->next;
                free(ff);
        }
}

void shutdown_win(struct Swin *sw){

        /* release GDI Objects */
        if(sw->gs.selectedObjFlag){
                sw->gs.hdc=GetDC(sw->hWnd);
                swingdi_release(&sw->gs);
                ReleaseDC(sw->hWnd,sw->gs.hdc);
        }

    /* WndProc reset */
        if(sw->OrigWndProc){
                SetWindowLong(sw->hWnd,GWL_WNDPROC,(long)sw->OrigWndProc);
                sw->OrigWndProc=NULL;

        }
        DestroyWindow(sw->hWnd);
        freeall_filter(sw);
        free(sw->caption);
        free(sw->classname);

        free(sw);
}


void sync_sw(struct Swin *sw,HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam){
    switch (msg) {
    case WM_SIZE:
                sw->w=LOWORD(lParam);
                sw->h=HIWORD(lParam);
                break;
        case WM_MOVE:
                sw->x=LOWORD(lParam);
                sw->y=HIWORD(lParam);
                break;
        }
}

BOOL message_handling(struct Swin* sw,struct SwinMsg* sm,VALUE ymsg){
        VALUE rv;
        rv=Qnil;
       /* sync_sw(sw,sm->hWnd,sm->msg,sm->wParam,sm->lParam); */

        if(q_hitfilter(sw->filter,sm->msg)){   /*  call handler!!   */

                if(sm->msg==WM_PAINT){ /* begin PAINT block */
                        if(sw->painting==0){
                                BeginPaint(sm->hWnd,&(sw->ps));
                        }
                        sw->painting++;
                }

                swin_handling_ctrlc=FALSE;  /* ruby itself handles ctrlc */

                if( (sm->msg==WM_PAINT || threadcritical)){
                                                                               /* begin critical block */
                }
                rv=rb_funcall(sw->me,msghandler_id,1,ymsg);
                if(0){
                                                                                /* end critical block */
                }

                swin_handling_ctrlc=TRUE;

                if(sm->msg==WM_PAINT){
                        sw->painting--;
                        if(sw->painting==0){
                                swingdi_release(&(sw->gs));
                                EndPaint(sm->hWnd,&(sw->ps));
                        }
                }
        }

        if(rv!=Qnil) return(FALSE);
        return TRUE;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
        BOOL f_defcall=TRUE;
        struct Swin *sw;
        VALUE  ymsg;
        struct SwinMsg *sm;
        sw=Get_sw(hWnd);

        if(sw){
                ymsg=Data_Make_Struct(cSwinMsg, struct SwinMsg, 0, free, sm);
                sm->hWnd=hWnd;        sm->msg=msg;
                sm->wParam=wParam;    sm->lParam=lParam;
                sm->retval = 0;

                if(message_handling(sw,sm,ymsg)==FALSE){
                        return sm->retval;
                };

                msg=sm->msg; wParam=sm->wParam; lParam=sm->lParam;

                if(!SWIN_ORIGINALWNDCLASS(sw->classname)){
                        if(sw->OrigWndProc)
                                return(CallWindowProc(sw->OrigWndProc,hWnd,msg,wParam,lParam));
                return(DefWindowProc(hWnd, msg, wParam, lParam));
                }
        }

        /*   default process   */
        f_defcall=TRUE;
        
    switch (msg) {
        case WM_CREATE:
                if( ((CREATESTRUCT*)lParam)->hwndParent==NULL &&
                        ((CREATESTRUCT*)lParam)->lpCreateParams!=SWIN_TEMPWINDOW_CREATE_LPARAM ){
                                increment_opened_window();
                }
                break;
        case WM_CLOSE:
                DestroyWindow(hWnd);
                f_defcall=FALSE;
                break;
        case WM_DESTROY:
                if(sw){
                        if(sw->hParent==0) decrement_opened_window();
                        sw->hWnd=NULL;
                        f_defcall=FALSE;
                        Unregister_sw(hWnd);
                }
                break;
        default:
                f_defcall=TRUE;
        }

        if(f_defcall){
        return(DefWindowProc(hWnd, msg, wParam, lParam));
    }

    return (0L);
}

static VALUE
swin_getCaption(VALUE obj){
        struct Swin *sw;
    Data_Get_Struct(obj, struct Swin, sw);

        if(sw->hWnd){
                int len=GetWindowTextLength(sw->hWnd);
                if(len>lstrlen(sw->caption)){
                        free(sw->caption);
/*                      sw->caption=malloc(len+1); */
                        MYMALLOC(sw->caption,len*sizeof(TCHAR)+1,Qfalse);
                        GetWindowText(sw->hWnd,sw->caption,len+1);
                } else {
                        GetWindowText(sw->hWnd,sw->caption,len+1);
                }
        }
        return SWIN_OUTAPI_STR_NEW2(sw->caption);
}

static VALUE
swin_setCaption(VALUE obj,VALUE capt){
    struct Swin *sw;
    VALUE ct;

    ct = rb_funcall(capt,rb_intern("to_s"),0);
    ct = SWIN_API_STR(ct);
    Data_Get_Struct(obj, struct Swin, sw);

        if(sw->hWnd){
            swin_call_threadblocking(sw->nthread_id,2,SetWindowText,
                        sw->hWnd,(TCHAR*)RSTRING_PTR(ct));
        } else {
                if(sw->caption) free(sw->caption);
                MYMALLOC(sw->caption,RSTRING_LEN(ct)+1,Qfalse);
                lstrcpy(sw->caption,(TCHAR*)RSTRING_PTR(ct));
        }
        return capt;
}


static VALUE
swin_getClassname(VALUE obj){
    struct Swin *sw;
    Data_Get_Struct(obj, struct Swin, sw);
    if(sw->classname==NULL){
        return Qnil;
    }
    return SWIN_OUTAPI_STR_NEW2(sw->classname);
}

static VALUE
swin_setClassname(VALUE obj,VALUE cnameobj){
        struct Swin *sw;
        VALUE cname = SWIN_API_STR(cnameobj);

    Data_Get_Struct(obj, struct Swin, sw);

        if(sw->classname){
                if(RSTRING_LEN(cname) > lstrlen(sw->classname)*sizeof(TCHAR)){
                        free(sw->classname);
/*                      sw->classname = malloc(RSTRING_LEN(cname)+1);*/
                        MYMALLOC(sw->classname,RSTRING_LEN(cname)+1,Qfalse);
                }
        } else {
/*              sw->classname = malloc(RSTRING_LEN(cname)+1);*/
                MYMALLOC(sw->classname,RSTRING_LEN(cname)+1,Qfalse);
        }

        lstrcpy(sw->classname,(TCHAR*)RSTRING_PTR(cname));

        return obj;
}


static VALUE
swin_create(int argc,VALUE* argv, VALUE obj){
    struct Swin *sw;
    int exstyle;
    Data_Get_Struct(obj, struct Swin, sw);

    exstyle = (argc>0)? NUM2UINT(argv[0]) : sw->dwExStyle;
        sw->dwExStyle|=exstyle;

        if(sw->hWnd){
                rb_warn("created existing window object");
                return obj;  /* CAUSE BUG?  do nothing if created */
        }

        if(!sw->classname) swin_setClassname(obj, SWIN_API_STR_NEW(SWIN_T_CNAME,lstrlen(SWIN_T_CNAME)));
        sw->hWnd= CreateWindowEx( exstyle,
                sw->classname,sw->caption,sw->dwStyle,
                sw->x,sw->y,sw->w,sw->h,
                sw->hParent,
                sw->hEtc,  /* menu */
                (HINSTANCE)sw->hInst,
                NULL);
        if(sw->hWnd==NULL){
                rb_raise(rb_eRuntimeError,"failed to create window(%s:%s)",
                                           sw->classname,sw->caption);
                return Qfalse;
        }

        sw->nthread_id = GetCurrentThreadId();
        if( SWIN_ORIGINALWNDCLASS(sw->classname) ){
                Register_sw(sw->hWnd,sw);
        }
        return obj;
}

static VALUE
swin_close(VALUE obj){
        struct Swin *sw;
    Data_Get_Struct(obj, struct Swin, sw);
        SWIN_WINEXISTCHECK(sw);
/*      unregister_sw(sw->hWnd);   Don't unregister here */
        if(DestroyWindow(sw->hWnd)) return (Qtrue);
        return(Qfalse);
}

static VALUE
swin_show(int argc,VALUE* argv,VALUE obj){
        int style;
        struct Swin *sw;
    Data_Get_Struct(obj, struct Swin, sw);

        SWIN_WINEXISTCHECK(sw);

        style = (argc>0)? NUM2UINT(argv[0]) : SW_SHOW;
        swin_call_threadblocking(sw->nthread_id,2,ShowWindow,
                sw->hWnd, style);
        swin_call_threadblocking(sw->nthread_id,1,UpdateWindow,sw->hWnd);

        return obj;
}


static VALUE
swin_filterAdd(VALUE obj,VALUE evt){
        struct Swin *sw;
        struct SwinFilter* filter;
        unsigned msg=NUM2UINT(evt);

    Data_Get_Struct(obj, struct Swin, sw);

        filter=sw->filter;
        while(filter!=NULL){     /* Don't add registered filter  */
                if(filter->msg==msg) return Qtrue;
                filter=filter->next;
        }

/*      filter=(struct SwinFilter*)malloc(sizeof(struct SwinFilter));*/
        MYMALLOC(filter,sizeof(struct SwinFilter),Qfalse);
        filter->msg=msg;
        filter->next = sw->filter;
        sw->filter = filter;
        return evt;
}

static VALUE
swin_filterDel(VALUE obj, VALUE evt){
        struct Swin *sw;
        struct SwinFilter *filter, *prevfilter;
        unsigned msg;

        msg=NUM2UINT(evt);
    Data_Get_Struct(obj, struct Swin, sw);

        if(sw->filter==NULL) return Qfalse;

        filter=sw->filter;
        if(filter->msg == msg){
                sw->filter=filter->next;    /* cut */
                free(filter);
                return Qtrue;
        }

        prevfilter=filter;
        filter=prevfilter->next;

        while(filter!=NULL){
                if(filter->msg == msg){
                        prevfilter->next = filter->next; /* cut */
                        free(filter);
                        return Qtrue;
                }
                filter = filter->next;
        }
        return Qfalse;
}

static VALUE
swin_filterclear(VALUE obj){
        struct Swin *sw;
    Data_Get_Struct(obj, struct Swin, sw);
        freeall_filter(sw);
        return Qnil;
}

static VALUE
swin_eachfilter(VALUE obj){
        struct Swin *sw;
        struct SwinFilter* filter;
        int n=0;

    Data_Get_Struct(obj, struct Swin, sw);

        filter=sw->filter;
        while(filter!=NULL){
                rb_yield(INT2NUM(filter->msg));
                filter=filter->next;
                n++;
        }

        return INT2NUM(n);
}

static VALUE
swin_msgcaptured(VALUE obj){
        VALUE robj;
        struct Swin *sw;
        struct SwinFilter* filter;

        robj = rb_ary_new2(8); /* 8: decided by my inspiration */

        Data_Get_Struct(obj, struct Swin, sw);
        filter=sw->filter;
        while(filter!=NULL){
                rb_ary_push(robj,INT2NUM(filter->msg));
                filter=filter->next;
        }

        return robj;
}



static VALUE
swin_m_messageBox(int argc,VALUE* argv, VALUE obj){
        struct Swin *sw;
        Data_Get_Struct(obj,struct Swin,sw);
        return swin_t_messageBox(argc,argv,sw->hWnd);
}


static VALUE
swin_movewindow(VALUE obj,VALUE x, VALUE y, VALUE w, VALUE h){
        BOOL r;
        struct Swin *sw;
    Data_Get_Struct(obj, struct Swin, sw);
        if(sw->hWnd!=NULL){
            r = swin_call_threadblocking(sw->nthread_id,6,MoveWindow,
                        sw->hWnd, 
                        NUM2INT(x), NUM2INT(y), NUM2INT(w), NUM2INT(h),
                        TRUE);
        } else {
                r=TRUE;
                sw->x=NUM2INT(x); sw->y=NUM2INT(y);
                sw->w=NUM2INT(w); sw->h=NUM2INT(h);
        }

        if(r) return Qtrue;
        return Qfalse;
}

typedef LRESULT (WINAPI *MSGPROC)(HWND,UINT,WPARAM,LPARAM);

VALUE
swin_messagingcore(
                        MSGPROC messaging,
                        VALUE obj, VALUE msg,VALUE wparam,VALUE lparam){

        struct Swin *sw;
    unsigned long r,lp,wp;

#ifdef SWIN_FOR_RUBY19_M17N
  VALUE lpnew,wpnew;
  lpnew = Qnil; wpnew = Qnil;
#endif
    Data_Get_Struct(obj, struct Swin, sw);
        SWIN_WINEXISTCHECK(sw);

        /* Thanks to Rich Kilmer for adding this WPARAM switch */
        switch(TYPE(wparam)) {
                case T_FIXNUM:
                case T_BIGNUM:   /* Thanks to Shigitani-san */
                  wp=NUM2UINT(wparam);
                  break;
                case T_STRING:
#ifdef SWIN_FOR_RUBY19_M17N
                  if(rb_enc_get_index(wparam)) {
                     wpnew = SWIN_API_STR(wparam);
                     wp=(unsigned long)RSTRING_PTR(wpnew);
                  } else {
                     wp=(unsigned long)RSTRING_PTR(wparam);
                  }
#else
                  wp=(unsigned long)RSTRING_PTR(wparam);
#endif
                  break;
                default:
                  rb_warn("wparam is neither Integer nor String");
                  wp=(unsigned long)StringValuePtr(wparam);
                  break;
        }
        switch(TYPE(lparam)){
                case T_FIXNUM:
                case T_BIGNUM:   /* Thanks to Shigitani-san */
                  lp=NUM2UINT(lparam);
                  break;
                case T_STRING:
#ifdef SWIN_FOR_RUBY19_M17N
                  if(rb_enc_get_index(lparam)) {
                      lpnew = SWIN_API_STR(lparam);
                      lp=(unsigned long)RSTRING_PTR(lpnew);
                   } else {
                      lp=(unsigned long)RSTRING_PTR(lparam);
                   }
#else
                  lp=(unsigned long)RSTRING_PTR(lparam);
#endif
                  break;
                default:
                  rb_warn("lparam is neither Integer nor String");
                  lp=(unsigned long)StringValuePtr(wparam);
                  break;
        }
        
        r=swin_call_threadblocking(sw->nthread_id,4,(FARPROC)messaging,
                     sw->hWnd, NUM2UINT(msg), wp, lp);
#ifdef SWIN_FOR_RUBY19_M17N
        if(wpnew != Qnil) {
          rb_str_shared_replace(wparam,SWIN_OUTAPI_STR_NEW2((TCHAR*)wp));
        }
        if(lpnew != Qnil) {
          rb_str_shared_replace(lparam,SWIN_OUTAPI_STR_NEW2((TCHAR*)lp));
        }
#endif
        return INT2NUM(r);
}

static VALUE
swin_postmessage(VALUE obj,VALUE msg,VALUE wparam,VALUE lparam){
        return swin_messagingcore((MSGPROC)PostMessage,obj,msg,wparam,lparam);
}

static VALUE
swin_sendmessage(VALUE obj,VALUE msg,VALUE wparam,VALUE lparam){
        return swin_messagingcore((MSGPROC)SendMessage,obj,msg,wparam,lparam);
}

static VALUE
swin_setenable(VALUE obj, VALUE f){
        BOOL r;   /* This has 2 meanings. */
        struct Swin *sw;
        unsigned long style;

    Data_Get_Struct(obj, struct Swin, sw);


        if(f==Qfalse){
                style = RESET_BIT(sw->dwStyle,WS_DISABLED);
                r=FALSE;
        } else {
                style = SET_BIT(sw->dwStyle,WS_DISABLED);
                r=TRUE;
    }

        sw->enabled=r;
        if(sw->hWnd!=NULL){
            r = swin_call_threadblocking(sw->nthread_id,2,EnableWindow,
                    sw->hWnd, r);
        } else {
                r=TRUE;
        }


        if(r){
                sw->dwStyle=style;
                return Qtrue;
        }
        return Qfalse;
}

static VALUE
swin_setenable_true(VALUE obj){
        return swin_setenable(obj,Qtrue);
}
static VALUE
swin_setenable_false(VALUE obj){
        return swin_setenable(obj,Qfalse);
}



static VALUE
swin_getenable(VALUE obj){
        struct Swin *sw;
        BOOL r;

    Data_Get_Struct(obj, struct Swin, sw);

        if(sw->hWnd!=NULL){
            r= swin_call_threadblocking(sw->nthread_id,1,IsWindowEnabled,
                        sw->hWnd);
        } else {
            r = sw->enabled;
        }
        if(r) return Qtrue;

        return Qfalse;
}

static VALUE
swin_setvisible(VALUE obj,VALUE f){
        struct Swin *sw;
    Data_Get_Struct(obj, struct Swin, sw);
        SWIN_WINEXISTCHECK(sw);
        if(f==Qnil || f==Qfalse){
            swin_call_threadblocking(sw->nthread_id,2,ShowWindow,
                    sw->hWnd,SW_HIDE);
        } else {
            swin_call_threadblocking(sw->nthread_id,2,ShowWindow,
                    sw->hWnd,SW_SHOWNA);
        }
        return f;
}
static VALUE
swin_getvisible(VALUE obj){
        struct Swin *sw;
    Data_Get_Struct(obj, struct Swin, sw);

        if(sw->hWnd!=NULL){
                if(swin_call_threadblocking(sw->nthread_id,1,
                         IsWindowVisible,sw->hWnd)) return Qtrue;
                return Qfalse;
        }
        return Qfalse;
}

static VALUE
swin_hookwndproc(VALUE obj){
        struct Swin *sw;

    Data_Get_Struct(obj, struct Swin, sw);
        SWIN_WINEXISTCHECK(sw);

        if(!sw->OrigWndProc){
                sw->OrigWndProc=(void*)GetWindowLong(sw->hWnd,GWL_WNDPROC);
                SetWindowLong(sw->hWnd,GWL_WNDPROC,(long)WndProc);
                Register_sw(sw->hWnd,sw);
        } else {
                rb_warn("WndProc Hooked Multi");
/*              rb_raise(rb_eRuntimeError,"WndProc Hooked Multi Error");  */
        }

        return obj;
}

static VALUE
swin_unhookwndproc(VALUE obj){
        struct Swin *sw;
    Data_Get_Struct(obj, struct Swin, sw);

        if(sw->hWnd){
                SetWindowLong(sw->hWnd,GWL_WNDPROC,(long)(sw->OrigWndProc));
                Unregister_sw(sw->hWnd);
                sw->OrigWndProc=NULL;
        } else {
                rb_raise(eSwinWindowNotExistError,"UnHook WndProc Error");
        }

        return obj;

}

static VALUE
swin_hookedq(VALUE obj){
        struct Swin *sw;
        long wproc;

    Data_Get_Struct(obj, struct Swin, sw);

        if(sw->hWnd){
                if(sw->OrigWndProc) return Qtrue;

                wproc=GetWindowLong(sw->hWnd,GWL_WNDPROC);
                if(wproc==(long)WndProc) return Qtrue;
        }
        return Qfalse;
}


static VALUE
swin_getclientrect(VALUE obj){
        VALUE robj;
        RECT rc;
        struct Swin *sw;

    Data_Get_Struct(obj, struct Swin, sw);
        SWIN_WINEXISTCHECK(sw);

        GetClientRect(sw->hWnd, &rc);

        robj=rb_ary_new();
        rb_ary_push(robj,INT2NUM(rc.left));
        rb_ary_push(robj,INT2NUM(rc.top));
        rb_ary_push(robj,INT2NUM(rc.right-rc.left));
        rb_ary_push(robj,INT2NUM(rc.bottom-rc.top));
        return robj;
}
static VALUE
swin_getwindowrect(VALUE obj){
        VALUE robj;
        RECT rc;
        struct Swin *sw;

    Data_Get_Struct(obj, struct Swin, sw);
        SWIN_WINEXISTCHECK(sw);

        GetWindowRect(sw->hWnd, &rc);

        robj=rb_ary_new();
        rb_ary_push(robj,INT2NUM(rc.left));
        rb_ary_push(robj,INT2NUM(rc.top));
        rb_ary_push(robj,INT2NUM(rc.right-rc.left));
        rb_ary_push(robj,INT2NUM(rc.bottom-rc.top));
        return robj;
}

static VALUE
swin_msghandler(VALUE obj,VALUE msg){
        return Qnil;   /* default: do nothing */
}

static VALUE
swin_hwndalive(VALUE obj){
        struct Swin *sw;
    Data_Get_Struct(obj, struct Swin, sw);

        if(sw->hWnd) return Qtrue;
        return Qfalse;
}

static VALUE    /* This name doesn't tell its function properly */
swin_invalidateALL(int argc,VALUE* argv,VALUE obj){
        struct Swin *sw;
        BOOL bErase;

        bErase = TRUE;

    Data_Get_Struct(obj, struct Swin, sw);
        SWIN_WINEXISTCHECK(sw);

        if(argc>0){
                if(argv[0]==Qfalse || argv[0]==Qnil) bErase=FALSE;
        }

        InvalidateRect(sw->hWnd,NULL,bErase);
        swin_call_threadblocking(sw->nthread_id,1,UpdateWindow,sw->hWnd);
        return obj;
}


static VALUE
swin_setCapture(VALUE obj){
        struct Swin *sw;
    Data_Get_Struct(obj, struct Swin, sw);
        SWIN_WINEXISTCHECK(sw);

        if(sw->hWnd) SetCapture(sw->hWnd);
        return obj;
}
static VALUE
swin_releaseCapture(VALUE obj){
        struct Swin *sw;
    Data_Get_Struct(obj, struct Swin, sw);
        SWIN_WINEXISTCHECK(sw);

        if(sw->hWnd) ReleaseCapture();
        return obj;
}

static VALUE
swin_getdwStyle(VALUE obj){
        struct Swin *sw;
    Data_Get_Struct(obj, struct Swin, sw);

        if(sw->hWnd) sw->dwStyle=GetWindowLong(sw->hWnd,GWL_STYLE);

        return UINT2NUM(sw->dwStyle);
}

static VALUE
swin_setdwStyle(VALUE obj,VALUE style){
        struct Swin *sw;

    Data_Get_Struct(obj, struct Swin, sw);
        sw->dwStyle=NUM2ULONG(style);

        if(sw->hWnd){
            SetWindowLong(sw->hWnd,GWL_STYLE,sw->dwStyle);
            SetWindowPos(sw->hWnd,NULL,0,0,0,0,
                         SWP_NOMOVE | SWP_NOSIZE |
                         SWP_NOZORDER | SWP_FRAMECHANGED);
        }

        return style;
}

static VALUE
swin_getExStyle(VALUE obj){
        struct Swin *sw;
    Data_Get_Struct(obj, struct Swin, sw);

        if(sw->hWnd) sw->dwExStyle=GetWindowLong(sw->hWnd,GWL_EXSTYLE);

        return UINT2NUM(sw->dwExStyle);
}

static VALUE
swin_setExStyle(VALUE obj,VALUE exstyle){
        struct Swin *sw;

    Data_Get_Struct(obj, struct Swin, sw);
        sw->dwExStyle=NUM2ULONG(exstyle);

        if(sw->hWnd){
            SetWindowLong(sw->hWnd,GWL_EXSTYLE,sw->dwExStyle);
            swin_call_threadblocking(sw->nthread_id,7,
                    SetWindowPos,sw->hWnd,NULL,0,0,0,0,
                         SWP_NOMOVE | SWP_NOSIZE |
                         SWP_NOZORDER | SWP_FRAMECHANGED);
        }

        return exstyle;
}

static VALUE
swin_settop(int argc,VALUE* argv,VALUE obj){
        struct Swin *sw;
        struct Swin *tmpsw;
        HWND f;
        Data_Get_Struct(obj, struct Swin, sw);
        SWIN_WINEXISTCHECK(sw);

        f = HWND_TOP;
        if(argc>0){
                switch(TYPE(argv[0])){
                        case T_FALSE:
                                f = HWND_BOTTOM;
                                break;
                        case T_TRUE:
                                f = HWND_TOP;
                                break;
                        case T_NIL:
                                f = HWND_NOTOPMOST;
                                break;
                        case T_DATA:
                                Data_Get_Struct(obj, struct Swin, tmpsw);
                                f = tmpsw->hWnd;
                                break;
                        case T_FIXNUM:
                        case T_BIGNUM:
                                f = (HWND)NUM2UINT(argv[0]);
                                break;
                        default:
                                rb_raise(rb_eArgError,"illegal argument type");
                                return Qfalse;
                                break;
                }
        }
        swin_call_threadblocking(sw->nthread_id,7,
            SetWindowPos,sw->hWnd,f,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
        return obj;
}


static VALUE
swin_getFocus(VALUE obj){
        struct Swin *sw;
        HWND hwnd = GetFocus();

        Data_Get_Struct(obj, struct Swin, sw);
        return (hwnd==sw->hWnd)? Qtrue : Qfalse;
}

static VALUE
swin_setFocus(VALUE obj){
        struct Swin *sw;
        Data_Get_Struct(obj, struct Swin, sw);
        SWIN_WINEXISTCHECK(sw);

        SetFocus(sw->hWnd);
        return obj;
}

VALUE
swin_prop(VALUE obj,char* name){
        VALUE r;
        ID propid;

        propid = rb_intern(name);
        if(rb_ivar_defined(obj,propid)){
                r = rb_ivar_get(obj,propid);
        } else {
                r = rb_hash_new();
                rb_iv_set(obj,name,r);
        }
        return r;
}

static VALUE
swin_getprop(VALUE obj){
        return swin_prop(obj,"properties");
}

static VALUE
swin_DCYield(int argc,VALUE* argv,VALUE obj){
        struct Swin *sw;
        long ahdc;
        BOOL alc;

        Data_Get_Struct(obj, struct Swin, sw);

        ahdc= (argc>0)? NUM2UINT(argv[0]) : 0;
        alc = FALSE;

        if(!rb_iterator_p()){
                alc = !sw->painting;
        } else {
                if(!sw->painting){
                        if(ahdc==0){
                                sw->gs.hdc = sw->ps.hdc = GetDC(sw->hWnd);

                        }else{
                                sw->ps.hdc=(HDC)ahdc;
                        }
                        alc=TRUE;
                        sw->ps.rcPaint.left=-1;
                        sw->ps.rcPaint.top=-1;
                        sw->ps.rcPaint.bottom=-1;
                        sw->ps.rcPaint.right=-1;
                        sw->painting++;
                }

                if( threadcritical){ /* begin critical block */
                }

                rb_yield(UINT2NUM((unsigned)sw->ps.hdc));
                if(0){/* end critical block */
                }

                if(alc){
                        sw->painting--;
                        if(ahdc==0){
                                swingdi_release(&(sw->gs));
                                ReleaseDC(sw->hWnd,sw->ps.hdc);
                        }
                }
        }
        return (!alc)? Qtrue : Qfalse;
}

static VALUE
swin_getpsrect(VALUE obj){
        VALUE robj;
        struct Swin *sw;
        Data_Get_Struct(obj, struct Swin, sw);
        SWIN_WINEXISTCHECK(sw);
        SWIN_PAINTINGCHECK(sw);

        robj=rb_ary_new();
        rb_ary_push(robj,INT2NUM(sw->ps.rcPaint.left));
        rb_ary_push(robj,INT2NUM(sw->ps.rcPaint.top));
        rb_ary_push(robj,INT2NUM(sw->ps.rcPaint.right));
        rb_ary_push(robj,INT2NUM(sw->ps.rcPaint.bottom));
        return robj;
}

#define DefineSwinGetIntParam(x) static VALUE\
        swin_get##x(VALUE obj){ \
        struct Swin *sw; \
    Data_Get_Struct(obj, struct Swin, sw);\
        return INT2NUM((int)sw->x); \
        }

DefineSwinGetIntParam(hWnd)
DefineSwinGetIntParam(hParent)
DefineSwinGetIntParam(hEtc)

static VALUE
swin_sethEtc(VALUE obj,VALUE val){
  struct Swin *sw;
  Data_Get_Struct(obj, struct Swin, sw);
  sw->hEtc = (HANDLE)NUM2INT(val);
  return obj;
}

#define SwinGetWinClientXY(member) \
static VALUE \
swin_get##member(VALUE obj) {\
  struct Swin *sw;\
  RECT rc;\
  POINT pt;\
  int ret;\
  Data_Get_Struct(obj, struct Swin, sw);\
  if (sw->hWnd == 0) ret = sw->member;\
  else if (!GetWindowRect(sw->hWnd, &rc)) ret = sw->member;\
  else { \
    pt.x = rc.left; pt.y = rc.top;\
    ScreenToClient(sw->hParent, &pt);\
    ret = pt.member; \
  } \
  return INT2NUM(ret); \
}

#define SwinGetWinWH(swinmember,rcmember1,rcmember2) \
static VALUE \
swin_get##swinmember(VALUE obj) {\
  struct Swin *sw;\
  RECT rc;\
  int ret;\
  Data_Get_Struct(obj, struct Swin, sw);\
  if (sw->hWnd == 0) ret = sw->swinmember;\
  else if (!GetWindowRect(sw->hWnd, &rc)) ret = sw->swinmember;\
  else { \
    ret = rc.rcmember1 - rc.rcmember2; \
  } \
  return INT2NUM(ret); \
}

SwinGetWinClientXY(x)  // swin_getx()
SwinGetWinClientXY(y)  // swin_gety()

SwinGetWinWH(w,right,left) //swin_getw()
SwinGetWinWH(h,bottom,top) //swin_geth()

#define SwinGetSwinWindowPos \
  struct Swin *sw; \
  RECT rc;\
  POINT pt;\
  int x,y,w,h;\
  Data_Get_Struct(obj, struct Swin, sw);\
  if (!sw->hWnd || !GetWindowRect(sw->hWnd, &rc))\
    {\
      rb_raise(eSwinWindowNotExistError,"Window Not Exist");\
      return Qfalse;\
    }else{\
      pt.x = rc.left; pt.y = rc.top;\
      ScreenToClient(sw->hParent, &pt);\
      x = pt.x; y = pt.y; w = rc.right - rc.left; h = rc.bottom - rc.top;\
    }\

static VALUE
swin_setx(VALUE obj, VALUE val){
  SwinGetSwinWindowPos;
  MoveWindow(sw->hWnd,NUM2INT(val),y,w,h,TRUE);
  return val;
}

static VALUE
swin_sety(VALUE obj, VALUE val){
  SwinGetSwinWindowPos;
  MoveWindow(sw->hWnd,x,NUM2INT(val),w,h,TRUE);
  return val;
}

static VALUE
swin_setw(VALUE obj, VALUE val){
  SwinGetSwinWindowPos;
  MoveWindow(sw->hWnd,x,y,NUM2INT(val),h,TRUE);
  return val;
}

static VALUE
swin_seth(VALUE obj, VALUE val){
  SwinGetSwinWindowPos;
  MoveWindow(sw->hWnd,x,y,w,NUM2INT(val),TRUE);
  return val;
}

void Init_swinWindow(){
        msghandler_id  = rb_intern(SWIN_MSGHANDLERNAME);

        cSwin = rb_define_class_under(mSwin,"Window",rb_cObject);

        rb_define_singleton_method(cSwin,"new",swin_avoid_new,-1);
        rb_define_method(cSwin,"create",swin_create,-1);
        rb_define_method(cSwin,"close",swin_close,0);
        rb_define_method(cSwin,"delete",swin_close,0);
        rb_define_method(cSwin,"show",swin_show,-1);
        rb_define_method(cSwin,"top",swin_settop,-1);

        rb_define_method(cSwin,"addEvent",swin_filterAdd,1);
        rb_define_method(cSwin,"delEvent",swin_filterDel,1);
        rb_define_method(cSwin,"clearEventFilter",swin_filterclear,0);
        rb_define_method(cSwin,"eachFilter",swin_eachfilter,0);
        rb_define_method(cSwin,"capturedEvents",swin_msgcaptured,0);

        rb_define_method(cSwin,"hWnd",swin_gethWnd,0);
        rb_define_method(cSwin,"handle",swin_gethWnd,0);
        rb_define_method(cSwin,"hParent",swin_gethParent,0);
        rb_define_method(cSwin,"etc=",swin_sethEtc,1);
        rb_define_method(cSwin,"etc",swin_gethEtc,0);

        rb_define_method(cSwin,"caption",swin_getCaption,0);
        rb_define_method(cSwin,"caption=",swin_setCaption,1);
        rb_define_method(cSwin,"x",swin_getx,0);
        rb_define_method(cSwin,"y",swin_gety,0);
        rb_define_method(cSwin,"w",swin_getw,0);
        rb_define_method(cSwin,"h",swin_geth,0);

        rb_define_method(cSwin,"x=",swin_setx,1);
        rb_define_method(cSwin,"y=",swin_sety,1);
        rb_define_method(cSwin,"w=",swin_setw,1);
        rb_define_method(cSwin,"h=",swin_seth,1);

        rb_define_method(cSwin,"move",swin_movewindow,4);
        rb_define_method(cSwin,"sendMessage",swin_sendmessage,3);
        rb_define_method(cSwin,"postMessage",swin_postmessage,3);

        rb_define_method(cSwin,"style=",swin_setdwStyle,1);
        rb_define_method(cSwin,"style",swin_getdwStyle,0);
        rb_define_method(cSwin,"classname=",swin_setClassname,1);
        rb_define_method(cSwin,"classname",swin_getClassname,0);

        rb_define_method(cSwin,"enabled=",swin_setenable,1);
        rb_define_method(cSwin,"enabled?",swin_getenable,0);
        rb_define_method(cSwin,"enabled",swin_getenable,0);
        rb_define_method(cSwin,"enable",swin_setenable_true,0);
        rb_define_method(cSwin,"disable",swin_setenable_false,0);

        rb_define_method(cSwin,"clientrect",swin_getclientrect,0);
        rb_define_method(cSwin,"windowrect",swin_getwindowrect,0);

        rb_define_method(cSwin,"messageBox",swin_m_messageBox,-1);
        rb_define_method(cSwin,"hookwndproc",swin_hookwndproc,0);
        rb_define_method(cSwin,"unhookwndproc",swin_unhookwndproc,0);
        rb_define_method(cSwin,"hookedwndproc?",swin_hookedq,0);
        rb_define_method(cSwin,SWIN_MSGHANDLERNAME,swin_msghandler,1);
        rb_define_method(cSwin,"alive?",swin_hwndalive,0);
        rb_define_method(cSwin,"refresh",swin_invalidateALL,-1);

        rb_define_method(cSwin,"setCapture",swin_setCapture,0);
        rb_define_method(cSwin,"releaseCapture",swin_releaseCapture,0);

        rb_define_method(cSwin,"focused?",swin_getFocus,0);
        rb_define_method(cSwin,"focus",swin_setFocus,0);

        rb_define_method(cSwin,"visible?",swin_getvisible,0);
        rb_define_method(cSwin,"visible=",swin_setvisible,1);
        rb_define_method(cSwin,"exstyle",swin_getExStyle,0);
        rb_define_method(cSwin,"exstyle=",swin_setExStyle,1);

        rb_define_method(cSwin,"properties",swin_getprop,0);
        rb_define_method(cSwin,"dopaint",swin_DCYield,-1);
        rb_define_method(cSwin,"psrect",swin_getpsrect,0);
}
