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


mw.addEvent WMsg::WM_PAINT

file=["digital.bmp",
      "c:\\windows\\エジプト.bmp","c:\\windows\\雲.bmp","c:\\windows\\花見.bmp"]

f = if ARGV.size>0 then ARGV[0] else file[2] end

ibmp=SWin::Bitmap.loadFile f

$bmp=SWin::Bitmap.createBitmap ibmp.infoStr,ibmp.bmpStr

rszw = SWin::Application.resizeborders
hsz = rszw[0]*2
vsz = SWin::Application.captionheight + rszw[1]

mw.move(300,200,$bmp.width + hsz,$bmp.height + vsz)
mw.create.show

def mw.msghandler(msg)
 #only WM_PAINT comes
  
#  drawBitmap $bmp
  stretch 0,0,self.w,self.h,$bmp

end
mw.refresh
SWin::Application.messageloop

exit
