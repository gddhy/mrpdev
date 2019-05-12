#include "mrc_base.h"
#include "mrc_bmp.h"
#include "debug.h"



uint16 *OldScreen;//��ʵ��Ļ�����ַ
uint16 *NewScreen;//��̨��Ļ�����ַ
int32 Scr_w,Scr_h;//��Ļ��Ϣ

/********   �ṩһ������ʵ��Ļ�����ת�ĺ�̨���� ********/
/*           void RotateScreen(void)                    */
/*           ��� RotateRefresh  ʹ��                   */
/********   �ṩһ������ʵ��Ļ�����ת�ĺ�̨���� ********/
void RotateScreen(void)
{
    mrc_getScreenSize(&Scr_w,&Scr_h);//��ȡ��Ļ������Ϣ
    NewScreen=mrc_malloc(Scr_w*Scr_h*2);//����һ���̨����,������Ʋ���Ƶ��,����ֻ����1��
    mrc_memset(NewScreen,0,Scr_w*Scr_h*2);
    w_setScreenBuffer(NewScreen);//����̨������ʱ����Ϊ��Ļ�����ַ
    mrc_setScreenSize(Scr_h,Scr_w);//�˴�ʵ�ֿ�ߵ�ת
}
/********   ����̨����������ת90�ȿ�������ʵ��Ļ��ˢ��   ********/
/*           void RotateRefresh(void)                           */
/*           ��� RotateScreen  ʹ��                            */
/********   ����̨����������ת90�ȿ�������ʵ��Ļ��ˢ��   ********/
void RotateRefresh(void)
{
     w_setScreenBuffer(OldScreen);  //�˴���ԭ��ʵ��Ļ�����ַ
     mrc_setScreenSize(Scr_w,Scr_h);//�˴���ԭ��ʵ��Ļ���
     mrc_bitmapShowFlip(NewScreen,0,0,(int16)Scr_h,(int16)Scr_h,(int16)Scr_w,BM_COPY|TRANS_ROT90,0,0,0);//�˴�����̨������ת90�Ⱥ�,���Ƶ���ʼ��Ļ����
     mrc_refreshScreen(0,0,(int16)Scr_w,(int16)Scr_h);//�������ʽ���Ļ�������ݸ��µ���Ļ

     mrc_free(&NewScreen);//�ͷź�̨����,������Ʋ���Ƶ��,���鵽�����˳�ʱ���ͷ�
     NewScreen=NULL;
}

void HorizontalScreenTest()
{
    int32 f;

	OldScreen = w_getScreenBuffer();//��ȡ��ʵ��Ļ�����ַ
	
	RotateScreen();//���RotateRefreshʹ��
    
    mrc_bitmapNew(0, 41, 26);
    mrc_bitmapLoad(0, "plane.bmp", 0, 0, 41, 26,41);
    mrc_bitmapShow(0, 240,50,BM_COPY,0, 0, 41, 26);
    mrc_drawText("ԭʼͼ",0,0,255,0,0,0,1);
    mrc_drawLine(0,0,320,0,255,0,0);

    mrc_refreshScreen(0,0,240,320);
    f=mrc_open("data.bmp",12);
    mrc_write(f,NewScreen,Scr_w*Scr_h*2);
    mrc_close(f);
//     return;
	RotateRefresh();//���RotateScreenʹ��
}

int32 MRC_EXT_INIT(void)
{   
    HorizontalScreenTest();

    return MR_SUCCESS;
}

int32 MRC_EXT_EXIT(void)
{	
return MR_SUCCESS;
}

int32 mrc_appEvent(int32 code, int32 param0, int32 param1)
{
return MR_SUCCESS;
}

int32 mrc_appPause()
{
return MR_SUCCESS;	
}

int32 mrc_appResume()
{
return MR_SUCCESS;
}
