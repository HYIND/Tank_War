#include "header.h"
#include "FileIO.h"
#include "commdlg.h"

namespace FileIO
{

	bool LoadFile(const TCHAR* filename, char*& buf, int& len)
	{

		FILE* fp = nullptr;
		_wfopen_s(&fp, filename, TEXT("rb"));
		if (!fp)
			return false;

		fseek(fp, 0, SEEK_END);      //���ļ�ָ��ָ����ļ������
		len = ftell(fp);   //����ָ��λ�ã���ʱ��������ļ����ַ���
		buf = new char[len + 1];
		memset(buf, '\0', len + 1);
		fseek(fp, 0, SEEK_SET);                 //���½�ָ��ָ���ļ��ײ�
		int l = fread(buf, sizeof(char), len, fp);
		bool  result = (len == l);
		//bool result = (len == fread(buf, sizeof(char), len, fp)); //��ʼ��ȡ�����ļ�
		fclose(fp);
		return result;
	}

	bool SaveFile(const TCHAR* filename, const char* buf, int len)
	{
		FILE* fp = nullptr;
		_wfopen_s(&fp, filename, TEXT("wb"));
		if (!fp)
			return false;
		bool result = (len == fwrite(buf, sizeof(char), len, fp));
		fclose(fp);
		return result;
	}

	bool OpenOneFile(TCHAR*& filename)
	{
		OPENFILENAME ofn;
		TCHAR szOpenFileNames[80 * MAX_PATH];
		TCHAR szPath[MAX_PATH];
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.Flags = OFN_EXPLORER;
		ofn.lStructSize = sizeof(ofn);
		ofn.lpstrFile = szOpenFileNames;
		ofn.nMaxFile = sizeof(szOpenFileNames);
		ofn.lpstrFile[0] = '\0';
		ofn.lpstrFilter = TEXT("All Files(*.*)\0*.*\0Map Files(*.map)\0*.map\0\0");
		if (GetOpenFileName(&ofn))
		{

			lstrcpyn(szPath, szOpenFileNames, ofn.nFileOffset);
			szPath[ofn.nFileOffset] = '\0';
			lstrcat(szPath, TEXT("\\"));
			TCHAR* p = szOpenFileNames + ofn.nFileOffset; //��ָ���Ƶ���һ���ļ�

			filename = new TCHAR[80 * MAX_PATH];
			ZeroMemory(filename, sizeof(80 * MAX_PATH));
			lstrcat(filename, szPath);  //���ļ�������·��  
			lstrcat(filename, p);    //�����ļ��� 
			//MessageBox(NULL, filename, TEXT("OneFileOpen"), MB_OK);

			return true;
		}
		else return false;
	}

	bool SaveOneFile(TCHAR*& filename)
	{
		OPENFILENAME ofn;
		TCHAR szOpenFileNames[80 * MAX_PATH];
		TCHAR szPath[MAX_PATH];
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.Flags = OFN_EXPLORER;
		ofn.lStructSize = sizeof(ofn);
		ofn.lpstrFile = szOpenFileNames;
		ofn.nMaxFile = sizeof(szOpenFileNames);
		ofn.lpstrFile[0] = '\0';
		ofn.lpstrFilter = TEXT("Map Files(*.map)\0*.map\0All Files(*.*)\0*.*\0\0");
		ofn.lpstrDefExt = TEXT("map\0");
		if (GetSaveFileName(&ofn))
		{

			lstrcpyn(szPath, szOpenFileNames, ofn.nFileOffset);
			szPath[ofn.nFileOffset] = '\0';
			lstrcat(szPath, TEXT("\\"));
			TCHAR* p = szOpenFileNames + ofn.nFileOffset; //��ָ���Ƶ���һ���ļ�

			filename = new TCHAR[80 * MAX_PATH];
			ZeroMemory(filename, sizeof(80 * MAX_PATH));
			lstrcat(filename, szPath);  //���ļ�������·��  
			lstrcat(filename, p);    //�����ļ��� 
			//MessageBox(NULL, filename, TEXT("OneFileOpen"), MB_OK);

			return true;
		}
		else return false;
	}

	bool OpenMultiFile()
	{
		OPENFILENAME ofn;
		TCHAR szOpenFileNames[80 * MAX_PATH];
		TCHAR szPath[MAX_PATH];
		TCHAR szFileName[80 * MAX_PATH];
		TCHAR* p;
		int nLen = 0;
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.Flags = OFN_EXPLORER | OFN_ALLOWMULTISELECT;
		ofn.lStructSize = sizeof(ofn);
		ofn.lpstrFile = szOpenFileNames;
		ofn.nMaxFile = sizeof(szOpenFileNames);
		ofn.lpstrFile[0] = '/0';
		ofn.lpstrFilter = TEXT("All Files(*.*)\0*.*\0Map Files(*.map)\0*.map\0\0");
		if (GetOpenFileName(&ofn))
		{
			//�ѵ�һ���ļ���ǰ�ĸ��Ƶ�szPath,��:
			//���ֻѡ��һ���ļ�,�͸��Ƶ����һ��'/'
			//���ѡ�˶���ļ�,�͸��Ƶ���һ��NULL�ַ�
			lstrcpyn(szPath, szOpenFileNames, ofn.nFileOffset);
			//��ֻѡ��һ���ļ�ʱ,�������NULL�ַ��Ǳ����.
			//���ﲻ����Դ�ѡ��һ���Ͷ���ļ������
			szPath[ofn.nFileOffset] = '/0';
			nLen = lstrlen(szPath);

			if (szPath[nLen - 1] != '//')   //���ѡ�˶���ļ�,��������'//'
			{
				lstrcat(szPath, TEXT("//"));
			}

			p = szOpenFileNames + ofn.nFileOffset; //��ָ���Ƶ���һ���ļ�

			ZeroMemory(szFileName, sizeof(szFileName));
			while (*p)
			{
				lstrcat(szFileName, szPath);  //���ļ�������·��  
				lstrcat(szFileName, p);    //�����ļ���  
				lstrcat(szFileName, TEXT("/n")); //����   
				p += lstrlen(p) + 1;     //������һ���ļ�
			}
			return true;
			//MessageBox(NULL, szFileName, TEXT("MultiSelect"), MB_OK);
		}
		else return false;
	}

}