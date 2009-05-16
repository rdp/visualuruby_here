# requires run via: ruby -Ks
require 'vr/vruby'
require 'vr/vrcontrol'
require 'vr/vrtimer'

$BKCOLOR=RGB(0x00,0x00,0x80)
$TIMECOLOR=RGB(0xff,0xff,0xff)
$DATECOLOR=RGB(0xff,0xff,0x00)

$newclass = VRLocalScreen.factory.registerWinClass("clock",0)

class MyForm <VRForm
  include VRTimerFeasible
  include VRDrawable
  include VRMenuUseable

  def MyForm.winclass
    $newclass
  end

  def construct
    self.caption = "fonttest"
    move 100,100,320,200
    addTimer 1000
    setMenu newMenu.set( [["&File", [ ["&FontSet","cset"],["e&xit","exit"] ] ]])
    @font = @screen.factory.newfont( "‚l‚r –¾’©",54 )
    @dfont = @screen.factory.newfont("‚l‚r ƒSƒVƒbƒN",26)
  end

  def self_paint
    a=Time.now
    t=format("%02d:%02d:%02d",a.hour,a.min,a.sec)
    d=format("%04d/%02d/%02d",a.year,a.month,a.day)

    setBrush($BKCOLOR);        fillRect(0,0,400,400)
    self.drawBkColor=$BKCOLOR

    self.setFont *@font
    self.textColor= $TIMECOLOR
    self.drawText t,40,10,300,90
    self.setFont @dfont
    self.textColor= $DATECOLOR
    self.drawText d,80,90,300,120
  end
  
  def self_timer
    self.refresh
  end
  
  def cset_clicked
    font = chooseFontDialog()
    if font then
      @font.delete
      @font = @screen.factory.newfont(*font.params)
    end
  end
  
  def exit_clicked
    self.close
  end
end


VRLocalScreen.start(MyForm)
