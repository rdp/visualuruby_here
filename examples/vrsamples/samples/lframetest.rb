require 'vr/vrcontrol'
require 'vr/vrlayout2'

class MyForm < VRForm
  include VRResizeable
  def construct
    self.caption = "layout frame test"
    @rm  = VRHorizLayoutFrame.new
    @rm1 = VRVertLayoutFrame.new
    @rm2 = VRHorizLayoutFrame.new
    @rm.register(@rm1,@rm2)

    addControl VRButton,"btn1","vframe btn1",0,0,10,10
    addControl VRButton,"btn2","vframe btn2",0,0,10,10
    addControl VRButton,"btn3","vframe btn3",0,0,10,10
    addControl VRButton,"btn4","vframe btn4",0,0,10,10
    @rm1.register(@btn1,@btn2,@btn3,@btn4)

    addControl VRButton,"btn11","hframe btn11",0,0,10,10
    addControl VRButton,"btn12","hframe btn12",0,0,10,10
    addControl VRButton,"btn13","hframe btn13",0,0,10,10
    @rm2.register(@btn11,@btn12,@btn13)
  end
  
  def self_resize(w,h)
    @rm.move 0,0,w,h
  end
end
VRLocalScreen.start(MyForm)
