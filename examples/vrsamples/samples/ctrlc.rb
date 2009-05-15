require 'vr/vruby'
require 'vr/vrcontrol'


module MyForm
  def construct
   self.caption= "VisualuRuby Interrupt Sample1"
   move 100,100,280,230
   addControl(VRButton,  "btn1","rescue‚³‚ê‚é",  75,50, 130,40)
   addControl(VRButton,  "btn2","rescue‚³‚ê‚È‚¢",75,100,130,40)
  end

  def btn1_clicked
    i=0
    0.upto(1000000) do
     begin
       i+=1 
     rescue Interrupt
       print "CTRL-C\n"
       retry
     end
    end

    messageBox @btn1.caption,"MSGBOX",0
  end
  def btn2_clicked
    i=0
    0.upto(1000000) do
     i+=1 
    end

    messageBox @btn2.caption,"MSGBOX",0
  end
end

frm=VRLocalScreen.newform
frm.extend MyForm
frm.create
frm.show

$int=0

begin
  VRLocalScreen.messageloop
rescue SWin::MsgloopInterrupt
  $int+=1
  print "Ctrl-C : #{$int}\n"
  retry
rescue Interrupt
  print "CTRL-C on executing script\n"
#  retry causes "Double Messageloop Error"
end

print "Total Ctrl-C : #{$int}\n"

exit