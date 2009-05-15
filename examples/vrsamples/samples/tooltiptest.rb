require 'vr/vrcontrol'
require 'vr/vrtooltip'

class MyForm < VRForm
  include VRDrawable

  AREA =  [10,100,160,160]

  def construct
    self.caption = "tooltip test"
    self.move 100,100,200,200

    addControl VRStatic,  "sta", "each controls has tooltip",5,0,200,30
    addControl VRCheckbox,"chk1","check box 1",5,30,110,20
    addControl VRCheckbox,"chk2","check box 2",5,55,110,20

    addControl VRButton,"bt","tool",120,30,60,60,WStyle::WS_BORDER

    tt = createTooltip  # you can use @tooltip instead of tt
    tt.maxtipwidth=30
    tt.bkColor = RGB(0xff,0xe0,0xe0)

    @btt = tt.addTool @bt,"never clicked button"
    tt.addTool @chk1,"the first checkbox"
    tt.addTool @chk2,"the second checkbox"
    tt.addTool @sta,"static control"

    tt.addToolArea AREA,"an area tool"
    @clicked=0
  end

  def bt_clicked
    @tooltip.setTextOf(@btt,"clicked #{@clicked+=1} time(s)")
  end

  def self_paint   # draw tool area
    bkc = RGB(0,0,0x80)
    setPen bkc
    setBrush bkc
    fillRect *AREA
    self.textColor = RGB(0xff,0xff,0xff)
    self.opaque=false
    drawText "Tool Area",AREA[0]+40,AREA[1]+20,160,150
  end
end

VRLocalScreen.start(MyForm)

