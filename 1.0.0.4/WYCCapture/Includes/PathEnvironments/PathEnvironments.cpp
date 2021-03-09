#include "stdafx.h"
#include "PathEnvironments.h"

CPathEnvironments* CPathEnvironments::m_singletone = NULL;

CPathEnvironments::CPathEnvironments()
{
	SetExePath();
	SetResourcePath();
}

CPathEnvironments::CPathEnvironments(const CPathEnvironments& r)
{

}

CPathEnvironments::~CPathEnvironments()
{
}

void CPathEnvironments::SetExePath()
{
	TCHAR full_path[MAX_PATH] = { 0, };
	::GetModuleFileName(NULL, full_path, MAX_PATH);

	m_exe_path = full_path;

	m_exe_path = m_exe_path.Left(m_exe_path.ReverseFind(_T('\\')) + 1);
}

void CPathEnvironments::SetResourcePath()
{
	if (m_exe_path.IsEmpty())
		SetExePath();

	m_resource_path = m_exe_path;
	m_resource_path += _T("Resource");
}
