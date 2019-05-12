
typedef struct _mrc_extChunk_st mrc_extChunk_st;

typedef struct  _mr_c_function_st       // ����˵��RW������
{
   uint8* start_of_ER_RW;	// RW��ָ��
   uint32 ER_RW_Length;		// RW����
   int32 ext_type;			// ext�������ͣ�Ϊ1ʱ��ʾext����
   mrc_extChunk_st * mrc_extChunk;	// extģ�������Σ�����Ľṹ�塣
   int32 stack;   //stack shell 2008-2-28
} mr_c_function_st;

typedef struct  _mrc_extChunk_st		// ����˵��extģ��������
{
   int32 check;		//0x7FD854EB ��־
   MR_LOAD_C_FUNCTION init_func; // mr_c_function_load��ڵ�
   MR_C_FUNCTION event;		// �¼���ں���������δ֪
   uint8* code_buf;		// extģ���ַ
   int32   code_len;	// extģ�鳤��
   uint8* var_buf;		// ȫ�ֱ���RW�ε�ַ
   int32   var_len;		// RW����
   mr_c_function_st* global_p_buf;		// RW�����Σ�����Ľṹ�塣
   int32   global_p_len;		// ����
   int32   timer;		// ��ʱ�������mrc_extLoad����mrc_timerCreate����
   mrc_extMainSendAppMsg_t sendAppEvent;	// ������ext��Ϣ���պ���ָ��
   mr_table *extMrTable;		// mr_table������
#ifdef MRC_PLUGIN	// ����ļ���û��ʹ�ã���Ϊ�����0x30��С����Ϊֹ��
   MR_C_FUNCTION_EX eventEx;
#endif
   int32 isPause;/*1: pause ״̬0:����״̬*/
#ifdef SDK_MOD
   mrc_init_t init_f;
   mrc_event_t event_f;
   mrc_pause_t pause_f;
   mrc_resume_t resume_f;
   mrc_exitApp_t exitApp_f;
#endif
} mrc_extChunk_st;

// �����һ��ȫ�ֱ����������Ķ�����
uint32* pMrTable = *(uint32**)(mr_c_function_load - 8);

/************************************
		mrc_extLoad
		չʾ��ext���ص�ȫ����
************************************/
int32 mrc_extLoad (int32* extAddr, int32 len)
{
	mr_table* ext_table;	// ��ext��MrTable
	mr_extChunk_st* ext_handle; // ��extģ���������
	mr_c_function_st* pSt;	// RW������

	
	if(mrc_extInitMainModule() != MR_SUCCESS )
		return -1;//ʧ��
		
	// Ϊ�µ�ext�����������ڴ�
	ext_table = mrc_malloc(0x248); //mr_table��
	if(ext_table == NULL)
		return -1;		
	mrc_memcpy(ext_table, pTable, 0x248);//����mr_table

	ext_table[25] = mrc_extFunction_new; //�ñ��غ������滻
	
	// ����ext�������ڴ�
	ext_handle = mrc_malloc(0x30); // ���䡰ext������ڴ�
	if(ext_handle == NULL)
	{
		mrc_free(ext_table);
		return -1;
	}

	mrc_memset(ext_handle, 0, 30);
	
	// ����ext������
	ext_handle->extMrTable = ext_table; //���ñ�
	ext_handle->check = 0x7FD854EB; // EXT��־
	ext_handle->timer = mrc_timerCreate();
	ext_handle->sendAppEvent = mrc_extMainSendAppEventShell;
	//����RW�ṹ��
	ext_handle->global_p_buf = pSt = mrc_malloc(20);
	
	if(pSt == NULL)
	{
		mrc_free(ext_table);
		mrc_free(ext_handle);
		return -1;//ʧ��
	}
	mrc_memset(pSt, 0, 20);
	//
	ext_handle->global_p_len = 20; //�ṹ���С
	ext_handle->init_func = extAddr + 8;//��ڣ�mr_c_function_load
	ext_handle->code_buf = extAddr;// �׵�ַ
	ext_handle->code_len = len; //ext��С
	
	//����extͷ
	extAddr[0] = ext_table; //���ú�����
	extAddr[1] = pSt; // ����RW�ṹ��
	pSt->mrc_extChunk = ext_handle; //���þ��
	
	//������� mrc_event(5001, )
	_mr_testCom1(0, 9, extAddr, len); //������ֲ�㺯��������δ֪
	
	mr_c_function_load(1); //����Ŀ��ext��������
	
	// ����mr_c_function_load��RW�Ѿ������ˡ�
	// �ٶ�<RW>��RW�λ�ַ��
	ext_handle->event = *(<RW> + 0x14);
	ext_handle->var_buf = pSt->pRW;
	ext_handle->var_len = pSt->len;
	
	(ext_handle->event)(pSt, 6, 0, *(<RW>+0xC), 0, 0);//δ֪
	(ext_handle->event)(pSt, 8, *(<RW>+0x10), 0, 0, 0);
	
	
	return ext_handle; // ���ؾ��
}

/******************************
		�� EXT�ڲ����� ��
		mr_c_function_load
		���𲿷ֳ�ʼ��������
*******************************/
int32 mr_c_function_load (int32 code)
{
	mr_c_function_st* pSt = *((uint32)mr_c_function_load - 0x4); // ȡRW�������ṹ��ָ��
	void* p; int32 len;

	typedef int (*PFUNC_2) (void*,int);
	if( code == 1 ) // ext����
	{
		int32 a = _mr_c_function_new(mrc_extHelper, 20); // ������ֲ�㡣
		if( a == -2 ) return -1;
		pSt->ext_type = 1; //��������Ϊext����
		pMrTable->g_mr_timerStart = mrc_extTimerStart;	// mr_table -> g_mr_timerStart
		pMrTable->g_mr_timerStop = mrc_extTimerStop;	// mr_table -> g_mr_timerStop
	}
	else			// ��ͨ����
	{
		int32 a = _mr_c_function_new(mr_helper, 20); // ��ֲ�㺯��
		if( a == -2 ) return -1; //����ʧ��
		pSt->ext_type = 0; 
	}
	// �����������
	len = mr_helper_get_rw_len(); //ȡ��RW���ȣ�ext�ڲ��ĺ�����ÿ��ext��ͬ
	pSt->ER_RW_Length = len;

	p = mrc_malloc( len );
	pST->Start_of_ER_RW = p;
	if( p == NULL ) return -1;
	mrc_memset( p, 0, len); // ������ƫ��0x38����memset��

	return 0; //�ɹ�
}

/**************************************
		��EXT�ڲ�������
		mr_helper
		������Ϣ�ַ���
***************************************/
/**
	 * (void* P, int32 code, uint8* input, int32 input_len, uint8** output, int32* output_len);
	 * ������⣺
	 * input��Ϊһint32���飬���ڴ��ݲ���
	 * input_len��input�ֽ���
	 * p��global_p_buf ָ��
	 * 	code �������������£�
	 *		0��mrc_init input null
	 *		1��mrc_event input����ǰ3��Ϊmrc_event���� input_len=20
	 *					���� input[0] == 8 ʱΪext�˳���Ϣ��������mrc_exitApp
	 *		4��mrc_pause input=null input_len=0
	 *		5��mrc_resume input=null input_len=0	 
	 */
int32 mr_helper (void** pRW, int32 msg, int32 c[], int32 d)
{
	uint32* RWAddr = *(uint32*)pRW + 4; // ȡRW��
	int32 ret = 0;  //����ֵ

	if(msg >= 9) return 0;
	switch(msg) //��Ϣ����
	{
	case 0: // mrc_init
		_mr_init_c_helper_function(); // ��ʼ��RW��
		ret = mrc_init();
		mrc_refreshScreenReal();
		pMrTable->mr_internal_table->mr_timer_p = "dealtimer";
		break;
	case 1: // mrc_event
		ret = mrc_event(c[0], c[1], c[2]);
		if(c[0] == 0x8) // 0x8 ��ƽ̨�˳���Ϣ
			ret = mrc_exitApp();
		mrc_refreshScreenReal();
		break;
	case 2: 
		mrc_timerTimeout(); //δ֪
		mrc_refreshScreenReal();
		break;
	case 4:
		mrc_pause();
		mrc_refreshScreenReal();
		break;
	case 5:
		mrc_resume();
		mrc_refreshScreenReal();
		break;
	case 6:
		*(RWAddr + 0xC) = d; // λ��RW
		break;
	case 8:
		*(RWAddr + 0x10) = c; // λ��RW
		break;
	case 9:
		typedef int (*PFUNC_6) (int,int,int,int,int,int);
		{
			PFUNC_6* pfunc = (PFUNC) c[0];
			if(pfunc) 
				ret = (*pfunc)(c[1],c[2],c[3],c[4],c[5],c[6]);
		}
		break;
	default:
		return 0;
	}
	return ret;
}

int __cdecl _mr_intra_start(char *Str1, char *Source)
{
  char v3; // [sp+Ch] [bp-4Ch]@1
  int v4; // [sp+4Ch] [bp-Ch]@2
  int v5; // [sp+50h] [bp-8h]@15
  int i; // [sp+54h] [bp-4h]@10

  memset(&v3, -858993460, 0x4Cu);
  getAppInfo();
  Origin_LG_mem_len = _mr_getMetaMemLimit();
  if ( !_mr_mem_init_ex(*((_DWORD *)&mrc_appInfo_st + 3)) )
  {
    mr_event_function = 0;
    mr_timer_function = 0;
    mr_stop_function = 0;
    mr_pauseApp_function = 0;
    mr_resumeApp_function = 0;
    mr_ram_file = 0;
    mr_c_function_P = 0;
    mr_c_function_P_len = 0;
    mr_c_function_fix_p = 0;
    mr_exception_str = 0;
    mr_printf("Total memory:%d", LG_mem_len);
    v4 = 0;
    mr_screenBuf = 0;
    if ( !mr_platEx(1001, 0, 0, &mr_screenBuf, &v4, 0) )
    {
      if ( mr_screenBuf && v4 >= 2 * mr_screen_h * mr_screen_w )
      {
        dword_18D0 = 1;
        dword_18CC = v4;
      }
      else
      {
        if ( mr_screenBuf )
        {
          mr_platEx(1002, mr_screenBuf, v4, 0, 0, 0);
          mr_screenBuf = 0;
        }
      }
    }
    if ( !mr_screenBuf )
    {
      mr_screenBuf = mr_malloc(2 * mr_screen_h * mr_screen_w);
      dword_18D0 = 0;
      dword_18CC = 2 * mr_screen_h * mr_screen_w;
    }
	
    dword_18D4 = mr_screenBuf;
    word_18CA = mr_screen_h;
    word_18C8 = mr_screen_w;
    vm_state = 0;
    mr_timer_state = 0;
    mr_timer_run_without_pause = 0;
    bi &= 2u;
    memset(&mr_bitmap, 0, 0x1E0u);
    memset(&mr_sound, 0, 0x3Cu);
    memset(&mr_sprite, 0, 0x14u);
    memset(&mr_tile, 0, 0x3Cu);
    memset(&mr_map, 0, 0xCu);
	
    for ( i = 0; i < 3; ++i )
    {
      word_1610[10 * i] = 0;
      word_1612[10 * i] = 0;
      word_1614[10 * i] = mr_screen_w;
      word_1616[10 * i] = mr_screen_h;
    }
	
    if ( !Source )
      Source = "_dsm";
	  
    mr_strncpy(mr_entry, Source, 0x7Fu);
    mr_printf("Used by VM(include screen buffer):%d bytes", LG_mem_len - LG_mem_left);
    mr_state = 1;
    v5 = mr_doExt(Str1);
	
    if ( v5 )
    {
      mr_state = 5;
      mr_stop();
      mr_printf("init failed");
      mr_connectWAP("http://wap.skmeg.com/dsmWap/error.jsp");
    }
  }
  return _chkesp();
}

int __cdecl mr_timer()
{
  char v1; // [sp+Ch] [bp-44h]@1
  int v2; // [sp+4Ch] [bp-4h]@11

  memset(&v1, -858993460, 0x44u);
  if ( mr_timer_state == 1 )
  {
    mr_timer_state = 0;
    if ( mr_state == 1 || mr_timer_run_without_pause && mr_state == 2 )
    {
      if ( !mr_timer_function || (mr_timer_function(), v2 = _chkesp(), v2 == 1) )
        _mr_TestComC(801, 0, 1, 2);
    }
    else
    {
      if ( mr_state == 3 )
      {
        mr_stop();
        _mr_intra_start(start_filename, &_pad__1__mr_md5_finish__9_9);
      }
    }
  }
  else
  {
    mr_printf("warning:mr_timer event unexpected!");
  }
  return _chkesp();
}

int __cdecl mr_start_dsm(char *Source)
{
  char v2; // [sp+Ch] [bp-4Ch]@1
  int v3; // [sp+4Ch] [bp-Ch]@1
  int v4; // [sp+50h] [bp-8h]@2
  int v5; // [sp+54h] [bp-4h]@2

  memset(&v2, -858993460, 0x4Cu);
  if ( !mr_getScreenInfo(&v3) )
  {
    mr_screen_w = v3;
    mr_screen_h = v4;
    mr_screen_bit = v5;
	
    memset(&pack_filename, 0, 0x80u);
	
    if ( Source && *Source == 42 )
    {
      strcpy(&pack_filename, Source);
    }
    else
    {
      if ( Source && *Source == 37 )
      {
        strcpy(&pack_filename, Source + 1);
      }
      else
      {
        if ( Source && *Source == 35 && Source[1] == 60 )
          strcpy(&pack_filename, Source + 2);
        else
          strcpy(&pack_filename, "*A");
      }
    }
	
    mr_printf(&pack_filename);
    memset(old_pack_filename, 0, 0x80u);
    memset(old_start_filename, 0, 0x80u);
    memset(start_fileparameter, 0, 0x80u);
    *((_DWORD *)&mrc_appInfo_st + 3) = 0;
    _mr_intra_start("cfunction.ext", Source);
	
  }
  return _chkesp();
}