require 'vr/vrcontrol'
require 'vr/vrhandler'
require 'vr/vrolednd'

class DropPanel < VRPanel
  include VROleDropTarget
  CF_NETSCAPE = ClipboardFormat::RegisterClipboardFormat.call("Netscape Bookmark")

  def construct
    addControl VRStatic,"label","Drop here(CF_TEXT,Files,Netscape Bookmark",
                0,0,500,200
    start_oledroptarget([CF_NETSCAPE, CF_URL, CF_TEXT, CF_HDROP])
  end

  def self_oledrop(handle,format,keystate)
    case format
    when CF_HDROP
      print "drop files(" << DragDropFiles.get(handle).files.join(" , ") << ")"
    when CF_TEXT
      print "drop text(" << DragDropText.get(handle).text << ")"
    when CF_URL  # ???
      print "drop url(" << DragDropText.get(handle).text << ")"
    when CF_NETSCAPE
      print "drop netscape bookmark (" << DragDropText.get(handle).text << ")"
    else
      print "drop unknown"
    end
  end

  def print(txt)
    @label.caption=txt
  end

end

class DragSourcePanel < VRPanel
#  include VROleDragSourceLow
  include VROleDragSource
  include VRMouseFeasible

  def construct
    addControl VRStatic,"sta","Drag here to copy. (Multiple format)\r\n" + 
                              "  CF_TEXT[This is sample texts]\r\n" +
                              '  CF_HDROP[C:\Autoexec.bat]',
                              0,0,500,200
  end

  def self_lbuttondown(*arg)
    dragtext = DragDropText.set("This is sample texts")
    dragfile = DragDropFiles.set(['C:\autoexec.bat'])

    dragobj = [dragfile,dragtext]
    start_oledrag(dragobj)
  end

# The following is how to use VROleDragSourceLow
#  def self_lbuttondown(*arg)
#    @dragobj = DragDropText.set("This is sample texts")
#    start_oledragsource([CF_TEXT])
#  end
#  def self_getoledragitem(format)
#    case format
#    when CF_TEXT
#      @dragobj.handle
#    else
#      0
#    end
#  end

end

class MyForm < VRForm

  def construct
    self.caption = " OLE Drag Drop test"
    move 10,10,500,450
    addControl DropPanel,"drop","",0,0,500,300
    addControl DragSourcePanel,"drag","",0,300,490,90,WStyle::WS_BORDER
  end
end

VRLocalScreen.start(MyForm)
