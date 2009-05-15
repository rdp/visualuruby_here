require 'vr/vrcontrol'
require 'vr/vrlayout'
require 'vr/vrdialog'

def test(parent,testcase)
  case testcase
  when 1
    print "Create Dialog from VRLocalScreen\n"

    p VRLocalScreen.modalform(parent,nil,MyDialog)

  when 2
    print "Create Original Dialog by Singleton methods\n"

    tp=VRDialogTemplate.new

    tp.move 5,5,200,100
    tp.caption="TEST DIALOG"
    tp.addDlgControl VRButton,"button1",10,10,100,30,WStyle::WS_TABSTOP
    tp.addDlgControl VRButton,"button2",10,40,100,30,WStyle::WS_TABSTOP

    tpl=tp.to_template

    dlg = VRModalDialog.new(VRLocalScreen,tp)
    def dlg.msghandler(msg)
      if msg.msg==WMsg::WM_COMMAND then
        close(true)
      end
    end
    p dlg.open(parent)

  when 3
    print "Create Dialog from Template\n"

    tp=VRDialogTemplate.new

    tp.move 5,5,180,80
    tp.caption="Inputbox"
    tp.fontname="system"
    tp.fontsize=12
    tgt = tp.addDlgControl(
          VREdit,"",4,4,170,20,WStyle::WS_TABSTOP|WStyle::WS_BORDER)
    okbtn = 
         tp.addDlgControl VRButton,"OK",24,40,60,20,WStyle::WS_TABSTOP
    cancelbtn =
         tp.addDlgControl VRButton,"Cancel",90,40,60,20,WStyle::WS_TABSTOP

    dlg = VRInputbox.new(VRLocalScreen,tp)
    dlg.options["okbutton"]=okbtn
    dlg.options["cancelbutton"]=cancelbtn
    dlg.options["target"] = tgt
    dlg.options["default"]="Input and push 'OK'"
    dlg.move 20,20,dlg.w,dlg.h
    p dlg.open(parent)
  when 4
    print "Create Dialog from Template 2\n"

    dlg = VRInputbox.new2(VRLocalScreen) do |opt,tp| 
      tp.caption="日本語のキャプション"
      opt["target"] = 
          tp.addDlgControl(VREdit,"",4,4,170,20,
                       WStyle::WS_TABSTOP|WStyle::WS_BORDER|WStyle::WS_GROUP)
      opt["okbutton"] = 
          tp.addDlgControl(VRButton,"OK",24,40,60,20,WStyle::WS_TABSTOP)
      opt["cancelbutton"] = 
         tp.addDlgControl(VRButton,"Cancel",90,40,60,20,WStyle::WS_TABSTOP)
    end
    dlg.options["default"]="try to input"
    dlg.move 20,20,dlg.w,dlg.h
    p dlg.open(parent)
  end
end



class MyForm  < VRForm
  include VRVertLayoutManager
  def construct
    move 200,200,400,200
    self.caption="dialog test"
    addControl VRButton,"btn1","create dialog in usual construction method"
    addControl VRButton,"btn2","create dialog by singleton method"
    addControl VRButton,"btn3","create inputbox by prepared template"
    addControl VRButton,"btn4","create inputbox by template defined in iterator"
  end

  def btn1_clicked()  test(self,1)   end
  def btn2_clicked()  test(self,2)   end
  def btn3_clicked()  test(self,3)   end
  def btn4_clicked()  test(self,4)   end
end


class MyDialog <VRModalDialog
  
  def construct
    addControl VRButton,"btn3","btbt",10,70,100,30,WStyle::WS_TABSTOP
    addControl VRButton,"btn4","tbtb",10,100,100,30,WStyle::WS_TABSTOP
  end

  def btn3_clicked
    p "bbbtttnnn3"
    close( ["This","is","test","array."] )
  end
  def btn4_clicked
    p "ぼぼぼぼぼたんんんんん"
    close(true)
  end
end


VRLocalScreen.showForm(MyForm)
VRLocalScreen.messageloop

