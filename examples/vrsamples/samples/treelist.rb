require 'vr/vrcontrol'
require 'vr/vrcomctl'
require 'vr/vrtwopane'
require 'vr/rscutil'

module FSImageList
  Imagelist = VRLocalScreen.factory.newimagelist 13,13

  OpenImg = Imagelist.add  SWin::Bitmap.loadString(<<EEOOSS
BAN1OgEMU1dpbjo6Qml0bWFwAd5CTd4AAAAAAAAAdgAAACgAAAANAAAADQAA
AAEABAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAAACAAAAAgIAA
gAAAAIAAgACAgAAAgICAAMDAwAAAAP8AAP8AAAD//wD/AAAA/wD/AP//AAD/
//8A////////8ADwAAAAAP/wAACIiIiID/AACA+/v78P+l8IC/v7+/DwAAiA
v7+/sPAACID7+/v7AAAIiAAAAAAAAAiIiIiID/AACIiAAAD/8ADwAA/////w
AP////////AA////////8AA=
EEOOSS
)
  ClosedImg = Imagelist.add SWin::Bitmap.loadString( <<EEOOSS
BAN1OgEMU1dpbjo6Qml0bWFwAd5CTd4AAAAAAAAAdgAAACgAAAANAAAADQAA
AAEABAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAAACAAAAAgIAA
gAAAAIAAgACAgAAAgICAAMDAwAAAAP8AAP8AAAD//wD/AAAA/wD/AP//AAD/
//8A////////8AD////////wAPAAAAAAAPAACIiIiIiA+l8L+/v7+/DwAA+/
v7+/sPAAC/v7+/vw8AAPv7+/v7DwAAv7+/v78PAAAAAAAAAP8ADwv78P///w
AP8AAP////AA////////8AA=
EEOOSS
)
end

module SmallFileImageList
  Imagelist = VRLocalScreen.factory.newimagelist(13,13)

  LeafImg = Imagelist.add SWin::Bitmap.loadString( <<EEOOSS
BAN1OgEMU1dpbjo6Qml0bWFwAd5CTd4AAAAAAAAAdgAAACgAAAANAAAADQAA
AAEABAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAAACAAAAAgIAA
gAAAAIAAgACAgAAAgICAAMDAwAAAAP8AAP8AAAD//wD/AAAA/wD/AP//AAD/
//8A////////8ADwAAAAAA/wAPD/////D/AA8P////8P+l/w/0RE/w/wAPD/
////D/AA8P9ERP8P8ADw/////w/wAPD/RE8AD/AA8P///wgP8ADw////AP/w
APAAAAAP//AA////////8AA=
EEOOSS
)
end

module FileImageList
  Imagelist = VRLocalScreen.factory.newimagelist
 
  LeafImg = Imagelist.add  SWin::Bitmap.loadString( <<EEOOSS
BAN1OgEMU1dpbjo6Qml0bWFwAb5CTb4AAAAAAAAAPgAAACgAAAAgAAAAIAAA
AAEAAQAAAAAAgAAAAMQOAADEDgAAAAAAAAAAAAAAAAAA////AP//////////
//////wAAA/9///v/f//7/3//+/9wABv/f//7/3//+/9gABv/f//7/3//+/9
/+Hv/f//7/3gH+/9///v/f//7/3//+/9gA/v/f//7/3/+O/9//cP/f/3n/3/
7n/9/+n//f/H//wAH///////////////////////
EEOOSS
)
end

class DirListview <VRListview
  def vrinit
    super
    setImagelist FileImageList::Imagelist
    setImagelist SmallFileImageList::Imagelist,1
    addColumn("name",120)
    insertColumn(1,"ext",50)
    addColumn("size",80)
    addColumn("atime",120)
#    insertMultiItems(0,[ [["test2","dat"],0], [["foo","bar"],1] ])
  end

  def setdir(dir)
    clearItems
    Dir[dir+"/*"].each do |f|
      fn  = File.basename(f)
      ext = fn.scan(/\..*$/)[0]
      ext = "" unless ext
      begin
#        raise RuntimeError   # This script is too slow for c:/windows !!
        atime = File.atime(f).to_s
        size = File.size(f)
      rescue
        atime = ""
        size = ""
      end
      p = addItem([fn[0..-1-ext.length],ext[1..-1],size,atime])
      setItemIconOf(p,FileImageList::LeafImg)
    end
  end
end

class DirTreeview <VRTreeview
  def vrinit
    super
    setImagelist FSImageList::Imagelist
  end

  def adddir(parent,dir)
#print "::",dir,"\n"
    Dir[dir+"/*"].each do |f|
      if File.directory?(f) then
        p=addItem(parent,File.basename(f))
        setItemIconOf(p,FSImageList::ClosedImg,FSImageList::OpenImg)
        adddir(p,f)
      end
    end
  end

  def setdir(dir)
    adddir(root,dir)
  end
end


module MyForm
  include VRMenuUseable
  include VRHorizTwoPane

  attr_accessor :topdir

  def construct
    self.move(10,10,620,420)
    self.caption="TreeList TEST"
    addPanedControl(DirTreeview,"tv1","tv")
    addPanedControl(DirListview,"lv1","lv")
    setMenu newMenu.set( [["&Test",[
                           ["c&lear Columns","clearc"],
                           ["&Count Columns","countc"],
                           ["&Add Item","aitem"],
                           ["c&hange Item","citem"],
                           ["&Selected Items","sitem"],
                           ["&Listview",[
                             ["&icon","iconview"],
                             ["&report","reportview"],
                             ["&smallicon","smallview"],
                             ["l&ist","listview"],
                             ["&LoadDir","loaddir"]
                           ]],
                           ["&Treeview",[
                             ["&select top","tselect"]
                           ]]
                          ]
                        ]] )
    @basedir=@topdir
    @tv1.setdir(@basedir)
    @lv1.setdir(@basedir)
  end

  def tv1_selchanged(hitem,lparam)
    dir= @tv1.getItemTextOf(hitem)
    r=hitem
    while r=@tv1.getParentOf(r) do
      dir= @tv1.getItemTextOf(r)+"/"+dir
    end
    dir=@basedir+"/"+dir
    @lv1.setdir(dir)
  end

  def lv1_dblclicked
    sl=@lv1.focusedItem
    messageBox @lv1.getItemTextOf(sl) if @lv1.focused?(sl)
  end


############
# for tests

  def clearc_clicked
    messageBox "Œ³‚É–ß‚¹‚Ü‚¹‚ñ"
    @lv1.clearColumns
  end

  def countc_clicked
    messageBox "COUNT#{@lv1.countColumns}"
  end

  def aitem_clicked
    @lv1.insertItem(0,["test","dat"])
  end

  def citem_clicked
    @lv1.setItemTextOf(0,0,"TeSt")
    @lv1.selectItem(0,true)
  end

  def sitem_clicked
    @lv1.eachSelectedItems do |i|
      messageBox @lv1.getItemTextOf(i,0)
    end
  end

  def iconview_clicked() @lv1.iconview end
  def reportview_clicked() @lv1.reportview end
  def smallview_clicked() @lv1.smalliconview end
  def listview_clicked() @lv1.listview end

  def tselect_clicked
    @tv1.selectItem(@tv1.topItem)
  end
end

frm=VRLocalScreen.newform
frm.extend MyForm
frm.topdir = if ARGV[0] then ARGV[0] else "c:/mydocu~1/" end
frm.create.show
VRLocalScreen.messageloop

