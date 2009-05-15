=begin
= Document of Project VisualuRuby(temporary-name)

Author: Nishikawa,Yasuhiro (nyasu) <nyasu@osk.3web.ne.jp>

changed    on 2000/06/21   : license

== Abstract
This project is for the convenience of GUI ruby scripting on MS Windows.
The project has two part, one is about swin.so which is ruby extension library, 
the other is a set of vr* series of ruby scripts which wrap swin.so.

Japanese information about 'Project VisualuRuby(temporary-name)' are available
at http://www.threeweb.ad.jp/~nyasu/software/vrproject.html .

English documents of swin.so are available at co-developer 
Yukimisake-san's page http://homepage3.nifty.com/Yukimi_Sake/ruby.html .
Those are almost translated by machine. 

== Requirements
* Win32 environment.
* Ruby for your environment. cygwin/mswin/etc..
* C compiler (If you make swin.so from source files)

== Features
* with swin.so
  * able to create/show/destroy window.
  * adding child window(button/combobox/treeview and so on) on the window.
  * event handling for window's messages. (WM_LBUTTONUP,WM_COMMAND,...)
  * making a subclass of system-define window class.
  * creating menues.
  * creating common dialog (not all dialogs)
  * drawing the window using pen,brush,font,..
  * bitmap class is prepared.
  * handling idleloop in messageloop
  * handling Ctrl-C in messageloop
  * creating modal dialog
  * able to use OLE drag and drop
* with ruby libraries
  * programming with visualbasic-like style.(visualbasic is MicroSoft's product)
  * easily creating window.
  * easily creating controls on the window.
  * easily handling messages to the window.
  * layout manager(?) available
  * playing multimedia files (AVI/WAV/MP3/...) available with MCI 
  * owner-draw (button)
  * dde (execute/poke/request client, execute server)
  * handling clipboard
  * use ActiveX controls (testing)

== How to install
+ make swin.so
You are required to prepare the source of ruby interpreter itself and
make it to configure some C headers and some installer scripts.

To create and install swin.so,
  ruby extconf.rb
  make
  make install

+ using already compiled swin.so
Install it in the directory to be read by ruby.
Try the commandline  
  ruby -e'p $:'  
to inspect the proper directory.

+ install vr* series ruby scripts
Script library is in vr directory.
Install them into a proper directory.
I also prepare vrinstall.rb for your install.

== Simple sample of swin
The following is a sample to capture WM_COMMAND message sent by a Button.

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
  bn.etc=1234
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

== Simple sample of vruby
The feature is the same as the sample of swin.

  require 'vr/vruby'
  require 'vr/vrcontrol'

  class MyForm <VRForm
    def construct
      self.caption="Window Caption"
      self.move(300,200,400,400)
      addControl(VRButton,"btn1","Button1",80,50,80,40)
    end

    def btn1_clicked
      messageBox "Button Pushed"
    end
  end

  VRLocalScreen.showForm(MyForm)
  VRLocalScreen.messageloop


== Special Mensions
* No mis-spelling of 'VisualuRuby'  :)
* Libellous issues are disclaimed but positive comments are welcome.

== License
* This software is provided "AS IS" and WITHOUT ANY EXPRESS or IMPLIED WARRANTIES.

* Nyasu (Nishikawa,Yasuhiro) keeps the author's rights.

* Identical with Ruby's License

== Links for your convenience
: Home page of Project VisualuRuby (tmp-name) in Japanese 
  ((<here
  |URL:http://www.threeweb.ad.jp/~nyasu/software/vrproject.html
  >))

: Project VisualuRuby(tmp-name) download page (in Japanese)
  ((<here
  |URL:http://www.threeweb.ad.jp/~nyasu/vruby/core.html
  >))

: swin(VisualuRuby(temp-name) Core) source. ????? is the version's part.
  http://www.threeweb.ad.jp/~nyasu/vruby/vrswin??????.lzh

: vruby(VisualuRuby(temp-name) ruby part) 
  http://www.threeweb.ad.jp/~nyasu/vruby/vruby??????.lzh

: examples 
  ((<here
  |URL: http://www.threeweb.ad.jp/~nyasu/vruby/vrsamples.lzh
  >))

: html help files
  ((<Yukimisake-san's page|URL:http://homepage3.nifty.com/Yukimi_Sake/ruby.html>))

=end
