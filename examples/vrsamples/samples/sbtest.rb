require 'vr/vrcontrol'


class MyForm < VRForm
  def construct
    move 250,250,250,250
    addControl VRHScrollbar,"sbh","sb",0,200,200,20
    @sbh.setRange 0,100
    @sbh.position= 50
    @sbh.pagesize = 30
    @sbh.longstep = 20
    addControl VRVScrollbar,"sbv","sb",0,0,20,200
    @sbv.setRange 0,100
    @sbv.position= 50
    @sbv.pagesize = 3
    @sbv.longstep = 10

    addControl VRStatic, "sbhtext","50", 100,0,100,30
    addControl VRStatic, "sbvtext","50", 100,40,100,30
  end

  def sbh_changed
    @sbhtext.caption =  @sbh.position.to_s
  end
  def sbv_changed
    @sbvtext.caption =  @sbv.position.to_s
  end
end

VRLocalScreen.start MyForm
