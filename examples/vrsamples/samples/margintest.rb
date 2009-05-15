# test of vrmargin
# programmed by yukimi_sake@mbi.nifty.com

require 'vr/vrcontrol'
require 'vr/vrlayout2'
require 'vr/vrmargin'

module MyForm
  include VRResizeable
  def construct
    addControl(VRButton,'button1','Set Margin 10',0,0,120,24)
    addControl(VRButton,'button2','Reset Margin',128,0,120,24)
    addControl(VRButton,'hbt1','Horiz Button1',88,0,80,24).
                          extend(VRMargin).initMargin(0,0,0,0)
    addControl(VRButton,'hbt2','Horiz Button2',88,0,80,24).
                          extend(VRMargin).initMargin(0,0,0,0)
    addControl(VRButton,'hbt3','Horiz Button3',88,0,80,24).
                          extend(VRMargin).initMargin(0,0,0,0)
    addControl(VRButton,'vbt1','Vert Button1',88,0,80,24).
                          extend(VRMargin).initMargin(0,0,0,0)
    addControl(VRButton,'vbt2','Vert Button2',88,0,80,24).
                           extend(VRMargin).initMargin(0,0,0,0)
    @vrm = VRVertLayoutFrame.new
    @vrm.register(@hbt1,@hbt2,@hbt3)
    @pn1=VRVertTwoPaneFrame.new(@vrm,@vbt2).setup(self)
    @pn0=VRHorizTwoPaneFrame.new(@vbt1,@pn1).setup(self).
                         extend(VRMargin).initMargin(0,30,0,0)
  end
  
  def button1_clicked
    @hbt1.setMargin(10,10,10,10)
    @hbt2.setMargin(10,10,10,10)
    @hbt3.setMargin(10,10,10,10)
    @vbt1.setMargin(10,10,10,10)
    @vbt2.setMargin(10,nil,10,10)
    @vbt2.caption="This button was aligned to bottom"
  end
  
  def button2_clicked
    @hbt1.setMargin(0,0,0,0)
    @hbt2.setMargin(0,0,0,0)
    @hbt3.setMargin(0,0,0,0)
    @vbt1.setMargin(0,0,0,0)
    @vbt2.setMargin(0,0,0,0)
    @vbt2.caption='Vert Button2'
  end

  def self_resize(w,h)
    @pn0.move 0,0,w,h
  end
  
  
end

VRLocalScreen.start MyForm
