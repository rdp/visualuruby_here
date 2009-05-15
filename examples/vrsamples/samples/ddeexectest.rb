require 'vr/vrdde'
require 'vr/vrcontrol'

module MyForm
  include VRDdeClient

  def construct
    self.move 100,100,350,200
    self.caption="dde_exec test"
    addControl(VRStatic,"desc",
               "Push connect button and select an URL in the combobox",
               10,100,300,40)

    addControl(VRCombobox,"combo","Select from this list",10,10,300,100)
    @combo.setListStrings [
      "http://www.ruby-lang.org/",
      "http://www.rubyist.net/~kazu/samidare/",
      "http://www.osk.3web.ne.jp/~nyasu/software/vrproject.html"
      ]
  end

  def nsshell_dderefused(*arg)
    print "refused "; p arg
  end

  def combo_selchanged
    url = @combo.getTextOf(@combo.selectedString)
    ddeexecute "Firefox","WWW_OpenURL",url # +",,-1,0,,,,"
  end
end

VRLocalScreen.showForm(MyForm)
VRLocalScreen.messageloop


