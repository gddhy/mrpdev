#ifndef _SCREENSHOT_H_
#define _SCREENSHOT_H_

//mrperʹ�õĽ�16λbmpͼ����

/*ע�� ֻ�ʺ���16bit��Ļ

����˵��

pBmp	��ͼ��ַ��ע��������char * ָ��

SaveDir	�����ļ���(��ͼ�������� SaveDir+4λ����+4λʱ��+2λ������.bmp)

x,y,w,h	�ǽ�ͼλ��

max_h	����Ļ���

isSPR	�����Ƿ���SPRչѶƽ̨,ȡֵ0/1 ��Ӧ MTK/SPR

����ֵ��
MR_SUCCESS	�����ɹ�
MR_FAILED	����ʧ�ܣ������ļ�ʧ�ܣ�

ʾ����MTK����ȫ��
picShot( (char *)w_getScreenBuffer,0,0,240,320,240,0);
*/
int32 picShot(char *pBmp,char *SaveDir,int16 x,int16 y,int16 w,int16 h,int16 max_w,int8 isSPR);

#endif