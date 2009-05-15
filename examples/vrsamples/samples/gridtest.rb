require "vr/vrcontrol"
require "vr/vrlayout"

#  module-definitive form definition
#
#

module GridForm
  include VRGridLayoutManager
  
  def construct
    setDimension(10,10)
    addControl(VRStatic,"stc1","Resize! and See Titlebar",0,0,10,1)
    addControl(VRButton,"btn1","ÇÕÇ»",0,1,7,5)
    addControl(VRButton,"btn2","Ç‡Ç∞",7,1,3,5)
    addControl(VRButton,"btn3","ÇÁ..",0,6,10,4)
  end

  def btn1_clicked
    messageBox "Ç±ÇÍÇÕÇÕÇ»Ç≈Ç∑ÅB","GridForm",0
  end
end



frm3 = VRLocalScreen.newform
frm3.extend GridForm

frm3.move 350,150,300,300
frm3.create.show

o=""

VRLocalScreen.addIdleproc Proc.new{
  while true do
    t=Time.now.to_s
    frm3.caption=Time.now.to_s
    sleep 1
  end
}
VRLocalScreen.messageloop
