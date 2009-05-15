
require 'vr/vrcomctl'
require 'vr/vrtwopane'

module MyForm
  include VRHorizTwoPane

  def construct
    addPanedControl(VRTreeview,"tv1","TEST1")
    addPanedControl(VRListview,"lv1","TEST2")

    @lv1.insertColumn(0,"–¼‘O",100)
    @lv1.insertColumn(1,"“Ç‚İ",100)
    @lv1.insertItem(0,["D“cM’·","‚¨‚¾‚Ì‚Ô‚È‚ª"])
    @lv1.insertItem(1,["‰HÄG‹g","‚Í‚µ‚Î‚Ğ‚Å‚æ‚µ"])

    @tv1.insertItem(WConst::TVI_ROOT,WConst::TVI_LAST,"^‚ñ’†‚Ì‹æØ‚è–Ú‚ÍˆÚ“®‚Å‚«‚Ü‚·",128,0)

    p=@tv1.insertItem(WConst::TVI_ROOT,WConst::TVI_LAST,"‚³‚æ‚¤‚È‚à‚Ì‚Å‚·‚©",128,0)
    @tv1.insertItem( p              ,WConst::TVI_LAST,"‚³‚æ‚¤",128,2)
    @tv1.insertItem( p              ,WConst::TVI_LAST,"‚»‚¤‚Å‚Í‚²‚´‚ç‚Ê",128,1)
  end
end

VRLocalScreen.showForm(MyForm)
VRLocalScreen.messageloop

