require 'vr/vrcontrol'
require 'vr/vrtray'

# MyForm: one form with (up to many) tray icons associated with it

class MyForm < VRForm
  include VRTrayiconFeasible
  include VRMenuUseable
  # load icons
  LoadIcon = Win32API.new("user32","LoadIcon","II","I")
  QUESTIONICON = LoadIcon.call(0,32514)
  EXCLAMATIONICON = LoadIcon.call(0,32515)

  def construct
    self.caption="tray test"
    @counter=0
    addControl VRButton,"btn","add",5,5,100,30
    addControl VRButton,"btn2","del",5,35,100,30
    addControl VRButton,"btn3","mod",5,65,100,30
    addControl VRButton,"btn4","to tray",5,95,100,30
    move 100,100,120,160

    # create the "when you right click on icon in the tray" bottom right menu

    @iconmenu = newPopupMenu
    @iconmenu.set([
       ["dummy","test"],["dummy2","test2"], ["exit","exit"]
     ])
    
    @dummymenu = newPopupMenu
    @dummymenu.set([
       ["dummy","test"],["dummy2","test2"], ["do nothing","nothing"]
     ])

    # trayicon menu for minimumized window
    @traymenu = newPopupMenu
    @traymenu.set([
      ["return to normal window","restore"],["exit","exit"]
    ])
    @mytrayicon=nil
  end

  def btn_clicked
    create_trayicon(nil,"test#{@counter}",@counter)
    @counter+=1
  end

  def btn2_clicked
    @counter-=1
    delete_trayicon(@counter)
  end

  def btn3_clicked
    modify_trayicon(QUESTIONICON,"modified",@counter-1)
  end

  def btn4_clicked
    into_trayicon
  end

  def self_traylbuttondown(iconid)
#    p "lbuttondown #{iconid}"  # makes this program insensitive
  end

  def self_trayrbuttonup(iconid)
    case iconid
    when @mytrayicon then
      showPopup @traymenu
    when 0
      showPopup @iconmenu
    else
      showPopup @dummymenu
    end
  end


  def into_trayicon
    create_trayicon(EXCLAMATIONICON,"trayicon",@counter)
    @mytrayicon=@counter
    @counter+=1
    self.hide
    myexstyle = self.exwinstyle
    myexstyle.ws_ex_toolwindow = true
    myexstyle.ws_ex_appwindow = false
  end

  def outfrom_trayicon
    delete_trayicon(@mytrayicon)
    myexstyle = self.exwinstyle
    myexstyle.ws_ex_toolwindow = false
    myexstyle.ws_ex_appwindow = true
    @mytrayicon=nil
    self.show
  end

  def restore_clicked
    outfrom_trayicon
  end

  def exit_clicked
    cleanup
    self.close
  end

  def cleanup
    0.upto(@counter-1) do |i|
      delete_trayicon(i)
    end
  end
end




VRLocalScreen.start(MyForm)
