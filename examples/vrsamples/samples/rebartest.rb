require 'vr/winconst'

require 'vr/vrcomctl'
require 'vr/vrcontrol'
require 'vr/vrlayout2'

class MyForm < VRForm
  include VRResizeable

  def construct
    @rm = VRVertLayoutFrame.new
    @rm1 = VRHorizLayoutFrame.new
    @rm2 = VRVertLayoutFrame.new
    @rm.register @rm1,@rm2

    addControl(VRButton,"btn1","1",0,0,10,10)
    addControl(VRButton,"btn2","2",0,0,10,10)
    addControl(VRButton,"btn3","3",0,0,10,10)
    addControl(VRButton,"btn4","4",0,0,10,10)
    addControl(VRButton,"btn5","5",0,0,10,10)
    addControl(VRButton,"btn6","6",0,0,10,10)
    addControl(VRButton,"btn7","7",0,0,10,10)

    @rm1.register @btn1,@btn2,@btn3
    @rm2.register @btn4,@btn5,@btn6,@btn7


    addControl VRRebar,"rb","rb",0,0,400,25,WStyle::CCS_TOP
    btn = @rb.addControl VRButton,"btn","BUTTON",20,0,50,25
    @rb.insertband(btn,"button",80)

    tb1 = @rb.addControl VRToolbar,"tbar1","std",20,0,50,25,WStyle::CCS_NORESIZE
    @rb.insertband(tb1,"",300)
    tb2 = @rb.addControl VRToolbar,"tbar2","std",20,0,50,25,WStyle::CCS_NORESIZE
    @rb.insertband(tb2,"view",150)

    @rb.relayout

    tb1.sendMessage WMsg::TB_LOADIMAGES,0,-1
    1.upto(15) do |i| tb1.addButton "stdtb#{i}" ; end

    tb2.sendMessage WMsg::TB_LOADIMAGES,4,-1
    1.upto(12) do |i| tb2.addButton "vwtb#{i}" ; end
  end

  def relayout(w,h)
    rb_h = @rb.h + 4 
    @rm.move 0,rb_h,w,h-rb_h
  end
  def self_resize(w,h)
    relayout(w,h)
  end
  def rb_layoutchanged()
    relayout(self.w,self.h)
  end

  def setText(text)
    @btn1.caption="clicked #{text}"
  end

  def btn_clicked
    setText"btn1"
  end

# following defines event handlers for each toolbar button.

# manual definition
  def stdtb1_clicked()
    setText "stdtb1"
  end

# definition by script
2.upto(15) do |i| 
eval <<"EEOOFF"
  def stdtb#{i}_clicked();
    setText "stdtb#{i}"
  end
EEOOFF
end

1.upto(12) do |i| 
eval <<"EEOOFF"
  def vwtb#{i}_clicked();
    setText "vwtb#{i}"
  end
EEOOFF
end

end

VRLocalScreen.start MyForm

