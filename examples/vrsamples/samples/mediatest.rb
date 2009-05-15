require 'vr/vrcontrol'
require 'vr/vrmmedia.rb'

module MyForm
  def construct
    self.caption="MediaView Sample"
    addControl(VRButton,"open", "Open",   10,10,50,20)
    addControl(VRButton,"play", "Play",   60,10,50,20)
    addControl(VRButton,"pause","Pause", 110,10,50,20)
    addControl(VRButton,"stop", "Stop",  160,10,50,20)
    addControl(VRButton,"close","Close", 210,10,50,20)
    addControl(VRButton,"mode", "M",     260,10,15,20)
    addControl(VRButton,"exit", "X",     275,10,15,20)

    addControl(VRButton,"volup",  "V UP",  300,10,50,20)
    addControl(VRButton,"voldwn", "V DWN", 350,10,50,20)

    addControl(VRStatic, "label","",10,30,200,50)
    addControl(VRMediaView,"mm","mm",10,80,100,10)

  end

  def open_clicked
    @mm.mediaclose
    fn=openFilenameDialog
    @mm.mediaopen(fn) if fn
  end

  def play_clicked
    @mm.play
  end

  def pause_clicked
    @mm.pause
  end

  def stop_clicked
    @mm.stop
  end

  def close_clicked
    @mm.close
  end

  def mode_clicked
    messageBox @mm.mode,(@mm.playable?)?"playable" : "not-playable",0
  end

  def exit_clicked
    self.close
  end

  def volup_clicked
    @mm.volume = @mm.volume+50
  end

  def voldwn_clicked
    @mm.volume = @mm.volume-50
  end

  def mm_modechanged(n)
    @label.caption="mode:"+@mm.modestring(n)
    @label.refresh
  end

  def mm_onerror(n)
    @label.caption=@mm.errorstring
  end

  def mm_sizechanged()
#    p "size changed"
  end

  def mm_mediachanged(n)
    @label.caption = "New : "+n
  end

end


VRLocalScreen.showForm(MyForm)
VRLocalScreen.messageloop

