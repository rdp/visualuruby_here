require 'vr/vrcontrol'
require 'vr/vrhandler'

$white=RGB(0xff,0xff,0xff)
$red=RGB(0xff,0,0)
$blue=RGB(0,0,0xff)
$green=RGB(0,0xff,0)
$black=RGB(0,0,0)

class MyDrawingCanvasPanel < VRCanvasPanel
  include VRMouseFeasible

  def vrinit
    super
    @width=0
    @col=0
  end

  def setcolor(col)
    @col=col
    @canvas.setPen(@col,@width)
  end
  
  def setwidth(width)
    @width=width
    setcolor(@col)
  end

  def self_lbuttondown(shift,x,y)
    if shift==1 then
      @canvas.grLineTo(x,y)
    else
      @canvas.grMoveTo(x,y)
    end
    refresh
  end

end


module MyForm
  def construct
    self.caption="canvas test"
    addControl(MyDrawingCanvasPanel,"cv","canvas",0,0,400,400)
    addControl(VRButton,"cc","ColorChange",10,410,100,30)
    addControl(VRStatic,"lb","pen width",130,415,80,30)
    addControl(VRCombobox,"wd","1",200,410,50,130)
    addControl(VRButton,"sv","Save",300,410,100,30)

    @cv.createCanvas 400,400
    @wd.setListStrings(["1","2","4","8","16"])
    @wd.select(0)
  end
  
  def cc_clicked
    nc=chooseColorDialog
    @cv.setcolor(nc) if nc
  end
  
  def wd_selchanged
    @cv.setwidth( @wd.getString(@wd.selectedIndex).to_i )
  end

  def sv_clicked
    s = SWin::Bitmap.createBitmap *@cv.canvas.infoandbmp
    s.saveFile "canvas.bmp"
  end
end


VRLocalScreen.showForm(MyForm)
VRLocalScreen.messageloop

