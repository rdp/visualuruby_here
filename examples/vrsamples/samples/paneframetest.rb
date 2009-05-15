require 'vr/vrcontrol'
require 'vr/vrlayout2'


class MyForm2 < VRForm
  include VRResizeable
  Message="Drag each separator. Here are three paned frames"
  def construct
    self.caption = "TwoPaneFrame test"
    addControl VRStatic,"sta",Message,60,10,400,30
    addControl VRButton,"btn1","VERTBUTTON 1",0,0,10,10
    addControl VRButton,"btn2","VERTBUTTON 2",10,0,10,10
    @k1=VRVertTwoPaneFrame.new(@btn1,@btn2).setup(self)

    addControl VRButton,"btn3","HORIZBUTTON 1",0,0,10,10
    addControl VRButton,"btn4","HORIZBUTTON 2",10,0,10,10
    @k2=VRHorizTwoPaneFrame.new(@btn3,@btn4).setup(self)

    @k3 = VRVertTwoPaneFrame.new(@k1,@k2).setup(self)
  end
  def self_resize(w,h)
    @k3.move 50,50,w-100,h-100
  end
end

VRLocalScreen.start MyForm2
