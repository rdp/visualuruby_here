require 'vr/vruby'
require 'vr/vrowndraw.rb'
require 'Win32API'

class MyOwnerDrawButton < VROwnerDrawButton
  DrawFrameControl=Win32API.new("user32","DrawFrameControl",["L","P","I","I"],"I")

  def drawpushed(left,top,right,bottom,state)
    rect=[left,top,right,bottom].pack("iiii")
    DrawFrameControl.call(hdc,rect,4,0x210)

    t="PUSHED"
    drawText(t,0,20,right,bottom,t.size,9)

    c = RGB(0xff,0,0)
    setPen c
    setBrush c
    fillRect(4,4,12,12)
  end
  
  def drawreleased(left,top,right,bottom,state)
    rect=[left,top,right,bottom].pack("iiii")
    DrawFrameControl.call(hdc,rect,4,0x10)

    t="RELEASED"
    drawText(t,0,20,right,bottom,t.size,9)

    c = RGB(0,0,0xff)
    setPen c
    setBrush c
    fillRect(4,4,12,12)
  end

=begin 
# moved to parent's method for example
  def drawfocused(left,top,right,bottom,state)
    setPen RGB(0x40,0x40,0x40),1,2
    setBrush 0xffffff,SWin::BRUSH::NULL
    fillRect(left+2,top+2,right-3,bottom-3)
  end
=end

end


module MyForm
  def construct
    self.caption="owner draw button"
    move 100,100,150,150
    addControl(MyOwnerDrawButton,"test","OK",10,10,100,50)
    addControl(VRStatic,"cl","clicked=0",10,70,100,30)
    @clicking=0
  end
  
  def test_clicked
    @cl.caption = "clicked="+(@clicking+=1).to_s
  end

  def test_drawfocused(left,top,right,bottom,state)
    @test.setPen RGB(0x40,0x40,0x40),1,2
    @test.setBrush 0xffffff,SWin::BRUSH::NULL
    @test.fillRect(left+2,top+2,right-3,bottom-3)
  end
  
end

VRLocalScreen.showForm MyForm
VRLocalScreen.messageloop

