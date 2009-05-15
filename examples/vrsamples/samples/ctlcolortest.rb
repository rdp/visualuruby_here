# This is contributed by Shigitani-san.

require 'vr/vruby'
require 'vr/vrcontrol'
require 'vr/vrhandler'

class MyEdit < VREdit

  include VRFocusSensitive

  def self_gotfocus
    setCtlBkColor(RGB(0xef, 0xff, 0xff))
    refresh
  end

  def self_lostfocus
    setCtlBkColor(RGB(0xff, 0xff, 0xff))
    refresh
  end

end

class MyForm < VRForm

  include VRCtlColor
  include VRDestroySensitive

  def construct
    addControl(MyEdit, 'ed1', 'ed1', 3, 3, 100, 24)
    addControl(MyEdit, 'ed2', 'ed2', 3, 30, 100, 24)
    [@ed1, @ed2].each do |ctl|
      addCtlColor(ctl)
      ctl.setCtlTextColor(RGB(0xff, 0x00, 0x00))
      ctl.setCtlBkColor(RGB(0xff, 0xff, 0xff))
    end
  end

  def self_destroy
    messageBox "destroyed"
  end
end

VRLocalScreen.showForm(MyForm, 0, 0, 120, 84)
VRLocalScreen.messageloop
