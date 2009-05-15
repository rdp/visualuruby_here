require 'vr/vractivex'
require 'vr/vrlayout'

class ExplorerAxControl < VRActiveXControl
   ACTIVEXCINFO =["SHDocVwCtl.WebBrowser","DWebBrowserEvents"]
end

class MyForm < VRForm
  include VRHorizLayoutManager

  def construct
    addControl ExplorerAxControl,"exp",""

    @exp.ole_interface.Navigate("http://www.google.co.jp/")
    @exp.add_oleeventhandler("NavigateComplete","completed") 
    @exp.add_oleeventhandler("TitleChange","titlechange") 
  end

  def exp_titlechange(title)
    self.caption=title
  end

  def exp_completed(url)
    puts url
  end
end

VRLocalScreen.start(MyForm)

