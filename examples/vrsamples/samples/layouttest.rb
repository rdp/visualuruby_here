
require "vr/vrcontrol"
require "vr/vrlayout"

##########################################
module MyForm
  def construct
    addControl(VRButton,"btn1","‚±‚ê")
    addControl(VRButton,"btn2","‚±‚ê‚à",  0)
    addControl(VRButton,"btn3","‚ ‚ê")
    addControl(VRButton,"btn4","‚ ‚ê‚à",  0)
  end
end


frm = VRLocalScreen.newform
frm.caption="VertLayout"
frm.extend MyForm
frm.extend VRVertLayoutManager
frm.move 250,250,200,200
frm.create.show

frm2 = VRLocalScreen.newform
frm2.caption="HorizLayout"
frm2.extend MyForm
frm2.extend VRHorizLayoutManager
frm2.move 500,250,200,200
frm2.create.show


# ###
#  There are 2 ways to add a LayoutManager with a Panel
# 

class MyVertLayoutPanel < VRPanel # The first way is to create new Panel class
  include VRVertLayoutManager
end

frm3 = VRLocalScreen.newform
frm3.caption = "MultiPanel"
frm3.extend VRVertLayoutManager

def frm3.construct
  addControl(MyVertLayoutPanel, "gr1","‚½‚Ä‚½‚Ä")
 
                       # The second way is to extend Control with LayoutManager
  addControl(VRPanel,"gr2","‚æ‚±‚æ‚±")
  @gr2.extend VRHorizLayoutManager
  @gr2._init  # You need to call this method manually to configure added module.

  @gr1.addControl(VRButton,"btn1","‚½‚Ä")
  @gr1.addControl(VRButton,"btn2","‚½‚Ä")
 
  @gr2.addControl(VRButton,"btn3","‚æ‚±")
  @gr2.addControl(VRButton,"btn4","‚æ‚±")
end


frm3.move 350,150,200,200
frm3.create.show

VRLocalScreen.messageloop
