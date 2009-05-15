#!ruby

CUI = ARGV.include?("-cui")    # use GUI or CUI?

INSTALL_TITLE = "Project VisualuRuby(tmpname) vruby installer"
BACKSCREEN_TITLE = "VisualuRuby (vruby part)   Installer"

require 'rbconfig'
require 'Win32API'
require 'ftools'

MessageBox = Win32API.new("user32","MessageBoxA",["I","P","P","I"],"I")
def ErrorMessage(msg)
  if CUI then
    puts msg
  else
    MessageBox.call 0,msg,"Install Problem",0
  end
  exit
end

class Installer
  attr :systeminfo
  attr :installcommands

  CONFIG = Config::CONFIG

  def initialize
    @systeminfo=[]
    @installcommands=[]
    examine
  end

  def copyfile(src,dst)
#    @installcommands.push "filecopy '#{src}','#{dst}'"
    @installcommands.push "File.cp '#{src}',\t'#{dst}'"
  end

  def copydir(srcdir,dstdir)  # need dstdir existence
    Dir.open(srcdir).each do |srcfile|
      if srcfile=='.' or srcfile=='..' then next end
      longsrcfile    = File.join(srcdir,srcfile)
      longtargetfile = File.join(dstdir,srcfile)
      if FileTest.file?(longsrcfile) then
        copyfile(longsrcfile,longtargetfile)
      elsif FileTest.directory?(longsrcfile) then
        unless FileTest.directory?(longtargetfile) then
          @installcommands.push "Dir.mkdir '#{longtargetfile}'"
        end
        copydir(longsrcfile,longtargetfile)
      end
    end
  end

  def install!
    size = @installcommands.size
    step=0
    @installcommands.each_with_index do |cmd,i|
      step+=1
      begin
        eval cmd
        yield cmd,step*100/size,false if iterator?
      rescue
        yield cmd,step*100/size,true if iterator?
      end
    end
  end


  def examine
    targetdir = File.join(CONFIG['sitedir'],
                        "#{CONFIG['MAJOR']}.#{CONFIG['MINOR']}")
    vrubydir =  File.join(targetdir,'vr')
    @systeminfo.push ["ruby version",RUBY_VERSION]
    @systeminfo.push ["ruby platform",RUBY_PLATFORM]
    @systeminfo.push ["current swin version",SWin::VERSION]
    @systeminfo.push ["target directory",targetdir]
    
    # vruby installdir check
    vralready= File.exist?(vrubydir)
    @systeminfo.push ["the first install?","#{(vralready)?  'no':'yes' }"]
#    @installcommands.push "Dir.mkdir('#{vrubydir}')" unless vralready

    # files to be installed
    if File.directory?("./vr") then
      copydir('./vr',vrubydir) # old 
    else
      copydir('./lib',targetdir) # new
    end

    # DLLManager required?  ruby before 1.4.3 needs it
    rubyver="#{CONFIG['MAJOR']}.#{CONFIG['MINOR']}.#{CONFIG['TEENY']}"
    dllmanreq=(rubyver<"1.4.4")
    @systeminfo.push ["ruby version in rbconfig",rubyver.to_s]
#    @systeminfo.push ["dll problem probable","#{(dllmanreq)? 'yes':'no'}"]

#    copyfile "DLLManager.rb",File.join(targetdir,"DLLManager.rb") if dllmanreq
  end

end
#############################


begin 
  require 'swin'
rescue
  ErrorMessage "swin not installed\nPlease install 'swin' before installing vruby."
  exit
end



###########################
#
# CUI installer
#

if CUI then
  print "                 Information FYI\n"
  i = Installer.new
  i.systeminfo.each do |item,value|
    print format("%25s : %s\n",item,value)
  end
  
  while true
    printf "\n\nDo you proceed to install? (Yes/View/No) [y/v/n]:"
    c = $stdin.gets[0,1].upcase

    case c
    when 'Y'
      i.install! do |cmd,progress,err|
        if err then
          print "Failed : ",cmd,"\n"
        end
      end
      print "Install finished"
      exit
    when 'V'
      print i.installcommands.join("\n")
    else
      print "Abort.\n"
      exit
    end
  end
end


###########################
#
# GUI installer
#

$:.push File.join(File.dirname(__FILE__),"lib")

begin
  require 'vr/vruby'  # also for swin version check
rescue
  ErrorMessage "swin version may be too old"
  exit
end
require 'vr/vrcontrol'
require 'vr/vrcomctl'
require 'vr/vrhandler'
require 'vr/vrdialog'

SWIN_NEWER = ( SWin::VERSION >= "010716" )

MB_TOPMOST = 0x00040000

class JobDialog < VRModalDialog
  PARAMS=[]

  def construct
    self.caption="Installing Jobs"
    width=640; height=400
    dtop = VRLocalScreen.desktop
    move( (dtop.w-width)/2,(dtop.h-height)/2,width,height)
    cw=clientrect()
    cwidth,cheight=cw[2]-cw[0],cw[3]-cw[1]
    addControl VRText,"job","",0,0,cwidth,cheight-30,WStyle::WS_VSCROLL|0x800
    @font= @screen.factory.newfont("tahoma",12)
    @job.setFont @font
    @job.text = PARAMS[0].join("\r\n")
    addControl VRButton,"btn","close",0,cheight-28,cwidth,25
    @btn.focus
  end

  def btn_clicked
    close 0
  end
end

#class InstallerDialog < VRModalDialog
module InstallerDialog
  def construct
    self.top(-1)
    width=440; height=250
    dtop = VRLocalScreen.desktop
    
    self.caption = INSTALL_TITLE
    move( (dtop.w-width)/2,(dtop.h-height)/2,width,height )

    description="This installer script is written in vruby"

    addControl VRStatic,"ll",description,80,5,350,25
    addControl VRStatic,"l0","Information FYI",150,35,150,25
    addControl VRListview,"lv","",5,60,420,120
    addControl VRProgressbar,"pg","",10,190,90,25,1
    addControl VRButton,"ok","Install!",110,190,100,25
    addControl VRButton,"view","View Job",220,190,100,25
    addControl VRButton,"cancel","Cancel",330,190,100,25

    @lv.insertColumn 0,"item",130
    @lv.insertColumn 1,"value",280
    @ok.focus
    
    @installer = Installer.new
    viewinfo(@installer)
  end

  def viewinfo(installer)
    installer.systeminfo.each do |item,value|
      @lv.addItem [item,value]
    end
  end

  def cancel_clicked
    self.close 1
  end

  def ok_clicked
    @pg.setRange 0,100
    Thread.abort_on_exception =true    # Thanks to arton
    Thread.new {
      Thread.critical=true
      @installer.install! do |cmd,progress,err|
        @pg.position = progress
        if err then
          messageBox "Failed\n"+cmd,"vrinstaller",MB_TOPMOST
        end
      end
      messageBox "Install FINISHED","installer",MB_TOPMOST
      self.close 0
      Thread.critical=false
    }
  end

  def view_clicked
#    messageBox @installer.installcommands.join("\n"),"Installing Jobs"
     JobDialog::PARAMS[0]=@installer.installcommands
     VRLocalScreen.modalform self,nil,JobDialog
  end

end



class BackScreen < VRForm
  include VRDrawable

  def construct
    self.top(0) if SWIN_NEWER # using modaldialog
    @font= @screen.factory.newfont("tahoma",30,1)  #1=SWin::Font::ITALIC
    @bmp = SWin::Bitmap.newBitmap(1,64)
    0.upto(63) do |c|
      @bmp[0,63-c]=[0xa0-c*2,0,0]
    end
  end

  def self_paint
    stretchBitmap 0,0,self.w,self.h,@bmp
    setFont @font
    self.opaque=false
    self.textColor=RGB(0,0,0)
    drawText BACKSCREEN_TITLE,18,18,1000,100
    self.textColor=RGB(0xff,0xff,0xff)
    drawText BACKSCREEN_TITLE,10,10,1000,100
  end
end

style= 0x10000000 | 0x80000000

f = VRLocalScreen.newform(nil,style, BackScreen)
f.move 0,0,VRLocalScreen.width,VRLocalScreen.height
f.create.show

if SWIN_NEWER then
  VRLocalScreen.modalform f,nil,InstallerDialog
  VRLocalScreen.application.doevents  # consume the remains of messages
else
  i = VRLocalScreen.newform f,nil,InstallerDialog
  i.create.show
  VRLocalScreen.messageloop
end