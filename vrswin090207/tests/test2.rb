require 'swin'

def RGB(r,g,b)
  return r+(g<<8)+(b<<16)
end

WM_PAINT=0x000F
BS_NULL = SWin::BRUSH::NULL

RFactory=SWin::LWFactory.new SWin::Application.hInstance


$mw=RFactory.newwindow(nil)
$mw.caption="Caption2"
$mw.move(300,200,400,400)
$mw.create.show

$mw.addEvent WM_PAINT

$white=RGB(0xff,0xff,0xff)
$red=RGB(0xff,0,0)
$blue=RGB(0,0,0xff)
$green=RGB(0,0xff,0)
$black=RGB(0,0,0)

def $mw.msghandler(msg)
# only WM_PAINT message comes

    setPen($white);setBrush($white);        fillRect(0,0,400,400)
    setPen($red); setBrush($red);           fillRect(10,10,130,130)
    setPen($green); setBrush($green);       fillEllipse(10,10,130,130)
    setPen($blue); setBrush($blue,BS_NULL); fillRect(100,100,230,230)
    setPen($green);                         drawLine(10,10,230,230)
    setPen($black);                         fillRoundRect(10,140,90,180,15,15)
    setPen($black);                         fillArc(10,10,230,230,0,0,200,150)
    setPen($blue);                          drawChord(10,10,230,230,200,150,0,0)
                                            drawText("Graphic",20,150,90,180)
end


$mw.refresh
SWin::Application.messageloop
