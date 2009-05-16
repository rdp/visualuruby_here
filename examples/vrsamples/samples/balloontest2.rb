# slightly more complicated balloon

require 'vr/vrcontrol'
require 'vr/vrlayout'
require 'vr/vrtray'
require 'vr/vrtimer'
require 'go' # ruby-wmi
require 'pp'

module MyForm

  include VRTrayiconFeasible
  include VRTimerFeasible

  LoadIcon=    Win32API.new('user32', 'LoadIcon', 'II', 'I')
  INFO_ICON=   LoadIcon.call(0, 32516)

  def construct
    # create icon number 0
    create_trayicon(INFO_ICON, 'VRuby Balloon Tooltip Test') # last parameter is the tooltip. see traytest.rb

    self_timer
    addTimer 1000
  end

  def self_timer
   infoicon = VRTrayiconFeasible::NIIF_NONE

   procs = WMI::Win32_Process.find(:all)
   text = ""
   for proc in procs.sort_by{|p| p.WorkingSetSize.to_i}.reverse[0..5]
     text += "#{proc.Name}: #{proc.WorkingSetSize}\n"
   end

   modify_trayicon5(nil, 'title', text, infoicon, 0.01, 0)
  end

  def exit_clicked
    delete_trayicon
    self.close
  end

  def self_trayballoonshow iconid
    puts 'Balloon shown!'
  end

  def self_trayballoonhide iconid
    puts 'Balloon hidden!'
  end

  def self_trayballoontimeout iconid
    puts 'Balloon timed out or user dismissed it!'
  end

  def self_trayballoonclicked iconid
    puts 'User clicked inside balloon!'
    exit(0)
  end

  def show_clicked
    # check which radio button is checked, get its appropriate icon
    infoicon = [:info, :warning, :error, :none].inject(NIIF_INFO) do |icon, which|
      if instance_variable_get("@#{which}").checked?
        # NIIF_xxxx is available icons for balloon
        VRTrayiconFeasible.const_get("NIIF_#{which.to_s.upcase}")
      else
        icon
      end
    end
    modify_trayicon5(nil, @title.text, @text.text, infoicon, 0.01, 0)
  end

end

frm= VRLocalScreen.newform
frm.caption= 'Balloon Tooltip Test'
frm.extend(MyForm)
frm.extend(VRVertLayoutManager)
frm.move(100, 100, 300, 200)
frm.create
VRLocalScreen.messageloop

# see \vr\contrib\vrtrayballoon.rb for a more complete list of icons/messages 
