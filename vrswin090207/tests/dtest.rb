require 'swin'
require 'Win32API'

require 'vr/vrcontrol'  # for definition of VRButton
require 'vr/vrdialog'   # for VRDialogTemplate


module WMsg
  WS_VISIBLECHILD   = 0x50000000
  BS_AUTOCHECKBOX =   3
end

class TestWin < SWin::Window
end
class TestDialog < SWin::Dialog
end

RFactory=SWin::LWFactory.new SWin::Application.hInstance

tp=VRDialogTemplate.new
tp.style = 0x90c800c0
tp.exstyle=0
tp.move 20,5,200,100
tp.caption="TEST DIALOG"
tp.addDlgControl VRButton,"button",5,5,50,30,0


$wins=[]

$wins.push mw=RFactory.newwindow(nil,TestWin)
mw.caption="Caption2"
mw.move(300,200,400,400)
mw.create
mw.show
mw.move(300,200,400,400)


$wins.push $bn=RFactory.newwindow(mw)
$bn.classname="BUTTON"
$bn.caption="Open Dialog"
$bn.etc=1234
$bn.style=WMsg::WS_VISIBLECHILD 
$bn.move 80,50,150,40
$bn.create.show

$wins.push $dia=RFactory.newdialog(tpp = tp.to_template, TestDialog)

$dia.addEvent WMsg::WM_COMMAND
def $dia.msghandler(msg)
  self.enddialog([msg.wParam])
end

$dia.move 30,$dia.y,$dia.w,$dia.h

mw.addEvent WMsg::WM_COMMAND

def mw.msghandler(msg)
   if msg.msg==WMsg::WM_COMMAND then
      if LOWORD(msg.wParam)==1234 then     # command button
        print "Dialog ret="
        p $dia.open(self)
      end
   end
end

SWin::Application.messageloop

