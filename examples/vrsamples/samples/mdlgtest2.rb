require 'vr/vrdialog'
require 'vr/vrcontrol'

module MyDialogContents
  def construct
    move 0,0,150,150
    centering
    addControl VRButton,"btn1","button1",10,10,100,30,WStyle::WS_TABSTOP
    addControl VRButton,"btn2","button2",10,40,100,30,WStyle::WS_TABSTOP
    addControl VRButton,"btn3","button3",10,70,100,30,WStyle::WS_TABSTOP
  end

  def btn1_clicked
    close( ["This","is","sample","return","value"] )
  end
  def btn2_clicked
    close(false)
  end
  def btn3_clicked
    close(3)
  end
end

class MyModalDialog < VRModalDialog
  include MyDialogContents
end

class MyModelessDialog < VRModelessDialog
  include MyDialogContents
end

puts "== Using class"
puts "modeless dialog"
VRLocalScreen.modelessform(nil,nil,MyModelessDialog)  #parent,style,dialog
puts "in messageloop"
VRLocalScreen.messageloop

puts "modal dialog"
p VRLocalScreen.modalform(nil,nil,MyModalDialog)

puts "== Using module"
puts "modeless dialog"
VRLocalScreen.modelessform(nil,nil,MyDialogContents)
puts "in messageloop"
VRLocalScreen.messageloop

puts "modal dialog"
p VRLocalScreen.modalform(nil,nil,MyDialogContents)

puts "== Using template"

tp=VRDialogTemplate.new
tp.move 5,5,180,80
tp.caption="Inputbox"
tp.fontname="system"
tp.fontsize=12
okbtn = tp.addDlgControl VRButton,"OK",24,40,60,20,WStyle::WS_TABSTOP
cancelbtn = tp.addDlgControl VRButton,"Cancel",90,40,60,20,WStyle::WS_TABSTOP
tgt = tp.addDlgControl(VREdit,"",4,4,170,20,WStyle::WS_TABSTOP|WStyle::WS_BORDER)

#options = {"okbutton"=>okbtn,"cancelbutton"=>cancelbtn,"target"=>tgt }
options = {"okbutton"=>okbtn,          # or okbtn
           "cancelbutton"=>cancelbtn,  # or cancelbtn
           "target"=>tgt }

p VRLocalScreen.modalform(nil,nil,VRInputbox,tp.to_template,options)
