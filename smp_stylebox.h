#ifndef _SMP_STYLEBOX_H
#define _SMP_STYLEBOX_H

#include "window.h"


#define  SMP_BOXI_HILICHANGED	0x0001	//�����ı�

#define  SMP_BOXI_CLICKED		0x0002	//�����

#define  SMP_BOXI_SELECTED		0x0003	//ѡ��

#define  SMP_BOXI_MOUSEMOVE		0x0004	//����ƶ�

/*��ø������*/
int SMP_Box_GetHilightId(HWND hWnd);

/*������Ϣ*/
VOID SMP_Stylewnd_SetItem(HWND hWnd, const DWORD* bmps, int size);


LRESULT SMP_Box_WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

#endif