#include "Font.h"
#include "Factory.h"
#include <dwrite_3.h>

#pragma warning(disable: 4267)
#pragma warning(disable: 4244)
#pragma warning(disable: 4018)

Font::Font(IDWriteTextFormat* fontObject, float _fontsize) :_fontSize(_fontsize), _fontName(L""), _fontObject(fontObject)
{
	this->FontHeight = this->GetTextSize(L'I').height;
}
Font::Font(std::wstring fontFamilyName, float _fontsize)
{
	this->_fontSize = _fontsize;
	this->_fontName = fontFamilyName;
	_DWriteFactory->CreateTextFormat(
		this->_fontName.c_str(),
		NULL,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		this->_fontSize,
		L"",
		&_fontObject);
	this->FontHeight = this->GetTextSize(L'I').height;
}
Font::~Font()
{
	if (this->_fontObject) this->_fontObject->Release();
}
////要求:Windows10 - 1703
//Font* Font::CreateFontFromFile(std::wstring filePath, float _fontsize)
//{
//	HRESULT hr = S_OK;
//
//	// 创建字体集构建器
//	IDWriteFontSetBuilder1* pFontSetBuilder = NULL;
//	hr = _DWriteFactory->CreateFontSetBuilder(&pFontSetBuilder);
//	if (FAILED(hr))
//		return nullptr;
//
//	// 从文件路径创建字体文件引用
//	IDWriteFontFile* pFontFile = NULL;
//	hr = _DWriteFactory->CreateFontFileReference(
//		filePath.c_str(),
//		nullptr,
//		&pFontFile
//	);
//	if (FAILED(hr))
//		return nullptr;
//
//	// 将字体文件添加到字体集构建器
//	hr = pFontSetBuilder->AddFontFile(pFontFile);
//	if (FAILED(hr))
//		return nullptr;
//
//	// 创建字体集
//	IDWriteFontSet* pFontSet = NULL;
//	hr = pFontSetBuilder->CreateFontSet(&pFontSet);
//	if (FAILED(hr))
//		return nullptr;
//
//	// 从字体集创建字体集合
//	IDWriteFontCollection1* pFontCollection = NULL;
//	hr = _DWriteFactory->CreateFontCollectionFromFontSet(
//		pFontSet,
//		&pFontCollection
//	);
//	if (FAILED(hr))
//		return nullptr;
//
//	// 获取字体族数量
//	UINT32 familyCount = pFontCollection->GetFontFamilyCount();
//	if (familyCount == 0)
//		return nullptr;
//
//	// 获取第一个字体族
//	IDWriteFontFamily* pFontFamily = NULL;
//	hr = pFontCollection->GetFontFamily(0, &pFontFamily);
//	if (FAILED(hr))
//		return nullptr;
//
//	// 获取字体族名称
//	IDWriteLocalizedStrings* pFamilyNames = NULL;
//	hr = pFontFamily->GetFamilyNames(&pFamilyNames);
//	if (FAILED(hr))
//		return nullptr;
//
//	UINT32 index = 0;
//	BOOL exists = FALSE;
//	hr = pFamilyNames->FindLocaleName(L"zh-cn", &index, &exists);
//	if (!exists)
//	{
//		hr = pFamilyNames->FindLocaleName(L"en-us", &index, &exists);
//	}
//	if (!exists)
//	{
//		index = 0; // 使用第一个可用的名称
//	}
//
//	UINT32 length = 0;
//	hr = pFamilyNames->GetStringLength(index, &length);
//	if (FAILED(hr))
//		return nullptr;
//
//	std::wstring familyName(length + 1, L'\0');
//	hr = pFamilyNames->GetString(index, &familyName[0], length + 1);
//	if (FAILED(hr))
//		return nullptr;
//
//	// 创建 IDWriteTextFormat 对象
//	IDWriteTextFormat* pTextFormat = NULL;
//	hr = _DWriteFactory->CreateTextFormat(
//		familyName.c_str(),
//		pFontCollection,
//		DWRITE_FONT_WEIGHT_NORMAL,
//		DWRITE_FONT_STYLE_NORMAL,
//		DWRITE_FONT_STRETCH_NORMAL,
//		_fontsize,
//		L"", // 语言区域
//		&pTextFormat
//	);
//	if (FAILED(hr))
//		return nullptr;
//	Font* result = new Font(pTextFormat, _fontsize);
//	return result;
//}
GET_CPP(Font, IDWriteTextFormat*, FontObject)
{
	return this->_fontObject;
}
GET_CPP(Font, float, FontSize)
{
	return this->_fontSize;
}
SET_CPP(Font, float, FontSize)
{
	if (value != this->_fontSize && this->_fontObject)
	{
		this->_fontObject->Release();
		this->_fontObject = NULL;
		_DWriteFactory->CreateTextFormat(
			this->_fontName.c_str(),
			NULL,
			DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			this->FontSize,
			L"",
			&_fontObject);
	}
	this->_fontSize = value;
	this->FontHeight = this->GetTextSize(L'I').height;
}
GET_CPP(Font, std::wstring, FontName)
{
	return this->_fontName;
}
SET_CPP(Font, std::wstring, FontName)
{
	if (value != this->_fontName && this->_fontObject)
	{
		this->_fontObject->Release();
		this->_fontObject = NULL;
		_DWriteFactory->CreateTextFormat(
			this->_fontName.c_str(),
			NULL,
			DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			this->FontSize,
			L"",
			&_fontObject);
	}
	this->_fontName = value;
	this->FontHeight = this->GetTextSize(L'I').height;
}
D2D1_SIZE_F Font::GetTextSize(std::wstring str, float w, float h)
{
	D2D1_SIZE_F minSize = { 0,0 };
	IDWriteTextLayout* textLayout = 0;
	HRESULT hr = _DWriteFactory->CreateTextLayout(str.c_str(), str.size(), this->_fontObject, w, h, &textLayout);
	if SUCCEEDED(hr)
	{
		DWRITE_TEXT_METRICS metrics;
		hr = textLayout->GetMetrics(&metrics);
		textLayout->Release();
		if SUCCEEDED(hr)
		{
			minSize = D2D1::Size((float)ceil(metrics.widthIncludingTrailingWhitespace), (float)ceil(metrics.height));
			return minSize;
		}
	}
	return { 0,0 };
}
D2D1_SIZE_F Font::GetTextSize(IDWriteTextLayout* textLayout)
{
	D2D1_SIZE_F minSize = { 0,0 };
	if (textLayout)
	{
		DWRITE_TEXT_METRICS metrics;
		HRESULT hr = textLayout->GetMetrics(&metrics);
		if SUCCEEDED(hr)
		{
			minSize = D2D1::Size((float)ceil(metrics.widthIncludingTrailingWhitespace), (float)ceil(metrics.height));
			return minSize;
		}
	}
	return { 0,0 };
}
D2D1_SIZE_F Font::GetTextSize(wchar_t c)
{
	D2D1_SIZE_F minSize = { 0,0 };
	IDWriteTextLayout* textLayout = 0;
	HRESULT hr = _DWriteFactory->CreateTextLayout(&c, 1, this->_fontObject, FLT_MAX, FLT_MAX, &textLayout);
	if SUCCEEDED(hr)
	{
		DWRITE_TEXT_METRICS metrics;
		hr = textLayout->GetMetrics(&metrics);
		textLayout->Release();
		if SUCCEEDED(hr)
		{
			minSize = D2D1::Size((float)ceil(metrics.widthIncludingTrailingWhitespace), (float)ceil(metrics.height));
			return minSize;
		}
	}
	return { 0,0 };
}
int Font::HitTestTextPosition(std::wstring str, float x, float y)
{
	if (str.size() == 0) return -1;
	IDWriteTextLayout* textLayout = NULL;
	HRESULT hr = _DWriteFactory->CreateTextLayout(str.c_str(),str.size(),this->_fontObject,FLT_MAX,FLT_MAX,&textLayout);
	if FAILED(hr)
		return -1;
	BOOL isTrailingHit;
	BOOL isInside;
	DWRITE_HIT_TEST_METRICS caretMetrics;
	textLayout->HitTestPoint(x, y,&isTrailingHit,&isInside,&caretMetrics);
	textLayout->Release();
	return isTrailingHit ? caretMetrics.textPosition + 1 : caretMetrics.textPosition;
}
int Font::HitTestTextPosition(std::wstring str, float width, float height, float x, float y)
{
	if (str.size() == 0) return -1;
	IDWriteTextLayout* textLayout = NULL;
	HRESULT hr = _DWriteFactory->CreateTextLayout(str.c_str(), str.size(),this->_fontObject,width, height,&textLayout);
	if FAILED(hr)
		return -1;
	BOOL isTrailingHit;
	BOOL isInside;
	DWRITE_HIT_TEST_METRICS caretMetrics;
	textLayout->HitTestPoint(x, y,&isTrailingHit,&isInside,&caretMetrics);
	textLayout->Release();
	if (caretMetrics.width > 0.0f && x - caretMetrics.left >= caretMetrics.width * 0.5f)
		caretMetrics.textPosition += 1;
	return caretMetrics.textPosition;
}
int Font::HitTestTextPosition(IDWriteTextLayout* textLayout, float x, float y)
{
	if (!textLayout)
		return -1;
	BOOL isTrailingHit;
	BOOL isInside;
	DWRITE_HIT_TEST_METRICS caretMetrics;
	textLayout->HitTestPoint(x, y,&isTrailingHit,&isInside,&caretMetrics);
	return isTrailingHit ? caretMetrics.textPosition + 1 : caretMetrics.textPosition;
}
int Font::HitTestTextPosition(IDWriteTextLayout* textLayout, float width, float height, float x, float y)
{
	if (textLayout)
	{
		BOOL isTrailingHit;
		BOOL isInside;
		DWRITE_HIT_TEST_METRICS caretMetrics;
		textLayout->HitTestPoint(x, y,&isTrailingHit,&isInside,&caretMetrics);
		return caretMetrics.textPosition;
	}
	return -1;
}
std::vector<DWRITE_HIT_TEST_METRICS> Font::HitTestTextRange(std::wstring str, UINT32 start, UINT32 len)
{
	std::vector<DWRITE_HIT_TEST_METRICS> hitTestMetrics;
	IDWriteTextLayout* textLayout = NULL;
	HRESULT hr = _DWriteFactory->CreateTextLayout(str.c_str(),str.size(),this->_fontObject,FLT_MAX,FLT_MAX,&textLayout);
	if SUCCEEDED(hr)
	{
		UINT32 actualHitTestCount = 0;
		hr = textLayout->HitTestTextRange(start, len,0.0f, 0.0f,NULL, 0,&actualHitTestCount);
		hitTestMetrics.resize(actualHitTestCount);
		UINT32 textLen = len;
		hr = textLayout->HitTestTextRange(start, len,0.0f, 0.0f,hitTestMetrics.data(),hitTestMetrics.size(),&actualHitTestCount);
		textLayout->Release();
	}
	return hitTestMetrics;
}
std::vector<DWRITE_HIT_TEST_METRICS> Font::HitTestTextRange(IDWriteTextLayout* textLayout, UINT32 start, UINT32 len)
{
	std::vector<DWRITE_HIT_TEST_METRICS> hitTestMetrics;
	UINT32 actualHitTestCount = 0;
	if (textLayout)
	{
		HRESULT hr = textLayout->HitTestTextRange(start, len,0.0f, 0.0f,NULL, 0,&actualHitTestCount);
		hitTestMetrics.resize(actualHitTestCount);
		UINT32 textLen = len;
		hr = textLayout->HitTestTextRange(start, len,0.0f, 0.0f,hitTestMetrics.data(),hitTestMetrics.size(),&actualHitTestCount);
	}
	return hitTestMetrics;
}
std::vector<std::wstring> Font::GetSystemFonts()
{
	static std::vector<std::wstring> result = std::vector<std::wstring>();
	if (result.size() == 0)
	{
		IDWriteFontCollection* pFontCollection = NULL;
		HRESULT hr = _DWriteFactory->GetSystemFontCollection(&pFontCollection);
		UINT32 familyCount = 0;
		if SUCCEEDED(hr)
		{
			familyCount = pFontCollection->GetFontFamilyCount();
			for (UINT32 i = 0; i < familyCount; ++i)
			{
				IDWriteFontFamily* pFontFamily = NULL;
				if SUCCEEDED(hr)
				{
					hr = pFontCollection->GetFontFamily(i, &pFontFamily);
					IDWriteLocalizedStrings* pFamilyNames = NULL;
					if SUCCEEDED(hr)
					{
						hr = pFontFamily->GetFamilyNames(&pFamilyNames);
						UINT32 index = 0;
						BOOL exists = false;
						wchar_t localeName[LOCALE_NAME_MAX_LENGTH];
						if SUCCEEDED(hr)
						{
							if (GetUserDefaultLocaleName(localeName, LOCALE_NAME_MAX_LENGTH))
							{
								hr = pFamilyNames->FindLocaleName(localeName, &index, &exists);
							}
							if (SUCCEEDED(hr) && !exists)
							{
								hr = pFamilyNames->FindLocaleName(L"en-us", &index, &exists);
							}
						}
						if (!exists)
							index = 0;
						UINT32 length = 0;
						if SUCCEEDED(hr)
							hr = pFamilyNames->GetStringLength(index, &length);
						if SUCCEEDED(hr)
						{
							std::wstring name(length,L'\0');
							hr = pFamilyNames->GetString(index, &name[0], length + 1);
							result.push_back(name);
						}
					}
				}
			}
		}
	}
	return result;
}