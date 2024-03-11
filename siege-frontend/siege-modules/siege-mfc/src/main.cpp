#include "targetver.h"

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxcontrolbars.h>
#include <afxcmn.h>    

#include <map>
#include <string>
#include <array>
#include <string_view>
#include <cassert>



#pragma warning(disable: 4311)

template<typename TClass>
TClass* AllocateClass()
{
	return new TClass();
}

template<typename TClass>
LRESULT ProcessMessage(TClass* control, UINT message, WPARAM wparam, LPARAM lparam)
{
	return AfxCallWndProc(control, *control, message, wparam, lparam);
}

template<>
LRESULT ProcessMessage(CVSListBox* control, UINT message, WPARAM wparam, LPARAM lparam)
{
	if (message == LB_INSERTSTRING || message == LB_ADDSTRING)
	{
		wparam = message == LB_ADDSTRING ? -1 : wparam;

		return control->AddItem(CString(reinterpret_cast<wchar_t*>(lparam)), 0, int(wparam));
	}

	if (message == LB_SETITEMDATA)
	{
		//TODO call SetItemData
	}

	if (message == LB_GETITEMDATA)
	{
		//TODO call GetItemData
	}

	if (message == LB_GETCOUNT)
	{
		return LRESULT(control->GetCount());
	}

	if (message == LB_GETSEL)
	{
		// TODO call GetSelItem
	}

	if (message == LB_GETSELCOUNT)
	{
		return LB_ERR;
	}

	if (message == LB_SETCURSEL)
	{
		// TODO call SelectItem
	}

	if (message == LB_GETTEXT)
	{
		// TODO call GetItemText
	}

	if (message == LB_GETTEXTLEN)
	{
		// TODO call GetItemText
	}

	if (message == LB_DELETESTRING)
	{
		//TODO call RemoveItem
	}

	if (message == LVM_EDITLABELW)
	{
		if (control->EditItem(int(wparam)))
		{
			auto result = ::SendMessageW(::GetWindow(*control, GW_CHILD), LVM_GETEDITCONTROL, 0, 0);
			return result;		
		}

		return 0;
	}

	return AfxCallWndProc(control, *control, message, wparam, lparam);
}

template<>
LRESULT ProcessMessage(CMFCPropertyGridCtrl* control, UINT message, WPARAM wparam, LPARAM lparam)
{

	if (message == LVM_GETHEADER) 
	//LVM_GETITEM LVM_DELETEITEM LVM_INSERTITEM
	//LVM_INSERTGROUP LVM_GETGROUPINFO
	//LVM_GETCOLUMN LVM_GETHEADER LVM_ENABLEGROUPVIEW LVM_GETGROUPCOUNT
		//LVM_GETGROUPINFO
		//LVM_GETGROUPINFOBYINDEX
		//LVM_REMOVEGROUP
		//LVM_GETITEMCOUNT
	{
		//TODO call SetItemData
	}

	return ::SendMessageW(*control, message, wparam, lparam);
}

struct parent_wrapper
{
	static LRESULT OwnerDrawProc(HWND hWnd,
					  UINT uMsg,
					  WPARAM wParam,
					  LPARAM lParam,
					  UINT_PTR uIdSubclass,
					  DWORD_PTR dwRefData)
	{
				AFX_MANAGE_STATE(AfxGetStaticModuleState());
				if (uIdSubclass == WM_DRAWITEM)
				{
					if (uMsg == WM_MEASUREITEM || 
							uMsg == WM_DRAWITEM || 
							uMsg == WM_COMPAREITEM ||
							uMsg == WM_DELETEITEM)
					{
							HWND child = GetDlgItem(hWnd, int(wParam));

							if (child != nullptr && ::SendMessageW(child, uMsg, wParam, lParam))
							{
								return TRUE;
							}				
					}
					else if (uMsg == WM_NCDESTROY)
					{	
							RemoveWindowSubclass(hWnd, OwnerDrawProc, WM_DRAWITEM);
					}
				}

				return DefSubclassProc(hWnd, uMsg, wParam, lParam);
	}
		
	static LRESULT CwndProc(HWND hWnd,
					  UINT uMsg,
					  WPARAM wParam,
					  LPARAM lParam,
					  UINT_PTR uIdSubclass,
					  DWORD_PTR dwRefData)
	{
		if (DWORD_PTR(uIdSubclass) == dwRefData && uMsg == WM_NCDESTROY)
		{
			CWnd* self = (CWnd*)dwRefData;
			self->Detach();
			delete self;

			RemoveWindowSubclass(hWnd, CwndProc, uIdSubclass);
		}

		return DefSubclassProc(hWnd, uMsg, wParam, lParam);
	}
};


WNDCLASSEXW GetSystemClass(wchar_t* className)
{
	WNDCLASSEXW info{sizeof(WNDCLASSEXW)};

	assert(::GetClassInfoExW(nullptr, className, &info));

	return info;
}

WNDCLASSEXW GetSystemClass(const CRuntimeClass& info)
{
	static std::map<const CRuntimeClass*, WNDCLASSEXW> knownClasses = {
		{&CStatic::classCStatic, GetSystemClass(WC_STATICW)},
		{&CButton::classCButton, GetSystemClass(WC_BUTTONW)},
		{&CListBox::classCListBox, GetSystemClass(WC_LISTBOXW)},
		{&CComboBox::classCComboBox, GetSystemClass(WC_COMBOBOXW)},
		{&CEdit::classCEdit, GetSystemClass(WC_EDITW)},
		{&CScrollBar::classCScrollBar, GetSystemClass(WC_SCROLLBARW)},
		{&CListCtrl::classCListCtrl, GetSystemClass(WC_LISTVIEWW)},
		{&CTreeCtrl::classCTreeCtrl, GetSystemClass(WC_TREEVIEWW)},
		{&CTabCtrl::classCTabCtrl, GetSystemClass(WC_TABCONTROLW)},
		{&CHeaderCtrl::classCHeaderCtrl, GetSystemClass(WC_HEADERW)},
		{&CComboBoxEx::classCComboBoxEx, GetSystemClass(WC_COMBOBOXEXW)},
		{&CSpinButtonCtrl::classCSpinButtonCtrl, GetSystemClass(UPDOWN_CLASSW)},
		{&CReBar::classCReBar, GetSystemClass(REBARCLASSNAMEW)},
		{&CProgressCtrl::classCProgressCtrl, GetSystemClass(PROGRESS_CLASSW)},
		{&CToolTipCtrl::classCToolTipCtrl, GetSystemClass(TOOLTIPS_CLASSW)},
		{&CMonthCalCtrl::classCMonthCalCtrl, GetSystemClass(MONTHCAL_CLASSW)},
		{&CDateTimeCtrl::classCDateTimeCtrl, GetSystemClass(DATETIMEPICK_CLASSW)},
		{&CToolBarCtrl::classCToolBarCtrl, GetSystemClass(TOOLBARCLASSNAMEW)},
		{&CStatusBarCtrl::classCStatusBarCtrl, GetSystemClass(STATUSCLASSNAMEW)},
		{&CSliderCtrl::classCSliderCtrl, GetSystemClass(TRACKBAR_CLASS)},
	};

	for (auto& other : knownClasses)
	{
		if (info.IsDerivedFrom(other.first))
		{
			return other.second;
		}
	}

	WNDCLASSEXW fallback{sizeof(WNDCLASSEXW)};
	
	if (info.IsDerivedFrom(&CDialog::classCDialog))
	{
		fallback.lpfnWndProc = DefDlgProcW;
		fallback.cbWndExtra = DLGWINDOWEXTRA;
	}
	else
	{
		fallback.lpfnWndProc = DefWindowProcW;
		fallback.cbWndExtra = int(sizeof(void*));
	}

	return fallback;
}


template<typename TClass>
struct class_wrapper
{
	static LRESULT CwndProc(HWND hWnd,
					  UINT uMsg,
					  WPARAM wParam,
					  LPARAM lParam,
					  UINT_PTR uIdSubclass,
					  DWORD_PTR dwRefData)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		TClass* control = (TClass*)dwRefData;

		if (uMsg == WM_WINDOWPOSCHANGED)
		{
			auto* windowPos = (WINDOWPOS*)lParam;
			control->SetWindowPos(CWnd::FromHandle(windowPos->hwndInsertAfter), windowPos->x, windowPos->y, windowPos->cx, windowPos->cy, windowPos->flags);
		}

		if (uMsg == WM_NCDESTROY)
		{

			OutputDebugStringW(L"class_wrapper::WndProc WM_NCDESTROY\n");
			auto result = ProcessMessage(control, uMsg, wParam, lParam);
			RemoveWindowSubclass(*control, CwndProc, uIdSubclass);
			delete control;
			return result;
		}

		return ProcessMessage(control, uMsg, wParam, lParam);
	}

	static HWND CreateInstance(CREATESTRUCTW* params)
	{
		TClass* control = AllocateClass<TClass>();

		CWnd* parent = CWnd::FromHandlePermanent(params->hwndParent);

		if (parent == nullptr)
		{
			OutputDebugStringW(L"class_wrapper::WndProc Creating CWnd wrapper\n");

			parent = new CWnd();
			parent->Attach(params->hwndParent);

			SetWindowSubclass(params->hwndParent, parent_wrapper::CwndProc, UINT_PTR(parent), DWORD_PTR(parent));
			SetWindowSubclass(params->hwndParent, parent_wrapper::OwnerDrawProc, WM_DRAWITEM, 0);
			assert(CWnd::FromHandlePermanent(params->hwndParent));
		}

		if (CreateClass<TClass>(control, params))
		{
			SetWindowSubclass(*control, CwndProc, UINT_PTR(control), DWORD_PTR(control));
			return *control;
		}
		delete control;
		return nullptr;
	}

	inline static WNDCLASSEXW ParentClassInfo{};

	static LRESULT SuperProc(HWND hWnd,
					  UINT uMsg,
					  WPARAM wParam,
					  LPARAM lParam)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		auto proc = DefWindowProcW;

		if (ParentClassInfo.lpfnWndProc != nullptr)
		{
			proc = ParentClassInfo.lpfnWndProc;			
		}

		if (uMsg == WM_NCCREATE)
		{
			if (proc(hWnd, uMsg, wParam, lParam))
			{
				TClass* control = AllocateClass<TClass>();

				CREATESTRUCTW* params = reinterpret_cast<CREATESTRUCTW*>(lParam);
				CWnd* parent = CWnd::FromHandlePermanent(params->hwndParent);

				if (parent == nullptr)
				{
					OutputDebugStringW(L"class_wrapper::WndProc Creating CWnd wrapper\n");

					parent = new CWnd();
					parent->Attach(params->hwndParent);

					SetWindowSubclass(params->hwndParent, parent_wrapper::CwndProc, UINT_PTR(parent), DWORD_PTR(parent));
					SetWindowSubclass(params->hwndParent, parent_wrapper::OwnerDrawProc, WM_DRAWITEM, 0);
					assert(CWnd::FromHandlePermanent(params->hwndParent));
				}

				control->SubclassWindow(hWnd);
				//control->SetStandardButtons();
				SetWindowSubclass(*control, CwndProc, UINT_PTR(control), DWORD_PTR(control));
			
				return TRUE;
			}
		}

		return proc(hWnd, uMsg, wParam, lParam);
	}
};


template <typename TClass>
CRuntimeClass* GetRuntimeClass()
{
	if constexpr (std::is_same_v<CMFCRibbonBar, TClass>)
	{
		return RUNTIME_CLASS(CMFCRibbonBar);
	}
	else if constexpr (std::is_same_v<CMFCShellTreeCtrl, TClass>)
	{
		return RUNTIME_CLASS(CMFCShellTreeCtrl);
	}
	else if constexpr (std::is_same_v<CMFCShellListCtrl, TClass>)
	{
		return RUNTIME_CLASS(CMFCShellListCtrl);
	}
	else if constexpr (std::is_same_v<CMFCPropertyGridCtrl, TClass>)
	{
		return RUNTIME_CLASS(CMFCPropertyGridCtrl);
	}
	else if constexpr (std::is_same_v<CVSListBox, TClass>)
	{
		return RUNTIME_CLASS(CVSListBox);
	}
	else if constexpr (std::is_same_v<CMFCOutlookBarTabCtrl, TClass>)
	{
		return RUNTIME_CLASS(CMFCOutlookBarTabCtrl);
	}
	else
	{
		TClass instance;
		return instance.GetRuntimeClass();	
	}
}


template<typename TClass>
auto RegisterClass(std::string className = std::string{})
{		
		auto metaInfo = GetRuntimeClass<TClass>();

		if (className.empty())
		{
			className = std::string(metaInfo->m_lpszClassName);		
		}

		auto temp = L"MFC::" + std::wstring(className.begin(), className.end());
		WNDCLASSEXW classInfo = class_wrapper<TClass>::ParentClassInfo = GetSystemClass(*metaInfo);

		classInfo.hInstance = AfxGetInstanceHandle();
		
		OutputDebugStringW((L"Registering " + temp + L"\n").c_str());
		classInfo.lpszClassName = temp.c_str();
		classInfo.lpfnWndProc = class_wrapper<TClass>::SuperProc;
		
		auto result = ::RegisterClassEx(&classInfo);
		assert(result);

		return result;
}


struct CMFCLibrary : public CWinApp
{
	BOOL InitInstance() override
	{
		OutputDebugStringW(L"CMFCLibrary::InitInstance enter\n");
		
		bool result = true;

		// common controls
		RegisterClass<CButton>();
		RegisterClass<CSplitButton>();
		RegisterClass<CComboBox>();
		RegisterClass<CDateTimeCtrl>();
		RegisterClass<CEdit>();
		RegisterClass<CComboBoxEx>();
		RegisterClass<CListBox>();
		RegisterClass<CListCtrl>();
		RegisterClass<CStatic>();
		RegisterClass<CHeaderCtrl>();
		RegisterClass<CToolBarCtrl>();
		RegisterClass<CTreeCtrl>();
		RegisterClass<CTabCtrl>();
		RegisterClass<CStatusBarCtrl>();
		

		//extended controls
		//list boxes
		RegisterClass<CDragListBox>();
		RegisterClass<CVSListBox>();
		RegisterClass<CCheckListBox>();
	

		//buttons
		RegisterClass<CMFCMenuButton>();
		RegisterClass<CMFCButton>();
		RegisterClass<CMFCColorButton>();
		RegisterClass<CMFCColorPickerCtrl>("CMFCColorPickerCtrl");
		RegisterClass<CMFCLinkCtrl>();

		//edits
		RegisterClass<CMFCEditBrowseCtrl>();
		RegisterClass<CMFCMaskedEdit>();
		//// combo boxes
		RegisterClass<CMFCFontComboBox>("CMFCFontComboBox");

		//// lists
		RegisterClass<CMFCPropertyGridCtrl>();
		RegisterClass<CMFCListCtrl>();
		RegisterClass<CMFCShellListCtrl>();

		////trees
		RegisterClass<CMFCShellTreeCtrl>();

		//// spinners
		RegisterClass<CMFCSpinButtonCtrl>();
		//
		////headers
		RegisterClass<CMFCHeaderCtrl>();
		
		////rebars
		RegisterClass<CMFCReBar>();		

		////tabs
		RegisterClass<CMFCTabCtrl>();
		RegisterClass<CMFCOutlookBarTabCtrl>();

		//// TODO to fix
		RegisterClass<CMFCRibbonBar>();

		return CWinApp::InitInstance();
	}

	int ExitInstance() override
	{
		OutputDebugStringW(L"CMFCLibrary::ExitInstance unregistering class\n");
		
		// TODO remove all the registered classes

		return CWinApp::ExitInstance();
	}
} library{};