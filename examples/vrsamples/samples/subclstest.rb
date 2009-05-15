require "vr/vrcontrol"
require "vr/vrlayout"
require "vr/vrhandler"

#
#  Example for subclassing a control.
#
#  additional: enum-ing windows with message handler
#


#    This is  Edit Control which is hooked WM_CHAR message.

class VRHookedEdit < VREdit
  include VRKeyFeasible
  def vrinit
    super
    add_parentcall("char")
  end
end

####################################

module MyForm
  include VRVertLayoutManager
  def construct
    self.caption="HookedWindow"
    addControl(VRHookedEdit,"edit1","Edit1")
    addControl(VRButton,"btn2","msg handling windows")
  end

  def edit1_char(ansi,keydata)
    messageBox "“ü—Í‚µ‚Ü‚µ‚½[#{ansi.chr}]",self.caption,0
  end

  def btn2_clicked
    SWin::Application.eachMsghandled do |w|
      messageBox w.caption,"messagehandled",0
    end
  end
end

VRLocalScreen.showForm(MyForm,350,150,200,200)
VRLocalScreen.messageloop

