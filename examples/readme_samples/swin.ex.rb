require 'swin'
  
  WM_COMMAND        = 0x00000111
  WS_VISIBLECHILD   = 0x50000000
  BS_PUSHBUTTON     = 0x00000000
  
  RFactory=SWin::LWFactory.new(SWin::Application.hInstance)
  
  mw=RFactory.newwindow(nil)
  mw.caption="Window Caption"
  mw.move(300,200,400,400)
  mw.create
  
  bn=RFactory.newwindow(mw)
  bn.classname="BUTTON"
  bn.caption="Button1"
  bn.etc=1234            #control id
  bn.style=WS_VISIBLECHILD | BS_PUSHBUTTON
  bn.move 80,50,80,40
  bn.create
  
  mw.addEvent WM_COMMAND
  
  def mw.msghandler(msg)  #msg has attrs of hWnd,msg,wParam,lParam
    if msg.msg==WM_COMMAND then
      messageBox "Button Pushed"
    end
  end
  
  mw.show
  SWin::Application.messageloop
