require 'vr/vruby'
require 'vr/vrcontrol'


class MyPanel1 < VRPanel
  include VRMessageParentRelayer
  include VRStdControlContainer
  def construct
    addControl VRButton,"btn1","BTN1",5,5,50,23
    addControl VRText,"txt1","Panel",5,30,50,23
  end
end

class MyPanel2 < VRPanel
  include VRMessageParentRelayer
  def construct
    addControl MyPanel1,"child1","p",5,5,80,80,WStyle::WS_BORDER
    addControl MyPanel1,"child2","p",5,90,80,80,WStyle::WS_BORDER
  end
end

class MyForm < VRForm
  def construct
    move 100,100,250,350
    addControl MyPanel1,"panel","p",0,0,80,80,WStyle::WS_BORDER
    addControl MyPanel2,"plane","",0,100,90,180,WStyle::WS_BORDER
    addControl VRStatic,"lbl1","relay test result",90,5,140,25
    addControl VREdit,"edit","",90,30,140,25
  end
  
  
  def panel_btn1_clicked
    @edit.text = "1-level relay"
  end

  def plane_child1_btn1_clicked
    @edit.text = "2-level relay(1)"
  end

  def plane_child2_btn1_clicked
    @edit.text = "2-level relay(2)"
  end
end

VRLocalScreen.start MyForm
