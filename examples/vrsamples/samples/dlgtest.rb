require 'vr/vrcontrol'

module MyForm
  include VRMenuUseable
  include VRDrawable
  
  def construct
    self.caption="DialogTest"
    setMenu newMenu.set [ ["&File" ,[["&Open","mopen"],["E&xit","mexit"]]] ]
    @bmp=nil
  end

  def self_paint
    drawBitmap @bmp if @bmp
  end
  
  def mopen_clicked
    fn=SWin::CommonDialog::openFilename(self,[ ["bmp file","*.bmp"],["rle file","*.rle"] ])
    if !fn then return end
    @bmp=SWin::Bitmap.loadFile fn
    refresh
  end
  def mexit_clicked
    self.close
  end
end


frm=VRLocalScreen.newform
frm.extend MyForm
frm.create.show

VRLocalScreen.messageloop

