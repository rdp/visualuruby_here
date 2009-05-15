require 'vr/vrcontrol'
require 'vr/vrlayout'
require 'vr/vrhandler'
require 'vr/clipboard'
require 'vr/vrclipboard'


class MyForm <VRForm
  include VRVertLayoutManager
  include VRClipboardObserver

  def self_drawclipboard
    Clipboard.open(self.hWnd) do |cb|
      begin
        @lst.addString 0,cb.getText
      rescue RuntimeError
#        p "Not text or flushed clipboard"
      end
    end
    SMSG::SendMessage.call @cbchainnext,WM_DRAWCLIPBOARD,0,0 if @cbchainnext!=0
  end

  def construct
    addControl VRListbox,"lst","clip"
  end
  
  def lst_selchanged
    begin
      Clipboard.open(self.hWnd) do |cb|
        str=@lst.getTextOf(@lst.selectedString)
        cb.setText str
      end
    rescue
    end
    @lst.deleteString @lst.selectedString
  end
  
end

VRLocalScreen.start(MyForm)

