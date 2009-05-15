require 'swin'

module WMsg
  WS_CHILD          = 0x40000000
  WS_VISIBLECHILD   = 0x50000000
  WS_BORDER         = 0x00800000
  WS_OVERLAPPED     = 0
  WM_COMMAND        = 0x00000111
  WM_RBUTTONUP      = 517
  WM_NOTIFY         = 0x004E

  PBM_SETRANGE32    =0x406
  PBM_SETPOS        =0x402
  PGM_STEP          =0x405

  LVM_SETBKCOLOR    =0x1001
  LVM_SETCOLUMNA    =0x1000+26
  LVM_INSERTCOLUMNA =0x1000+27
  LVM_SETITEMA      =0x1006
  LVM_INSERTITEMA   =0x1007

  TVM_INSERTITEMA   =0x1100
end

TVI_ROOT=0xffff0000
TVI_LAST=0xffff0002



RFactory=SWin::LWFactory.new SWin::Application.hInstance

$wins=[]

class TEST < SWin::Window
end


$wins.push mw=RFactory.newwindow(nil,TEST)


mw.caption="Caption2"
mw.move(300,200,400,400)
mw.create.show
mw.move(300,200,400,400)

#usecomctl

$wins.push $text=RFactory.newwindow(mw)
$text.caption="Notify Messages"
$text.classname="STATIC"
$text.style= WMsg::WS_VISIBLECHILD
$text.move 10,10,300,40
$text.etc=$wins.size-1
$text.create.show


$wins.push $cb=RFactory.newwindow(mw)
$cb.caption="updwn"
$cb.classname="msctls_updown32"
$cb.style= WMsg::WS_VISIBLECHILD
$cb.move 80,50,50,40
$cb.etc=$wins.size-1
$cb.create.show

$wins.push $pg=RFactory.newwindow(mw)
$pg.caption="progress"
$pg.classname="msctls_progress32"
$pg.style= WMsg::WS_VISIBLECHILD | 1
$pg.move 180,50,100,20
$pg.etc=$wins.size-1
$pg.create.show

$pg.sendMessage WMsg::PBM_SETRANGE32,0x0,0x7F #  w=low,l=high
$pg.sendMessage WMsg::PBM_SETPOS,0x10,0       #  w=val


$wins.push $lv=RFactory.newwindow(mw)
$lv.caption="listview"
$lv.classname="SysListView32"
$lv.style= WMsg::WS_VISIBLECHILD | WMsg::WS_BORDER | 1   # 1:LVS_REPORT
$lv.move 80,100,100,100
$lv.etc=$wins.size-1
$lv.create.show

$lv.sendMessage WMsg::LVM_SETBKCOLOR,0,0x80F0F0   


LVITEM="UIIUUPIIL"
LVCOLUMN="UIIPIIII"

lvcol=SWin::Application.arg2cstructStr(LVCOLUMN,0xf,0,50,"‚Ó[",128,0,0,0)
$lv.sendMessage WMsg::LVM_INSERTCOLUMNA,0,lvcol
lvcol=SWin::Application.arg2cstructStr(LVCOLUMN,0xf,1,50,"‚Î[",128,1,0,0)
$lv.sendMessage WMsg::LVM_INSERTCOLUMNA,1,lvcol


lvitem=SWin::Application.arg2cstructStr(LVITEM,1,0,0,0,0,"‚Ù‚°",128,0,0)
$lv.sendMessage WMsg::LVM_INSERTITEMA,0,lvitem

lvitem=SWin::Application.arg2cstructStr(LVITEM,1,0,1,0,0,"hoge",128,0,0)
$lv.sendMessage WMsg::LVM_SETITEMA,0,lvitem

lvitem=SWin::Application.arg2cstructStr(LVITEM,1,1,0,0,0,"‚à‚°",128,0,0)
$lv.sendMessage WMsg::LVM_INSERTITEMA,0,lvitem

lvitem=SWin::Application.arg2cstructStr(LVITEM,1,1,1,0,0,"moge",128,0,0)
$lv.sendMessage WMsg::LVM_SETITEMA,0,lvitem


$wins.push $tv=RFactory.newwindow(mw)
$tv.caption="treeview"
$tv.classname="SysTreeView32"
$tv.style= WMsg::WS_VISIBLECHILD | WMsg::WS_BORDER | 0xf #HASBUTTONLINELINESEDIT
$tv.move 200,100,150,100
$tv.etc=$wins.size-1
$tv.create.show

TREEINSERTITEM="UUUUUUPIIIIL"

treeitem=SWin::Application.arg2cstructStr(TREEINSERTITEM,TVI_ROOT, TVI_LAST,
                        1,0,0,0,"‚³‚¢‚Å‚²‚´‚¢‚Ü‚·‚©",128,0,0,0,0)
p=$tv.sendMessage WMsg::TVM_INSERTITEMA+0,0,treeitem
treeitem=SWin::Application.arg2cstructStr(TREEINSERTITEM,p, TVI_LAST,
                        1,0,0,0,"‚³‚æ‚¤",128,0,0,0,0)
$tv.sendMessage WMsg::TVM_INSERTITEMA,0,treeitem
treeitem=SWin::Application.arg2cstructStr(TREEINSERTITEM,p, TVI_LAST,
                        1,0,0,0,"‚»‚¤‚Å‚Í‚²‚´‚ç‚Ê",128,0,0,0,0)
$tv.sendMessage WMsg::TVM_INSERTITEMA,0,treeitem
treeitem=SWin::Application.arg2cstructStr(TREEINSERTITEM,TVI_ROOT, TVI_LAST,
                        1,0,0,0,"‚¢‚ç‚Á‚µ‚á‚¢‚Ü‚¹",128,0,0,0,0)
p=$tv.sendMessage WMsg::TVM_INSERTITEMA,0,treeitem  #TVM_INSERTITEMA
treeitem=SWin::Application.arg2cstructStr(TREEINSERTITEM,p, TVI_LAST,
                        1,0,0,0,"“÷‚Ü‚ñ‚­‚¾‚³‚¢",128,0,0,0,0)
$tv.sendMessage WMsg::TVM_INSERTITEMA,0,treeitem
treeitem=SWin::Application.arg2cstructStr(TREEINSERTITEM,p, TVI_LAST,
                        1,0,0,0,"‚¨’Þ‚è‘«‚è‚Ü‚¹‚ñ‚¯‚Ç",128,0,0,0,0)
$tv.sendMessage WMsg::TVM_INSERTITEMA,0,treeitem


mw.addEvent WMsg::WM_NOTIFY

def mw.msghandler(msg)
  $pg.sendMessage WMsg::PGM_STEP,0,0

  arg= SWin::Application.cstruct2array(msg.lParam,"UUI")
  msgtitle=
    case arg[2]
    when 0xffffffff
      "OUTOFMEMORY"
    when 0xffffffff-1
      "CLICK"
    when 0xffffffff-2
      "RETURN"
    when 0xffffffff-3
      "DBLCLICK"
    when 0xffffffff-4
      "RCLICK"
    when 0xffffffff-5
      "RDBLCLK"
    when 0xffffffff-6
      "SETFOCUS"
    when 0xffffffff-7
      "KILLFOCUS"
    else
      "I don't know.."
    end
  $text.caption="[#{$wins[msg.wParam].caption}]  : msg=#{msgtitle}" 
end

SWin::Application.messageloop

exit
