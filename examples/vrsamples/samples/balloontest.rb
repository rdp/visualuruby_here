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
    create_trayicon(INFO_ICON, 'VRuby Balloon Tooltip Test')
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
  end

  def show_clicked
    infoicon= [:info, :warning, :error, :none].inject(NIIF_INFO) do |icon, which|
      if instance_variable_get("@#{which}").checked?
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
