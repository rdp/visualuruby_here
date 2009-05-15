require 'vr/vrcontrol'
require 'vr/vrlayout'


class GraphControlPanel < VRPanel
  include VRStdControlContainer

  def construct
    addControl(VRButton,"upbtn","up",60,10,50,50)
    addControl(VRButton,"dwbtn","down",60,110,50,50)
    addControl(VRButton,"lfbtn","left",10,60,50,50)
    addControl(VRButton,"rtbtn","right",110,60,50,50)

	send_parent("upbtn","clicked")
	send_parent("dwbtn","clicked")
	send_parent("lfbtn","clicked")
	send_parent("rtbtn","clicked")
  end
end



class VRGraphPanel < VRPanel
  include VRMessageHandler
  include VRDrawable

  attr_accessor :fprocs, :xmin, :xmax, :ymin, :ymax, :xaxisy, :yaxisx, :xstep
  
  def vrinit
    super
	@xmin=-1; @xmax=1
	@ymin=-1; @ymax=1
	@xaxisy=@yaxisx=0
	@xstep=0.05
  end

  def convertprep
    @paramx=self.w/(@xmax-@xmin)
    @paramy=self.h/(@ymax-@ymin)
  end
  
  def convertx(x)
    ( (x-@xmin) * @paramx ).to_i 
  end
  def converty(y)
    ( (@ymax-y) * @paramy ).to_i 
  end

  def drawgraph(f)
    step=@xstep
    minstep=(@xmax-@xmin)/self.w
	step=minstep if step<minstep  # for fast drawing, not for detailed drawing

    x=@xmin
    grMoveTo( convertx(x),converty(f.call(x)) )
    x+=step
    
    while x<=@xmax
      grLineTo( convertx(x),converty(f.call(x)) )
	  x+=step
    end
  end

  def self_paint
    setPen(0)
    setBrush(0xffffff)
    fillRect(0,0,self.w,self.h)
    
    convertprep
    
	ax=convertx(@yaxisx)
    ay=converty(@xaxisy)
    drawLine(convertx(@xmin),ay,convertx(@xmax),ay)
    drawLine(ax,converty(@ymin),ax,converty(@ymax))

	@fprocs.each do |f|
	    setPen(f[1])
    	drawgraph(f[0])
	end

	drawText "x: [#{self.xmin}..#{self.xmax}]",
	  self.w*0.4,30,self.w*0.4+200,50
  end
  
end


###########################################
#
#  main 
#

frm=VRLocalScreen.newform
frm.extend VRVertLayoutManager


def frm.construct
  self.caption="Graph Explorer"
  
  addControl(VRGraphPanel,"graph1","GRAPH")
  addControl(GraphControlPanel,"panel","caption")
  @graph1.xmin= -6.28
  @graph1.xmax= 6.28
  @graph1.ymin= -1
  @graph1.ymax= 1
  @graph1.xstep= 0.02

  @graph1.fprocs= []
  @graph1.fprocs.push [ Proc.new { |x| Math.sin(x) } ,  0xFF0000 ]
  @graph1.fprocs.push [ Proc.new { |x| Math.sin(2*x) }, 0x008000 ]
  @graph1.fprocs.push [ Proc.new { |x| x*0.2},          0x0080FF ]
  @graph1.fprocs.push [ Proc.new { |x| 0.5+0.1*x },     0x008080 ]
end

def frm.panel_upbtn_clicked
  @graph1.ymin+=0.2
  @graph1.ymax+=0.2
  @graph1.refresh
end
def frm.panel_dwbtn_clicked
  @graph1.ymin-=0.2
  @graph1.ymax-=0.2
  @graph1.refresh
end
def frm.panel_lfbtn_clicked
  @graph1.xmin-=0.2
  @graph1.xmax-=0.2
  @graph1.refresh
end
def frm.panel_rtbtn_clicked
  @graph1.xmin+=0.2
  @graph1.xmax+=0.2
  @graph1.refresh
end


frm.create.show

VRLocalScreen.messageloop
