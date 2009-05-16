 require 'vr/vruby'
  require 'vr/vrcontrol'

  class MyForm < VRForm
    def construct
      self.caption="Window Caption"
      self.move(300,200,400,400)
      addControl(VRButton,"btn1","Button1",80,50,80,40)
    end

    def btn1_clicked
      messageBox "Button Pushed"
    end
  end

  VRLocalScreen.showForm(MyForm)
  VRLocalScreen.messageloop

