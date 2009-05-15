
require 'vr/vrcomctl'
require 'vr/vrcontrol'
require 'vr/rscutil'

class MyForm <VRForm
  def construct
    
    self.caption = "Tab test"

    addControl VRTabbedPanel,"tab","tab",10,10,400,150
    @tab.setupPanels("T1","T2","T3","T4")

    f = @screen.factory.newfont("‚l‚r –¾’©",36)

    btn1=@tab.panels[0].addControl(VRButton,"btn","Panel 0",10,50,200,40)
    btn1.setFont f
    @tab.panels[0].addControl VRStatic,"s","Button has action",10,100,200,40
    @tab.panels[1].addControl(VRButton,"btn","Panel 1",10,40,200,40).setFont f
    @tab.panels[2].addControl(VRButton,"btn","Panel 2",100,50,200,40).setFont f
    @tab.panels[2].addControl VRStatic,"s","Button has action",10,100,200,40
    @tab.panels[3].addControl(VRButton,"btn","Panel 3",100,50,200,40).setFont f
    @tab.selectTab 1
#    p @tab.countTabs

# 2 patterns below to handle controls events.
  
    def btn1.clicked
      messageBox "CLICKED"
    end

    @tab.send_parent2 2,"btn","clicked"
  end

  def tab_panel2_btn_clicked
    messageBox "clicked"
    GC.start
  end

# 2 patterns end.

  def tab_selchanged
    i = @tab.selectedTab
    txt = @tab.getTabTextOf(i)
    if txt[0,1]=="T" then txt.downcase! else txt.upcase! end
    @tab.setTabTextOf(i,txt)
  end
end

VRLocalScreen.start MyForm

