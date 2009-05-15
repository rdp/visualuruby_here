require 'swin'

puts "== Enum Printers"
  SWin::Application::Printers.each do |p|
    puts p
  end

puts "== Page setup dialog [devname,paper,orient,scale,copies,color?]"
puts "   (on some environment, this took some seconds)"
  pp = SWin::CommonDialog::pageSetup(nil) 
  p pp[0],pp[2..-1]
#puts "== Select Printer"   # Old style
#ppo = SWin::CommonDialog::setPrinter(nil,nil,1,15,1,5)
#puts "== Selected Info(name,flag,from,to)"
#p ppo   # [pname,flag(0:all,1:selected,2:fromto),fromPage,toPage]

# about default printer
  unless pp then 
    puts "canceled to select default printer"
    pname = SWin::Application::Printers::default
    devmode = nil
  else
    pname = pp[0]
    devmode = pp[1]
  end
  puts "Selected printer is #{pname}\n\n"

puts "== Another way to setup printer (properties dialog)"
puts "In this case, setting in this dialog will be overwritten by the data of "
puts "previous dialog (by variable 'devmode'in the script)."
puts "But it's still saved as printer default setting."

  SWin::Application::Printers::propdialog nil,pname

puts "== Create PrintDoc"
  RFactory=SWin::LWFactory.new SWin::Application.hInstance
  printer = RFactory.openprinter(pname,devmode)
  raise "Printer open error" unless printer.is_a?(SWin::PrintDoc)

puts '== Paper size'
  print printer.size.join("x")," (pixel)","\n"

puts '== Extent size of Text "Graphic" '
  p printer.textExtent("Graphic")


def RGB(r,g,b) return r+(g<<8)+(b<<16); end

puts '== Resolution pixel/mm'
dpi = printer.dpi
hres,vres = dpi[0].to_f/25.4, dpi[1].to_f/25.4
p [hres,vres]

#exit

puts "== Drawing and printing"
blue=RGB(0,0,0xff)
green=RGB(0,0xff,0)
red=RGB(0xff,0,0)
black=RGB(0,0,0)
BS_NULL = SWin::BRUSH::NULL

printer.startdoc
printer.startpage do |p|
  p.setPen(black);p.setBrush(BS_NULL);
  p.fillEllipse(hres*20,vres*20,hres*120,vres*120)
  p.drawLine(hres*20,vres*70,hres*120,vres*70)

  p.drawLine(hres*25,vres*65,hres*20,vres*70)
  p.drawLine(hres*25,vres*75,hres*20,vres*70)

  p.drawLine(hres*115,vres*65,hres*120,vres*70)
  p.drawLine(hres*115,vres*75,hres*120,vres*70)

  p.textColor=red
  p.drawText("diameter=100mm",hres*40,vres*65,hres*100,vres*70)
end

printer.enddoc


