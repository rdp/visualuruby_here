require 'vr/vruby'
require 'vr/vrcontrol'
require 'vr/vrcomctl'
require 'vr/vrlayout'

class MyUpdown < VRPanel
  include VRGridLayoutManager
  include VRStdControlContainer

  def construct  # VRUpdown is force to be relocated by VRGridLayoutManager.
    setDimension 8,2
    addControl VRStatic,"","sample child-control",0,0,8,1
    addControl VREdit,"num","0",0,1,7,1
    addControl VRUpdown,"updown","0",7,1,1,1,WStyle::UDS_INTBUDDYRIGHT
    @updown.setRange(0,10)
  end

  def num_changed
    call_parenthandler "changed",@num.caption.to_i
  end
end


class MyForm < VRForm
  include VRVertLayoutManager

  def construct
    self.caption = "sample of Arrayed controls and layout-manager"
    addControl VRButton,"exam","exam"

    addArrayedControl 1,VRButton,"btn","arrayed button1"
    addArrayedControl 2,VRButton,"btn","arrayed button2"

    addControl VRStatic,"lbl","the following control's value area is [0-10]"
    addControl VRStatic,"ex",""

    addArrayedControl 1,MyUpdown,"updown","11"
    addArrayedControl 2,MyUpdown,"updown","-11"
  end

  def exam_clicked
    messageBox "normal control clicked"
  end

  def btn_clicked(index)
    messageBox "clicked #{index}","arrayed control"
  end

  def updown_changed(index,pos)
    @ex.caption = "original control no.#{index} changed into #{pos}"
  end
end


VRLocalScreen.start(MyForm)
