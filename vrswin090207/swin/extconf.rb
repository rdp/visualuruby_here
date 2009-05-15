require "mkmf"

=begin dll dependencies

kernel32 user32 for all

mod 	gdl32 comdlg32 comctl32 winspool ole32 uuid  olepro32
----+---------------------------------------------------------
gdi 	  o
bmp 
cvs 	  o
prn 	  o                        o
cdlg	          o                        o
menu
rsc 	  o                o
oledd                                     o     o
ole                                                     o
=end


SYSTEM_LIBRARIES = [
  "olepro32", # for OleLoadPicture
  "uuid",
  "ole32",
  "winspool",
  "comctl32",
  "comdlg32",
  "gdi32",
  "user32",
  "kernel32",
]

lacking = []
SYSTEM_LIBRARIES.each do |lib|
# Thanks Suketa-san
  lacking.push lib unless have_library(lib)
end

if RUBY_VERSION>="1.9.0" and RUBY_RELEASE_DATE>="2008-03-01" then
  puts "swin supports m17n."
  $CFLAGS += " -DUNICODE "
end

if lacking.size>0 then
  STDERR.puts "ABORT: missing libraries : "<< lacking.join(",")
else
  create_makefile("swin")
end
