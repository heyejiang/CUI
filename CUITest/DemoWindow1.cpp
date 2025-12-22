#include "DemoWindow1.h"

DemoWindow1::DemoWindow1()
	: Form(L"Form", POINT{ 100, 100 }, SIZE{ 800, 600 })
{

	// 创建控件
	// label1
	label1 = new Label(L"标签", 0, 0);
	label1->Size = {120, 20};
	label1->BackColor = D2D1::ColorF(0.f, 0.f, 0.f, 0.f);
	label1->ForeColor = D2D1::ColorF(0.f, 0.f, 0.f, 1.f);
	label1->BolderColor = D2D1::ColorF(0.f, 0.f, 0.f, 1.f);

	// button2
	button2 = new Button(L"按钮", 0, 23, 120, 30);
	button2->BackColor = D2D1::ColorF(0.8039f, 0.7882f, 0.7882f, 1.f);
	button2->ForeColor = D2D1::ColorF(0.f, 0.f, 0.f, 1.f);
	button2->BolderColor = D2D1::ColorF(0.5451f, 0.5373f, 0.5373f, 1.f);

	// richTextBox3
	richTextBox3 = new RichTextBox(L"", 0, 53, 300, 57);
	richTextBox3->BackColor = D2D1::ColorF(0.8275f, 0.8275f, 0.8275f, 1.f);
	richTextBox3->ForeColor = D2D1::ColorF(0.f, 0.f, 0.f, 1.f);
	richTextBox3->BolderColor = D2D1::ColorF(0.f, 0.f, 0.f, 1.f);

	// wrapPanel4
	wrapPanel4 = new WrapPanel(0, 110, 300, 200);
	wrapPanel4->BackColor = D2D1::ColorF(0.9098f, 0.9098f, 0.9098f, 1.f);
	wrapPanel4->ForeColor = D2D1::ColorF(0.f, 0.f, 0.f, 1.f);
	wrapPanel4->BolderColor = D2D1::ColorF(0.f, 0.f, 0.f, 1.f);
	wrapPanel4->SetOrientation(Orientation::Horizontal);
	wrapPanel4->SetItemWidth(0.f);
	wrapPanel4->SetItemHeight(0.f);

	// button5
	button5 = new Button(L"按钮", 0, 0, 120, 30);
	button5->BackColor = D2D1::ColorF(0.8039f, 0.7882f, 0.7882f, 1.f);
	button5->ForeColor = D2D1::ColorF(0.f, 0.f, 0.f, 1.f);
	button5->BolderColor = D2D1::ColorF(0.5451f, 0.5373f, 0.5373f, 1.f);

	// button6
	button6 = new Button(L"按钮", 120, 0, 120, 30);
	button6->BackColor = D2D1::ColorF(0.8039f, 0.7882f, 0.7882f, 1.f);
	button6->ForeColor = D2D1::ColorF(0.f, 0.f, 0.f, 1.f);
	button6->BolderColor = D2D1::ColorF(0.5451f, 0.5373f, 0.5373f, 1.f);

	// label7
	label7 = new Label(L"标签", 0, 30);
	label7->Size = {120, 20};
	label7->BackColor = D2D1::ColorF(0.f, 0.f, 0.f, 0.f);
	label7->ForeColor = D2D1::ColorF(0.f, 0.f, 0.f, 1.f);
	label7->BolderColor = D2D1::ColorF(0.f, 0.f, 0.f, 1.f);

	// label8
	label8 = new Label(L"标签", 120, 30);
	label8->Size = {120, 20};
	label8->BackColor = D2D1::ColorF(0.f, 0.f, 0.f, 0.f);
	label8->ForeColor = D2D1::ColorF(0.f, 0.f, 0.f, 1.f);
	label8->BolderColor = D2D1::ColorF(0.f, 0.f, 0.f, 1.f);

	// gridView9
	gridView9 = new GridView(0, 310, 360, 200);
	gridView9->BackColor = D2D1::ColorF(0.9098f, 0.9098f, 0.9098f, 1.f);
	gridView9->ForeColor = D2D1::ColorF(0.f, 0.f, 0.f, 1.f);
	gridView9->BolderColor = D2D1::ColorF(0.f, 0.f, 0.f, 1.f);
	gridView9->Columns.Clear();
	gridView9->Columns.Add(GridViewColumn(L"c1", 120.f, ColumnType::Text, true));
	gridView9->Columns.Add(GridViewColumn(L"c2", 120.f, ColumnType::Text, true));
	gridView9->Columns.Add(GridViewColumn(L"c3", 120.f, ColumnType::Check, true));

	// treeView10
	treeView10 = new TreeView(300, 53, 220, 257);
	treeView10->BackColor = D2D1::ColorF(0.9098f, 0.9098f, 0.9098f, 1.f);
	treeView10->ForeColor = D2D1::ColorF(0.f, 0.f, 0.f, 1.f);
	treeView10->BolderColor = D2D1::ColorF(0.f, 0.f, 0.f, 1.f);
	// TreeView nodes
	for (auto n : treeView10->Root->Children) delete n;
	treeView10->Root->Children.Clear();
	auto* treeView10_node1 = new TreeNode(L"1");
	treeView10_node1->Expand = true;
	treeView10->Root->Children.push_back(treeView10_node1);
		auto* treeView10_node2 = new TreeNode(L"2");
		treeView10_node1->Children.push_back(treeView10_node2);
		auto* treeView10_node3 = new TreeNode(L"2");
		treeView10_node1->Children.push_back(treeView10_node3);
		auto* treeView10_node4 = new TreeNode(L"2");
		treeView10_node1->Children.push_back(treeView10_node4);
	auto* treeView10_node5 = new TreeNode(L"1");
	treeView10_node5->Expand = true;
	treeView10->Root->Children.push_back(treeView10_node5);
		auto* treeView10_node6 = new TreeNode(L"2");
		treeView10_node5->Children.push_back(treeView10_node6);
		auto* treeView10_node7 = new TreeNode(L"2");
		treeView10_node5->Children.push_back(treeView10_node7);
		auto* treeView10_node8 = new TreeNode(L"2");
		treeView10_node5->Children.push_back(treeView10_node8);

	// 组装控件层级（包含布局容器）
	this->AddControl(label1);

	this->AddControl(button2);

	this->AddControl(richTextBox3);

	this->AddControl(wrapPanel4);
		wrapPanel4->AddControl(button5);
		button5->HAlign = HorizontalAlignment::Left;
		button5->VAlign = VerticalAlignment::Top;

		wrapPanel4->AddControl(button6);
		button6->HAlign = HorizontalAlignment::Left;
		button6->VAlign = VerticalAlignment::Top;

		wrapPanel4->AddControl(label7);
		label7->HAlign = HorizontalAlignment::Left;
		label7->VAlign = VerticalAlignment::Top;

		wrapPanel4->AddControl(label8);
		label8->HAlign = HorizontalAlignment::Left;
		label8->VAlign = VerticalAlignment::Top;

	wrapPanel4->PerformLayout();

	this->AddControl(gridView9);

	this->AddControl(treeView10);

}

DemoWindow1::~DemoWindow1()
{
}
