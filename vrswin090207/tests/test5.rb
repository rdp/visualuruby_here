require 'swin'

module WMsg
  WS_CHILD          = 0x40000000
  WS_VISIBLECHILD   = 0x50000000
  WS_BORDER         = 0x00800000
  WS_OVERLAPPED     = 0
  WM_COMMAND        = 0x00000111
  WM_RBUTTONUP      = 517
  WM_NOTIFY         = 0x004E
  WM_PAINT          = 0xf

end


RFactory=SWin::LWFactory.new SWin::Application.hInstance

$wins=[]

$wins.push mw=RFactory.newwindow(nil)
mw.caption="Caption2"

menu=RFactory.newmenu
pop1=RFactory.newpopup
pop1.append "&Nest",1
pop1.append "&Best",2
pop1.append "&Xest",3
pop2=RFactory.newpopup
pop2.append "&Hest",4
pop2.append "&Fest",5
pop2.append "&Dest",6

menu.append "&Test",7
menu.append "&Rest",8
menu.append "&West",9

menu.append "&Xyz",pop1
menu.append "&UUU",pop2

mw.create.show


$wins.push $label=RFactory.newwindow(mw)
$label.classname="STATIC"
$label.style=WMsg::WS_VISIBLECHILD
$label.move 10,20,100,30
$label.caption="TEST"
$label.create.show

mw.setMenu menu

mw.addEvent WMsg::WM_COMMAND

def mw.msghandler(msg)
  $label.caption= "MENU NO:"+msg.wParam.to_s
end

SWin::Application.messageloop

exit
