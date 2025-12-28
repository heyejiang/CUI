#pragma once
#include "GUI/Button.h"
#include "GUI/CheckBox.h"
#include "GUI/ComboBox.h"
#include "GUI/Control.h"
#include "GUI/Form.h"
#include "GUI/GridView.h"
#include "GUI/Label.h"
#include "GUI/Layout/DockPanel.h"
#include "GUI/Layout/GridPanel.h"
#include "GUI/Layout/RelativePanel.h"
#include "GUI/Layout/StackPanel.h"
#include "GUI/Layout/WrapPanel.h"
#include "GUI/Menu.h"
#include "GUI/Panel.h"
#include "GUI/PasswordBox.h"
#include "GUI/PictureBox.h"
#include "GUI/ProgressBar.h"
#include "GUI/RadioBox.h"
#include "GUI/RichTextBox.h"
#include "GUI/Slider.h"
#include "GUI/Switch.h"
#include "GUI/TabControl.h"
#include "GUI/TextBox.h"
#include "GUI/TreeView.h"
#include "GUI/WebBrowser.h"

class DemoWindow1 : public Form
{
private:
	TabControl* tabControl1;
	Label* label1;
	Button* button1;
	TextBox* textBox1;
	PasswordBox* passwordBox1;
	RichTextBox* richTextBox1;
	CheckBox* checkBox1;
	RadioBox* radioBox1;
	ComboBox* comboBox1;
	ProgressBar* progressBar1;
	Slider* slider1;
	Switch* switch1;
	GridView* gridView15;
	TreeView* treeView16;
	PictureBox* pictureBox1;
	StackPanel* stackPanel18;
	Button* button22;
	Button* button19;
	Button* button20;
	Label* label21;
	Label* label23;
	GridPanel* gridPanel24;
	Button* button25;
	Button* button26;
	Button* button27;
	Button* button28;
	Button* button29;
	Button* button30;
	Button* button31;
	Button* button32;
	Button* button33;
	Button* button34;
	Button* button35;
	Button* button36;
	Button* button37;
	Button* button38;
	Button* button39;
	Button* button52;
	DockPanel* dockPanel41;
	Button* button53;
	WrapPanel* wrapPanel43;
	Button* button44;
	Button* button46;
	Button* button45;
	Button* button47;
	RelativePanel* relativePanel48;
	Button* button50;
	Button* button49;
	Panel* panel1;
	Button* button54;
	WebBrowser* webBrowser52;
	Menu* menu1;
	TabPage* tabPage1;
	TabPage* tabPage2;
	TabPage* tabPage3;

	void tabControl1_OnSelectedChanged(Control* sender);
	void button1_OnMouseClick(Control* sender, MouseEventArgs e);
	void textBox1_OnTextChanged(Control* sender, std::wstring oldText, std::wstring newText);
	void slider1_OnValueChanged(Control* sender, float oldValue, float newValue);
	void switch1_OnChecked(Control* sender);
	void menu1_OnMenuCommand(Control* sender, int id);

public:
	DemoWindow1();
	virtual ~DemoWindow1();
};
