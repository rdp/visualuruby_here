require 'vr/vruby'
require "vr/vrcontrol"

##################################################
# Create Form and design its controls
#
frm=VRLocalScreen.newform

def frm.construct
 self.caption= "VisualuRuby Sample1"
 
 addControl(VRButton,  "btn1","ボタンだよ",80,50,100,40)
 addControl(VRButton,  "btn2","CHKBOX check",80,100,120,40)
 addControl(VRStatic,  "label1","This is Static",200,50,200,40)
 addControl(VRCheckbox,"chk1","This is checkbox",80,150,150,40)
 addControl(VRListbox, "lst1","",80,200,150,90)
 @lst1.setListStrings ["あー","本日","は","晴天","なり"]
 addControl(VREdit,    "edt1","This is Edit",220,100,160,40, WStyle::WS_BORDER)
 addControl(VRCombobox,"cmb1","",250,200,150,90)
 @cmb1.setListStrings [ "ほげ", "へげ", "ふげ" ]

end

def frm.self_created
  @cmb1.select(0)
end

def frm.btn1_clicked
  messageBox @btn1.caption,"MSGBOX",0
end

def frm.btn2_clicked
  if @chk1.checked? then
	  @edt1.text= "Checked"
  else
	  @edt1.text= "Not Checked"
  end
end

def frm.lst1_selchanged
   @label1.caption= "選択は「" + @lst1.getTextOf(@lst1.selectedIndex)+"」"
end

def frm.cmb1_selchanged
  @label1.caption= "コンボは「" + @cmb1.getTextOf(@cmb1.selectedIndex)+"」"
end

#######################################
#  create instance of the Form
#
frm.create
frm.show

VRLocalScreen.messageloop
exit