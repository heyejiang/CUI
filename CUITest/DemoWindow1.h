#pragma once
#include "GUI/Button.h"
#include "GUI/Control.h"
#include "GUI/Form.h"
#include "GUI/GridView.h"
#include "GUI/Label.h"
#include "GUI/Layout/WrapPanel.h"
#include "GUI/RichTextBox.h"
#include "GUI/TreeView.h"

class DemoWindow1 : public Form
{
private:
	Label* label1;
	Button* button2;
	RichTextBox* richTextBox3;
	WrapPanel* wrapPanel4;
	Button* button5;
	Button* button6;
	Label* label7;
	Label* label8;
	GridView* gridView9;
	TreeView* treeView10;

public:
	DemoWindow1();
	virtual ~DemoWindow1();
};
