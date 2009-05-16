These are sample script for VisualuRuby Project(temp-name).
You need VisualuRubyProject's archives to examine them.
See http://www.threeweb.ad.jp/~nyasu/software/vrproject.html .
(Download page is at http://www.threeweb.ad.jp/~nyasu/vruby/vruby/core.html)

See ../../README if things crash

FILES:
margintest.rb
  This is a sample of vrmargin which used in VRTwoPaneFrame and 
  VRHolizLayoutFrame.
paneltest.rb
  The window captures window size changing and moves Groupbox to left and right.
layouttest.rb
  This opens three windows which lays their buttons out vertically and 
  horizontally, and which lays with these combination.
  The layout managers handle window-resize event to re-arrange its child windows.
customed.rb
  This hooks the control's event into the control itself.
peventtest.rb
  This is a sample of handling events of grand-child controls on the panel 
  which is on the window itself.
subclstest.rb
  This makes EditControl Subclass-ed.
gpaneltest.rb
  The window draws graphs for drawing sample.
gridtest.rb
  This is a test script for grid layout manager.
  And this prints current time on the title bar in its idling loop procedure.
comctltest2.rb
  This is a Common Control(Listview,Treeview) sample.
menutest.rb
  This opens a window which is using menues and handles menu-click events.
dlgtest.rb
  You can view bitmap files selected by common dialog invoked
  by the menu [File->Open]
dlgbmptest.rb
  This is a sample for opening common dialogs.
  And test the original VRBitmapPanel control which is displaying bitmap.
ctrlc.rb
  This is test script for handling Ctrl-C interrupts.
  There are 2 buttons both that starts time consuming process.
  The upper button starts the process which are rescued from Ctrl-C,
  and the other starts the non-rescued process.
mediatest.rb
  This is a sample for VRMMediaViewer to playing multimedia files using MCI.
canvastest.rb
  This is sample of drawable bitmap.
  Drawing pad window opens.
panetest.rb
  This opens a window which has Treeview and Listview separated by movable
  separator. I know this is not a beautiful gadget. Are there clever kind and
  persons to teach me how to make beautiful one?
droptest.rb
  This opens a window with Edit Control.
  You can drop text files on this window to view that text file.
  Your multi-file dropping opens plural windows.
treelist.rb
  Treeview and Listview test like explorer. This can have an argument of 
  start directory.
fontclock.rb
  Font test. This is a clock where you can change the font.
ownerdraw.rb
  Owner draw test. A colored rectangle is drawn at the left-top corner of 
  the button.
ddeexectest.rb
  DDE_EXECUTE test. At first, click "connect" button to connect with Netscape.
  And select the URL from the combobox to navigate the netscape.
ftest.rb
  Change the font of button into big one.
mdlgtest.rb
  Sample for using modal dialogs.
tabtest.rb
  Sample of using tab control.
richtest.rb
  RichEdit control sample to change font,color and style of character.
clipboardtest.rb
  Sample about clipboard.
  This is an utility making clipboard history.
printer.rb
  Sample of using printer.
relaytest.rb
  Sample of relaying event from control on a panel to the parent of the panel.
arraytest.rb
  Sample using control array with layout manager.
tooltiptest.rb
  Sample of using tooltip window.
lframetest.rb
  Sample using layout frames.
rebartest.rb
  Sample of using rebar control and layoutframes.
paneframetest.rb
  Sample of using two-paned frames.
sbtest.rb
  Sample of using Scrollbars.
