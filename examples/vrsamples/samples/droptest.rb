require 'vr/vrcontrol'
require 'vr/vrlayout'
require 'vr/vrddrop'

module DropFileViewerForm
  include VRVertLayoutManager
  include VRDropFileTarget

  def construct
    self.caption="Drop me a text file"
    addControl(VRText,"text1","",WStyle::WS_VSCROLL)
  end
  
  def textset(fname)
    a=open(fname).read.gsub("\n","\r\n")
    self.caption=fname
    @text1.text=a
  end

  def self_dropfiles(files)
    textset(files[0])
    1.upto(files.size-1) do |i|
      VRLocalScreen.showForm(DropFileViewerForm).textset(files[i])
    end
  end
end


VRLocalScreen.showForm(DropFileViewerForm)
VRLocalScreen.messageloop
 
  