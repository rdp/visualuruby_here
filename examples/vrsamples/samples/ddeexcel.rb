
require 'vr/vrcontrol'
require 'vr/vrlayout'
require 'vr/vrdde'

module MyClientForm
  include VRDdeClient
  include VRVertLayoutManager
  def construct
    move 100,100,140,200
    self.caption="dde client"
    addControl(VRButton,"request","Excel Request")
    addControl(VRButton,"exec","Excel Exec")
    addControl(VRButton,"poke","Excel Poke")
    addControl(VRButton,"error","Error")
    addControl(VRButton,"test","TEST")
  end

  def request_clicked
    dderequest "Excel","[Book1]Sheet1","R1C1"
  end
  def exec_clicked
    ddeexecute "Excel","System","1243{ENTER}"
  end
  def poke_clicked
    ddepoke "Excel","[Book1]Sheet1","R1C1","POKE\r\n"
  end

  def error_clicked
    ddeexecute "Hoge","Foo","[hanamogera]"
  end
  
  def excel_dderefused
    p "refused"
  end

  def excel_ddedata(data,fmt,flag)
    p "request data="+data
  end

  def excel_ddeexecdone(retcode)
    p "done"
  end
  def excel_ddepokedone(retcode)
    p retcode
  end

  def test_clicked
    ddeexecute "myform","test","[sample execute]"
  end
  def myform_ddeexecdone(*arg)
    p "myform dde done"
  end
end

module MyServerForm
  include VRDdeExecuteServer
  include VRVertLayoutManager
  def construct
    move 400,100,200,100
    self.caption="dde server"
    addControl VRStatic,"label1",""
    addDDEAppTopic("myform","test")
  end
  
  def self_ddeexecute(cmd,shwnd,app,topic)
    @label1.caption=cmd
  end
end

VRLocalScreen.showForm(MyClientForm)
VRLocalScreen.showForm(MyServerForm)
VRLocalScreen.messageloop
