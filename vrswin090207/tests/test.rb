require 'swin'

module WMsg
  WS_CHILD          = 0x40000000
  WS_VISIBLECHILD   = 0x50000000
  WS_BORDER         = 0x00800000
  WS_OVERLAPPED     = 0
  WM_COMMAND        = 0x00000111
  WM_RBUTTONUP      = 517

  BS_CHECKBOX =       2
  BS_AUTOCHECKBOX =   3
  BS_RADIOBUTTON =    4
  BS_3STATE =         5
  BS_GROUPBOX =       7
  BS_AUTORADIOBUTTON= 9

  LBS_STANDARD =    0x00a00000 | 1
  LB_ADDSTRING =    0x0180
  LB_INSERTSTRING = 0x0181
  LB_SETSEL =       0x0185
  LB_GETSEL =       0x0187
  LB_GETCURSEL =    0x188
  LB_GETTEXT =      0x189
  LB_GETTEXTLEN =   0x18a
  LBN_SELCHANGE=1
  LBN_DBLCLK=2
  
  CBS_STANDARD    = 0x00a00200 | 3
  CB_ADDSTRING    = 0x143
  CB_DELETESTRING = 0x144
  CB_GETCURSEL =    0x147
  CB_GETLBTEXT =    0x148
  CB_GETLBTEXTLEN = 0x149
  CB_INSERTSTRING = 0x14a
  CB_SETCURSEL =    0x14e
  CBN_SELCHANGE =     1
  
end

def LOWORD(lParam)
  return (lParam & 0xffff)
end
def HIWORD(lParam)
  return ( (lParam>>16) & 0xffff)
end
def MAKELPARAM(w1,w2)
  return (w2<<16) | w1
end


RFactory=SWin::LWFactory.new SWin::Application.hInstance

$wins=[]

$wins.push mw=RFactory.newwindow(nil)
mw.caption="Caption2"
mw.move(300,200,400,400)
mw.create.show
mw.move(300,200,400,400)


$wins.push $cb=RFactory.newwindow(mw)
$cb.caption="checkbox is button"
$cb.classname="BUTTON"
$cb.style= WMsg::WS_VISIBLECHILD | WMsg::BS_AUTOCHECKBOX
$cb.move 80,100,150,40
$cb.etc=0012
$cb.create.show

$wins.push $bn=RFactory.newwindow(mw)
$bn.classname="BUTTON"
$bn.caption="ボタン１"
$bn.etc=1234
$bn.style=WMsg::WS_VISIBLECHILD 
$bn.move 80,50,80,40
$bn.create.show

$wins.push $bt=RFactory.newwindow(mw)
$bt.classname="BUTTON"
$bt.caption="終了"
$bt.etc=4321
$bt.style=WMsg::WS_VISIBLECHILD
$bt.move 180,50,80,40
$bt.create.show

$wins.push $ed=RFactory.newwindow(mw)
$ed.classname="EDIT"
$ed.caption="This is  editbox"
$ed.etc=1324
$ed.style=WMsg::WS_VISIBLECHILD | WMsg::WS_BORDER 
$ed.move 40,150,320,40
$ed.create.show

$wins.push $lb=RFactory.newwindow(mw)
$lb.classname="LISTBOX"
$lb.caption="This is  listbox"
$lb.etc=4123
$lb.style=WMsg::WS_VISIBLECHILD | WMsg::LBS_STANDARD 
$lb.move 80,250,120,96
$lb.create.show
$lb.sendMessage WMsg::LB_ADDSTRING,0,"はな"
$lb.sendMessage WMsg::LB_ADDSTRING,1,"もげ"
$lb.sendMessage WMsg::LB_ADDSTRING,2,"ら"
$lb.sendMessage WMsg::LB_INSERTSTRING ,2,"ら?"

$wins.push $cm=RFactory.newwindow(mw)
$cm.classname="COMBOBOX"
$cm.caption="This is  combobox"
$cm.etc=4124
$cm.style=WMsg::WS_VISIBLECHILD | WMsg::CBS_STANDARD 
$cm.move 200,250,120,96

$cm.create.show
$cm.sendMessage 0x155 ,1,0
$cm.sendMessage WMsg::CB_ADDSTRING,0,"への"
$cm.sendMessage WMsg::CB_ADDSTRING,1,"もへ"
$cm.sendMessage WMsg::CB_ADDSTRING,2,"じ"
$cm.sendMessage WMsg::CB_INSERTSTRING ,2,"じ?"
$cm.sendMessage WMsg::CB_SETCURSEL ,1,0




$wins.push $gb=RFactory.newwindow(mw)
$gb.classname="BUTTON"
$gb.caption="This is groupbox"
$gb.etc=4125
$gb.style=WMsg::WS_VISIBLECHILD | WMsg::BS_GROUPBOX 
$gb.move 40,200,300,45
$gb.create.show

$wins.push $cb1=RFactory.newwindow($gb)
$cb1.caption="check1"
$cb1.classname="BUTTON"
$cb1.style= WMsg::WS_VISIBLECHILD | WMsg::BS_AUTORADIOBUTTON
$cb1.move 10,15,150,25
$cb1.etc=0015
$cb1.create
$wins.push $cb2=RFactory.newwindow($gb)
$cb2.caption="check2"
$cb2.classname="BUTTON"
$cb2.style= WMsg::WS_VISIBLECHILD | WMsg::BS_AUTORADIOBUTTON
$cb2.move 160,15,150,25
$cb2.etc=0016
$cb2.create





mw.addEvent WMsg::WM_COMMAND
mw.addEvent WMsg::WM_RBUTTONUP

$ccc=0
def mw.msghandler(msg)
   if msg.msg==WMsg::WM_COMMAND then

     if LOWORD(msg.wParam)==1234 then     # command button
        messageBox "ボタン１が押されました","OK",0
     elsif LOWORD(msg.wParam)==4321 then 
	    close

	 elsif LOWORD(msg.wParam)==4123 then  #list box(single select)
	   if HIWORD(msg.wParam)==WMsg::LBN_SELCHANGE then       #LBN_selchange
	       selct= $lb.sendMessage(WMsg::LB_GETCURSEL,0,0)      #LB_getcursel
	       str=" "*$lb.sendMessage(WMsg::LB_GETTEXTLEN,selct,0) #LB_gettextlen
	       $lb.sendMessage(WMsg::LB_GETTEXT,selct,str)          #LB_getText
		   $ed.caption=str
	   elsif HIWORD(msg.wParam)==WMsg::LBN_DBLCLK then       #LBN_dblclk
	       selct= $lb.sendMessage(WMsg::LB_GETCURSEL,0,0)       #LB_getcursel
	       str=" "*$lb.sendMessage(WMsg::LB_GETTEXTLEN,selct,0) #LB_gettextlen
	       $lb.sendMessage(WMsg::LB_GETTEXT,selct,str)          #LB_getText
		   $ed.caption="DBLCLK "+str
	   end
	 elsif LOWORD(msg.wParam)==4124 then # combo box
	   if HIWORD(msg.wParam)==WMsg::CBN_SELCHANGE then       #CBN_selchange
	       selct= $cm.sendMessage(WMsg::CB_GETCURSEL,0,0)      #CB_getcursel
	       str=" "*$cm.sendMessage(WMsg::CB_GETLBTEXTLEN,selct,0) #CB_gettextlen
	       $cm.sendMessage(WMsg::CB_GETLBTEXT,selct,str)          #CB_getText
		   $ed.caption="コンボ!! "+str
	   end
     end
   else
     x=LOWORD(msg.lParam)
     y=HIWORD(msg.lParam)
	 $ed.caption= "RMUP=("+x.to_s+","+y.to_s+")"
   end
   
end

SWin::Application.messageloop

SWin::Application.messageBox "END","trap"
exit
