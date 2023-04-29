#include "hzpch.h"
#include "Hazel/Utils/PlatformUtils.h"
#include "Hazel/Core/Application.h"

#include <commdlg.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <ShlObj.h>

namespace Hazel {

	float Time::GetTime()
	{
		return glfwGetTime();
	}

	const static int s_MaxSize = 256;

	std::string FileDialogs::OpenFile(const char* filter)
	{
		OPENFILENAMEA ofn;
		CHAR szFile[s_MaxSize] = { 0 };
		CHAR currentDir[s_MaxSize] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow());
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		if (GetCurrentDirectoryA(s_MaxSize, currentDir))
			ofn.lpstrInitialDir = currentDir;
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		if (GetOpenFileNameA(&ofn) == TRUE)
			return ofn.lpstrFile;

		return std::string();

	}

	std::string FileDialogs::SaveFile(const char* filter)
	{
		OPENFILENAMEA ofn;
		CHAR szFile[s_MaxSize] = { 0 };
		CHAR currentDir[s_MaxSize] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow());
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		if (GetCurrentDirectoryA(s_MaxSize, currentDir))
			ofn.lpstrInitialDir = currentDir;
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

		// Sets the default extension by extracting it from the filter
		ofn.lpstrDefExt = strchr(filter, '\0') + 1;

		if (GetSaveFileNameA(&ofn) == TRUE)
			return ofn.lpstrFile;
		
		return std::string();
	}

	static char s_StartingDir[s_MaxSize] = { 0 };

	//Example from
	//https://cpp.hotexamples.com/site/file?hash=0x8f0e78c3fb330f16b19d4098250056de26362211ff72383138301a1aafa83d59&fullName=LumixEngine-master/src/editor/platform_interface.cpp&project=gunsafighter/LumixEngine
	std::string FileDialogs::OpenDir()
	{
		bool ref = false;
		IFileDialog* pfd;
		if (SUCCEEDED(CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd)))) {
			if (s_StartingDir[0])
			{
				PIDLIST_ABSOLUTE pidl;
				WCHAR wstarting_dir[s_MaxSize];
				WCHAR* wc = wstarting_dir;
				for (const char* c = s_StartingDir; *c && wc - wstarting_dir < s_MaxSize - 1; ++c, ++wc)
				{
					*wc = *c == '/' ? '\\' : *c;
				}
				*wc = 0;

				HRESULT hresult = ::SHParseDisplayName(wstarting_dir, 0, &pidl, SFGAO_FOLDER, 0);
				if (SUCCEEDED(hresult))
				{
					IShellItem* psi;
					hresult = ::SHCreateShellItem(NULL, NULL, pidl, &psi);
					if (SUCCEEDED(hresult))
					{
						pfd->SetFolder(psi);
					}
					ILFree(pidl);
				}
			}

			DWORD dwOptions;
			if (SUCCEEDED(pfd->GetOptions(&dwOptions)))
			{
				pfd->SetOptions(dwOptions | FOS_PICKFOLDERS);
			}
			if (SUCCEEDED(pfd->Show(NULL)))
			{
				IShellItem* psi;
				if (SUCCEEDED(pfd->GetResult(&psi)))
				{
					WCHAR* tmp;
					if (SUCCEEDED(psi->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &tmp)))
					{
						char* c = s_StartingDir;
						while (*tmp && c - s_StartingDir < s_MaxSize - 1)
						{
							*c = (char)*tmp;
							++c;
							++tmp;
						}
						*c = '\0';
						
					}
					psi->Release();
					ref = true;
					
				}
			}
			pfd->Release();
		}

		if (ref) {
			return s_StartingDir;
		}
		return std::string();
	}


}
