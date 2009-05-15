require 'vr/vrcontrol'
require 'vr/vrlayout'



module MyForm
  include VRVertLayoutManager
  include VRMenuUseable

  def construct
    self.caption="MENU TEST"

    # @amenu for keep VRMenu instance
    @amenu=newMenu(true).set([["&Pop","pop1"],["P&ush","push1"]])

    setMenu newMenu.set( 
       [ ["&Test","menu1"],
         ["&West","menu2"],
         ["&Rest","menu3",VRMenuItem::GRAYED],
         ["&Submenu",[["Sub&1","smenu1"],VRMenu::SEPARATOR,["Sub&2","smenu2"]] ],
         ["&Checktest",[["&Checked?","checking",VRMenuItem::CHECKED]] ],
         ["St&ack",@amenu]     # This way to keep VRMenu instance.
       ])
    move 100,100,400,400
    addControl VRStatic,"label","test"
    addControl VRButton,"btn1","add menu"
    addControl VRButton,"btn2","del menu"
    addControl VRButton,"btn3","menu count and menu gray"
    addControl VRButton,"btn4","menu state of added menu"
    addControl VRButton,"btn5","modifyMenu for added menu"
  end

  def btn1_clicked
    @test=@menu.append "&Append","append"
    @label.caption="MenuAdded"
  end
  def btn2_clicked
    @menu.delete @test if @test
    @label.caption="MenuDeleted"
    @test=nil
  end
  def btn3_clicked
    @label.caption="MenuItems="+@menu.count.to_s
    @test.state=1 if @test  
  end
  def btn4_clicked
    @label.caption="MenuState="+@test.state.to_s if @test
  end
  def btn5_clicked
    @test.modify "MODIFYED" if @test
  end
  def menu1_clicked
    @label.caption="MENU1"
  end
  def menu2_clicked
    @label.caption="MENU2"
  end
  def smenu1_clicked
    @label.caption="SUB MENU1"
  end
  def pop1_clicked
    @label.caption="POP"
  end
  def push1_clicked
    @label.caption="PUSH"
  end
  def smenu2_clicked
    @label.caption="SUB MENU2"
  end
  def checking_clicked
    @checking.checked = ! @checking.checked?
  end
end

frm=VRLocalScreen.newform
frm.extend MyForm

frm.create.show

VRLocalScreen.messageloop

