#include<iostream>
#include <windows.h>
#include"CKernel.h"


using namespace std;

int main()
{
	CKernel kernel;
	if (!kernel.startServer())
	{
		cout << "�򿪷�����ʧ��" << endl;
		return 1;
	}
	
	while (1)
	{
		Sleep(5000);
		cout << "server is running" << endl;
	}




	return 0;
}