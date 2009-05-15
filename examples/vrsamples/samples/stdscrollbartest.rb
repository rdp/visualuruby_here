=begin
= 標準スクロールバーのサンプル
標準スクロールバーを使った例です。ここではフォームを使っています。
スクロールバーを操作すると、フォーム上のコントロールが移動して、スクロール
しているかのように見えます。

= Sample for scrollbar
Example that uses scroll bar by form. Operate the scroll bar. 
The control moves synchronizing with the operation. It seems to scroll. 

=end
require 'vr/vruby'
require 'vr/vrcontrol'
require 'vr/contrib/vrstdscrollbar'
class Frm_test < VRForm
  include VRStdScrollbar
  
  def construct
    self.caption = 'Testing VRStdScrollbar'
    self.move(140,124,300,250)
    addControl(VRStatic,'sta1',"edit1:",20,20,100,24)
    addControl(VREdit,'edt1',"edit1",150,20,100,24)
    addControl(VRStatic,'sta2',"edit2:",270,20,100,24)
    addControl(VREdit,'edt2',"edit2",400,20,100,24)
    addControl(VRStatic,'sta3',"listbox",20,50,200,24)
    addControl(VRListbox,'lst1',"",20,80,100,200)
    addControl(VRStatic,'sta4',"groupbox",270,50,100,24)
    addControl(VRGroupbox,'grp1',"",270,80,100,200)
    
    @x={} ; @y={}
    @controls.each_value do |ctl|
      @x[ctl.hWnd]=ctl.x
      @y[ctl.hWnd]=ctl.y
    end
    setVScrollRange(0,300)
    setHScrollRange(0,530)
    self.vscroll_position=0
    self.vscroll_pagesize=self.h
    self.vscroll_smallstep=self.h/10
    self.vscroll_longstep=self.h
    self.hscroll_position=0
    self.hscroll_pagesize=self.w
    self.hscroll_smallstep=self.w/10
    self.hscroll_longstep=self.w
  end 
  
  def scroll
    @controls.each_value do |ctl|
      ctl.move(@x[ctl.hWnd]-hscroll_position,@y[ctl.hWnd]-vscroll_position,ctl.w,ctl.h)
    end
    self.refresh
  end
  
  def self_hscroll(code)
    scroll
  end
  
  def self_vscroll(code)
    scroll
  end
end 
frmmenu = VRLocalScreen.showForm Frm_test
VRLocalScreen.messageloop
