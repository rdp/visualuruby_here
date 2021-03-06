
require 'vr/vrcomctl'
require 'vr/vrtwopane'

module MyForm
  include VRHorizTwoPane

  def construct
    addPanedControl(VRTreeview,"tv1","TEST1")
    addPanedControl(VRListview,"lv1","TEST2")

    @lv1.insertColumn(0,"名前",100)
    @lv1.insertColumn(1,"読み",100)
    @lv1.insertItem(0,["織田信長","おだのぶなが"])
    @lv1.insertItem(1,["羽柴秀吉","はしばひでよし"])

    @tv1.insertItem(WConst::TVI_ROOT,WConst::TVI_LAST,"真ん中の区切り目は移動できます",128,0)

    p=@tv1.insertItem(WConst::TVI_ROOT,WConst::TVI_LAST,"さようなものですか",128,0)
    @tv1.insertItem( p              ,WConst::TVI_LAST,"さよう",128,2)
    @tv1.insertItem( p              ,WConst::TVI_LAST,"そうではござらぬ",128,1)
  end
end

VRLocalScreen.showForm(MyForm)
VRLocalScreen.messageloop

