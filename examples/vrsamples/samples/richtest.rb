require 'vr/vrcontrol'
require 'vr/vrrichedit'
require 'vr/vrlayout'

class MyForm <VRForm
  include VRVertLayoutManager
  include VRMenuUseable

  def jlengthOf(text)
    text.gsub(/[^\Wa-zA-Z_\d]/, ' ').length  # from jcode.rb
  end

  def construct
    self.caption = "Richedit Control Test"
    addControl VRRichedit,"rich","This is test.\n"
    setMenu newMenu.set [
       [ "SetColor",[ ["Red","cred"],["Green","cgreen"],["Black","cblack"] ] ],
       [ "Font", [ ["Set","fontset"],["Print","fontget"] ] ],
       [ "SetAlign",[ ["Left","aleft"],["Right","aright"],["Center","acenter"] ] ]
     ]
    test
  end
  

  def realizecol
    @attrs.each do |attr|
      @rich.setSel *attr[0]
      @rich.selformat = attr[1]
    end
  end

  def addtextwithattr(txt,attr)
    @attrs = [] unless @attrs
    st = jlengthOf(@rich.text)
    len= jlengthOf(txt)
    @rich.text += txt
    @attrs.push [ [st,st+len],attr.dup ]
    realizecol
  end

  def test
    attr = @rich.selformat
    
    addtextwithattr("background LightGray\n",attr)
    @rich.bkcolor=0xE0E0E0

    attr.color=0xff
    addtextwithattr("Red ",attr)

    attr.color=0x8000
    addtextwithattr("DarkGreen ",attr)
    
    attr.color=0xff0000
    addtextwithattr("Blue\n",attr)

    attr.color=0x80
    attr.bold=true
    addtextwithattr("Bold ",attr)

    attr.color=0x8000
    attr.bold=false; attr.italic=true
    addtextwithattr("Italic ",attr)

    attr.color=0x800000
    attr.italic=false; attr.underlined=true; 
    addtextwithattr("Underline ",attr)

    attr.color=0x808000
    attr.underlined=false; attr.striked=true; 
    addtextwithattr("StrikeOut\n",attr)

    attr.color=0;attr.style=0
    attr.fontface="Arial"
    attr.height=400
    # omits setting charset and pitch&family
    addtextwithattr("Select texts to change their attributes from menu.\n",attr)

    attr.color=0x8080
    attr.fontface="@‚l‚r –¾’©"
    attr.height=480;  attr.pitch_family=17;  attr.charset=128
    addtextwithattr("—‚l‚r –¾’©   ",attr)

    attr.color=0x800080
    attr.fontface="‚l‚r ƒSƒVƒbƒN"
    attr.height=480;  attr.pitch_family=49;  attr.charset=128
    addtextwithattr("‚l‚r ƒSƒVƒbƒN\n",attr)

    attr.color=0xff
    addtextwithattr("Right Aligned Text\n",attr)
    @rich.setAlignment(VRRichedit::PFA_RIGHT)
  end

  
  def cred_clicked
    @rich.setTextColor 0xff
  end
  def cgreen_clicked
    @rich.setTextColor 0xff00
  end
  def cblack_clicked
    @rich.setTextColor 0
  end
  def fontset_clicked
    f=chooseFontDialog @rich.selformat
    return unless f
    @rich.selformat=f
  end

  def fontget_clicked
    p @rich.selformat
  end

  def aleft_clicked
    @rich.setAlignment VRRichedit::PFA_LEFT
  end
  def aright_clicked
    @rich.setAlignment VRRichedit::PFA_RIGHT
  end
  def acenter_clicked
    @rich.setAlignment VRRichedit::PFA_CENTER
  end
end

VRLocalScreen.start(MyForm)

