#!/usr/bin/env ruby

require 'vr/vrcontrol'
require 'vr/vrlayout'
require 'vr/vrtray'

module MyForm

  include VRTrayiconFeasible

  LoadIcon=    Win32API.new('user32', 'LoadIcon', 'II', 'I')
  INFO_ICON=   LoadIcon.call(0, 32516)

  def construct
    @title= addControl(VREdit, 'title', 'Balloon title')
    @text= addControl(VREdit, 'text', 'Balloon text')
    @info= addControl(VRRadiobutton, 'info_rb', 'Info icon')
    @info.check(true)
    @warning= addControl(VRRadiobutton, 'warning_rb', 'Warning  icon')
    @error= addControl(VRRadiobutton, 'error_rb', 'Error icon')
    @none= addControl(VRRadiobutton, 'none_rb', 'No icon')
    addControl(VRButton, 'show', 'Show balloon')
    addControl(VRButton, 'exit', 'Exit')
    create_trayicon(INFO_ICON, 'VRuby Balloon Tray Test') # last parameter is the tooltip. see traytest.rb
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
    puts 'Balloon timed out or user dismissed it or it was replaced by a new balloon!'
  end

  def self_trayballoonclicked iconid
    puts 'User clicked inside balloon!'
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
    modify_trayicon5(nil, @title.text, @text.text, infoicon)
  end

end

frm= VRLocalScreen.newform
frm.caption= 'Balloon Tooltip Test'
frm.extend(MyForm)
frm.extend(VRVertLayoutManager)
frm.move(100, 100, 300, 200)
frm.create.show
VRLocalScreen.messageloop

# see \vr\contrib\vrtrayballoon.rb for a more complete list of icons/messages 
# note that if you create balloons with the same text and they *once* click on the x, it will not display again...something like that
