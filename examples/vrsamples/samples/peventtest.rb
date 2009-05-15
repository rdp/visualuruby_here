require "vr/vrcontrol"
require "vr/vrlayout"

#  example:
#  event handling for controls under panel
#  2 ways

class MyVertPanel < VRPanel
  include VRVertLayoutManager
  include VRStdControlContainer

  def construct
    addControl(VRButton,"btn1","‚½‚Ä")
    addControl(VRButton,"btn2","‚½‚Ä")
  end

  def btn1_clicked
    messageBox "c‚Ì1 Clicked","TEST",0
  end
  def btn2_clicked
    messageBox "c‚Ì2 Clicked","TEST",0
  end
end

class MyHorizPanel < VRPanel
  include VRHorizLayoutManager
end

##############################


frm3 = VRLocalScreen.newform
frm3.caption = "MultiPanel"
frm3.extend VRVertLayoutManager

def frm3.construct
  addControl(MyVertPanel ,"gr1","‚½‚Ä‚½‚Ä")
  addControl(MyHorizPanel,"gr2","‚æ‚±‚æ‚±")


  @gr2.addControl(VRButton,"btn1","‚æ‚±")
  @gr2.addControl(VRButton,"btn2","‚æ‚±")
  @gr2.extend VRStdControlContainer
  @gr2._init
  @gr2.send_parent("btn2","clicked")
  
  def @gr2.btn1_clicked
    messageBox "‰¡‚Ì1 Clicked","TEST",0
  end
end

def frm3.gr2_btn2_clicked
    messageBox "‰¡‚Ì2 Clicked","TEST",0
end

frm3.move 350,150,200,200
frm3.create.show

VRLocalScreen.messageloop
