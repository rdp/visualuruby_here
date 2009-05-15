/*
###################################
#
# swinmenu.c
# Programmed by nyasu <nyasu@osk.3web.ne.jp>
# Copyright 1999-2005  Nishikawa,Yasuhiro
#
# More information at http://vruby.sourceforge.net/index.html
#
###################################
*/


#include "swin.h"

VALUE cSwinMenu;

void
shutdown_menu(struct SwinMenu* sm){
        if(sm->f_delete) DestroyMenu(sm->hmenu);
        free(sm);
}


static VALUE
swinfac_newmenu(VALUE klass){
        struct SwinMenu* sm;
        VALUE robj;

    robj = Data_Make_Struct(cSwinMenu, struct SwinMenu, 0, shutdown_menu, sm);
    sm->hmenu= CreateMenu();
        if(!sm->hmenu){
                rb_raise(rb_eRuntimeError,"failed to create menu");
                return Qfalse;
        }
    sm->f_delete=TRUE;
    sm->hwnd=NULL;
    return robj;
}
static VALUE
swinfac_newpopup(VALUE klass){
        struct SwinMenu* sm;
        VALUE robj;

    robj = Data_Make_Struct(cSwinMenu, struct SwinMenu, 0, shutdown_menu, sm);
    sm->hmenu= CreatePopupMenu();
    sm->f_delete=TRUE;
    sm->hwnd=NULL;
    return robj;
}

static VALUE  /* menustr,id,flag */
swinmenu_append(int argc, VALUE* argv, VALUE obj){
        struct SwinMenu* sm;
        struct SwinMenu* sp;
        VALUE text;
        UINT flag;
        BOOL r;
        UINT id;

        Data_Get_Struct(obj, struct SwinMenu, sm);

        flag = MF_ENABLED;
        if(argc>2){
                if(argv[2]!=Qnil){
                        flag = NUM2UINT(argv[2]);
                }
        }

        if(rb_obj_is_kind_of(argv[1], cSwinMenu)){
                flag |= MF_POPUP;
                Data_Get_Struct(argv[1], struct SwinMenu, sp);
                sp->f_delete=FALSE;
                sp->hwnd=sm->hwnd;
                id=(UINT)sp->hmenu;
        } else {
                flag |= MF_STRING;
                id=NUM2UINT(argv[1]);
        }

        text = StringValue(argv[0]);

        r=AppendMenu(sm->hmenu,flag,id,SWIN_API_STR_PTR(text));
        if(sm->hwnd) DrawMenuBar(sm->hwnd);

        if(r) return Qtrue;
        return Qfalse;
}

static VALUE    /* menustr,insertptr,id,flag */
swinmenu_insert(int argc, VALUE* argv, VALUE obj){
        struct SwinMenu* sm;
        struct SwinMenu* sp;
        VALUE text;
        UINT flag;
        UINT id;
        BOOL r;

        Data_Get_Struct(obj, struct SwinMenu, sm);

        flag = MF_ENABLED;
        if(argc>3){
                if(argv[3]!=Qnil){
                        flag = NUM2UINT(argv[3]);
                }
        }

        if(rb_obj_is_kind_of(argv[1], cSwinMenu)){
                flag |= MF_POPUP;
                Data_Get_Struct(argv[1], struct SwinMenu, sp);
                sp->f_delete=FALSE;
                sp->hwnd=sm->hwnd;
                id=(UINT)sp->hmenu;
        } else {
                flag |= MF_STRING;
                id=NUM2UINT(argv[1]);
        }

        text = SWIN_API_STR(argv[0]);

        r=InsertMenu(sm->hmenu,NUM2UINT(argv[2]),flag,id,(TCHAR*)RSTRING_PTR(text));
        if(sm->hwnd) DrawMenuBar(sm->hwnd);

        if(r) return Qtrue;
        return Qfalse;
}

static VALUE
swinmenu_delete(VALUE obj, VALUE iid){
        struct SwinMenu* sm;
        struct SwinMenu* sp;
        BOOL r;
        UINT id=0;

        Data_Get_Struct(obj, struct SwinMenu, sm);

        if(rb_obj_is_kind_of(iid, cSwinMenu)){
                Data_Get_Struct(iid, struct SwinMenu, sp);
                id=(UINT)sp->hmenu;
                sp->f_delete=FALSE;
                sp->hmenu=NULL;
        } else {
                id=NUM2UINT(iid);
        }
        r=DeleteMenu(sm->hmenu,id, MF_BYCOMMAND);

        if(sm->hwnd) DrawMenuBar(sm->hwnd);
        if(r) return Qtrue;
        return Qfalse;
}


static VALUE
swinmenu_setState(VALUE obj,VALUE id, VALUE state){
        struct SwinMenu* sm;
        BOOL r;
        int flag;
        MENUITEMINFO minfo;

        Data_Get_Struct(obj, struct SwinMenu, sm);

        flag=NUM2UINT(state);

        minfo.cbSize = sizeof(minfo);
        minfo.fMask = MIIM_STATE;
        minfo.fState = flag;

        r = SetMenuItemInfo(sm->hmenu,NUM2UINT(id), FALSE, &minfo);

        if(sm->hwnd) DrawMenuBar(sm->hwnd);
        if(r) return Qtrue;
        return Qfalse;
}



static VALUE
swinmenu_getState(VALUE obj,VALUE id){
        struct SwinMenu* sm;
        BOOL r;
        Data_Get_Struct(obj, struct SwinMenu, sm);

        r=GetMenuState(sm->hmenu,NUM2UINT(id), MF_BYCOMMAND);

        if(sm->hwnd) DrawMenuBar(sm->hwnd);
        return INT2NUM(r);
}

static VALUE
swinmenu_check(VALUE obj,VALUE id, VALUE state){
        struct SwinMenu* sm;
        BOOL r;
        int flag;

        Data_Get_Struct(obj, struct SwinMenu, sm);

        flag=MF_CHECKED;
        if(state==Qnil || state==Qfalse){
                flag=MF_UNCHECKED;
        }

        r=CheckMenuItem(sm->hmenu,NUM2UINT(id), flag | MF_BYCOMMAND);

        if(sm->hwnd) DrawMenuBar(sm->hwnd);
        if(r) return Qtrue;
        return Qfalse;
}


static VALUE
swinmenu_getCount(VALUE obj){
        struct SwinMenu* sm;
        BOOL r;
        Data_Get_Struct(obj, struct SwinMenu, sm);

        r=GetMenuItemCount(sm->hmenu);

        return INT2NUM(r);
}

static VALUE
swinmenu_modify(VALUE obj,VALUE id, VALUE text){
        struct SwinMenu* sm;
        UINT flag=0;
        long newitem;
        BOOL r;

        Data_Get_Struct(obj, struct SwinMenu, sm);

        if(TYPE(text)==T_STRING){
                flag=MF_STRING;
                newitem=(long)SWIN_API_STR_PTR(text);
        } else {
/* not impremented for this */
                rb_raise(rb_eNotImpError,"only text is supported now.");
                flag=MF_BITMAP;
                newitem=0;
        }
        r=ModifyMenu(sm->hmenu,NUM2INT(id),MF_BYCOMMAND|flag,NUM2INT(id),(TCHAR*)newitem);

        if(sm->hwnd) DrawMenuBar(sm->hwnd);
        if(r) return Qtrue;
        return Qfalse;
}

static VALUE
swin_setMenu(VALUE obj,VALUE mnu){
        struct Swin* sw;
        struct SwinMenu* sm;
        BOOL r;

        Data_Get_Struct(obj, struct Swin    , sw);
        Data_Get_Struct(mnu, struct SwinMenu, sm);
        SWIN_WINEXISTCHECK(sw);

        r=SetMenu(sw->hWnd,sm->hmenu);
        sm->hwnd=sw->hWnd;
        DrawMenuBar(sm->hwnd);
        if(r) return Qtrue; return Qfalse;
}

static VALUE   /* This function is created by Yoshida-san. Thanks! */
swin_popupMenu(VALUE obj, VALUE mnu, VALUE x, VALUE y) {
        struct Swin* sw;
        struct SwinMenu* sm;
        BOOL r;
        UINT flags = 0;
        RECT* rect = NULL;

        Data_Get_Struct(obj, struct Swin    , sw);
        Data_Get_Struct(mnu, struct SwinMenu, sm);
        SWIN_WINEXISTCHECK(sw);

        r=TrackPopupMenu(sm->hmenu, flags,
                         NUM2INT(x), NUM2INT(y),
                         0, sw->hWnd, rect);
        if(r) return INT2NUM(r); return Qnil;
}


static VALUE
swin_eachmenuid(VALUE obj){
        int i,max;
        struct SwinMenu* sm;
        struct SwinMenu* sm_sub;
        int etc;
        int count=0;
        VALUE sub;

        Data_Get_Struct(obj, struct SwinMenu, sm);
        max = GetMenuItemCount((HMENU) sm->hmenu);
        for(i=0;i<max;i++){
                etc = GetMenuItemID(sm->hmenu, i);
                if((int)etc!=-1){
                        rb_yield(UINT2NUM(etc));
                        count++;
                } else {
                    sub = Data_Make_Struct(cSwinMenu, struct SwinMenu, 0, shutdown_menu, sm_sub);
                        sm_sub->f_delete=FALSE;   /* don't destroy menu with this object */
                        sm_sub->hmenu = GetSubMenu(sm->hmenu,i);
                        sm_sub->hwnd = sm->hwnd;
                        rb_yield(sub);
                        count++;
                }
        }
        return count;
}


static VALUE
swin_gethmenu(VALUE obj){
        struct SwinMenu* sm;
        BOOL r;
        Data_Get_Struct(obj, struct SwinMenu, sm);
        return UINT2NUM((unsigned int)sm->hmenu);
}

void Init_swinMenu(){
        cSwinMenu=rb_define_class_under(mSwin,"Menu",rb_cObject);

        rb_define_method(cSwinFactory,"newmenu",swinfac_newmenu,0);
        rb_define_method(cSwinFactory,"newpopup",swinfac_newpopup,0);
        rb_define_singleton_method(cSwinMenu,"new",swin_avoid_new,-1);

        rb_define_method(cSwinMenu,"append",swinmenu_append,-1);
        rb_define_method(cSwinMenu,"insert",swinmenu_insert,-1);
        rb_define_method(cSwinMenu,"delete",swinmenu_delete,1);
        rb_define_method(cSwinMenu,"setState",swinmenu_setState,2);
        rb_define_method(cSwinMenu,"getState",swinmenu_getState,1);
        rb_define_method(cSwinMenu,"setChecked",swinmenu_check,2);
        rb_define_method(cSwinMenu,"count",swinmenu_getCount,0);
        rb_define_method(cSwinMenu,"modify",swinmenu_modify,2);
        rb_define_method(cSwinMenu,"application",swinaa_application,0);

        rb_define_method(cSwinMenu,"each_item",swin_eachmenuid,0);
        rb_define_method(cSwinMenu,"hmenu",swin_gethmenu,0);
        rb_define_method(cSwinMenu,"handle",swin_gethmenu,0);


        rb_define_method(cSwin,"setMenu",swin_setMenu,1);
        rb_define_method(cSwin,"popupMenu",swin_popupMenu,3);

}
