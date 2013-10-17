#include "NewMirror.h"
#include "atlbase.h"
#include "..\OsnVolumeCopyApi\OsnVolumeCopyApi.h"

DWORD CNewMirror::CheckVolIsBootableOrSys(wstring *label)
{
	/*try
	{*/
		DWORD dw = OSNInitWMI();
		if(dw == EXIT_FAILURE)
		{
			printf("Init WMI error!\n");
			return 3;
		}

		HRESULT hres;
		IEnumWbemClassObject* pEnumerator = NULL;
		wchar_t  pWQL[32];
		swprintf_s(pWQL,_countof(pWQL),L"Associators   of   {win32_LogicalDisk='%s'}   where   resultClass   =   Win32_DiskPartition",label->c_str());

		hres = m_pSvc->ExecQuery(
			bstr_t("WQL"),
			pWQL,
			WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
			NULL,
			&pEnumerator
			);
		if (FAILED(hres))
		{
			printf("pSvc->ExecQuery error\n");
			return 3;               // Program has failed.
		}
		IWbemClassObject *pclsObj;
		ULONG uReturn = 0;

		/*wstring *sysvol=(wstring *)System::Environment::SystemDirectory;
		sysvol = sysvol->Substring(0,2);
		bool issysvol = false;
		if(sysvol->compare(label) == 0)
		{
			issysvol = true;
			return  issysvol;
		}*/

		while(pEnumerator)
		{
			// 推出下一个对象
			pEnumerator->Next(WBEM_INFINITE, 1,&pclsObj, &uReturn);
			if(0 == uReturn)
			{
				break;
			}

			VARIANT vtProp;
			pclsObj->Get(L"Bootable", 0, &vtProp, 0, 0);
			wstring *VolumeLabel	= new wstring(vtProp.bstrVal);	//C:, D:, E:, etc.
			DWORD dw = _wtoi(VolumeLabel->c_str());
			delete(VolumeLabel);
			return dw;
		}
		return 3;
	//}
	//catch(...)
	//{
	//	// myEventLog->OSNWriteEventLog(String::Concat("CheckVolIsBootable: ",ex->ToString()),EventLogEntryType::Error,011);
	//}
	//return false;
}

DWORD CNewMirror::CheckFileSystem(wstring *LabelName)
{

	if(LabelName == NULL)
		return 0;

	char VolumeNameBuffer[32];
	DWORD lpVolumeSerialNumber;
	DWORD lpMaximumComponentLength;
	DWORD lpFileSystemFlags;
	char  lpFileSystemNameBuffer[32];
	wstring temp=L"\\";
	char  LabelNameC[10];

	temp +=  *LabelName;
	WcharToChar((wchar_t*)temp.c_str(),LabelNameC,sizeof(LabelNameC));
	BOOL ReturnValue = GetVolumeInformation(LabelNameC,
		VolumeNameBuffer,
		32,
		&lpVolumeSerialNumber,
		&lpMaximumComponentLength,
		&lpFileSystemFlags,
		lpFileSystemNameBuffer,
		32);
	if(ReturnValue == 0)
		return 0;

	if(strcmp("NTFS",lpFileSystemNameBuffer)!=0)
		return 1;
	else
		return 2;

}

DWORD CNewMirror::CharToWchar(const char *pChar,wchar_t *pWchar,int Length)
{
	if(!pChar || !pWchar)
	{
		return EXIT_FAILURE;
	}
	int   nLen  = strlen(pChar) + 1;      
	int   nwLen = MultiByteToWideChar(CP_ACP,0,pChar,nLen,NULL,0);  

	if(Length < nwLen)
		return EXIT_FAILURE;

	if(0 == MultiByteToWideChar(CP_ACP,0,pChar,nLen,pWchar,nwLen))
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}

DWORD CNewMirror::WcharToChar(const wchar_t *pWchar,char *pChar,int Length)
{
	if(!pChar || !pWchar)
	{
		return EXIT_FAILURE;
	}       

	int   nLen = wcslen(pWchar)+1;        

	if(Length < nLen)
		return EXIT_FAILURE;

	if(0 == WideCharToMultiByte(CP_ACP,0,pWchar,nLen,pChar,2*nLen,NULL,NULL)) 
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}

DWORD CNewMirror::OSNInitWMI()
{
	HRESULT hres;
	
	m_pLoc = 0;
	hres = CoCreateInstance(
		CLSID_WbemLocator,            
		0,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator, (LPVOID *) &m_pLoc);

	if (FAILED(hres))
	{
		printf("CoCreateInstance error\n");
		//CoUninitialize();
		return EXIT_FAILURE;       // Program has failed.
	}

	//使用pLoc连接到” root\cimv2” 并把pSvc的指针也搞定了
	hres = m_pLoc->ConnectServer(
		_bstr_t(L"ROOT\\CIMV2"), // WMI namespace
		NULL,                    // User name
		NULL,                    // User password
		0,                       // Locale
		NULL,                    // Security flags                
		0,                       // Authority      
		0,                       // Context object
		&m_pSvc                    // IWbemServices proxy
		);                             
	if (FAILED(hres))
	{
		printf("ConnectServer error\n");
		//m_pLoc->Release();    
		//CoUninitialize();
		return EXIT_FAILURE;                // Program has failed.
	}
	//已经连接到WMI了

	hres = CoSetProxyBlanket(
		m_pSvc,                         // the proxy to set
		RPC_C_AUTHN_WINNT,            // authentication service
		RPC_C_AUTHZ_NONE,             // authorization service
		NULL,                         // Server principal name
		RPC_C_AUTHN_LEVEL_CALL,       // authentication level
		RPC_C_IMP_LEVEL_IMPERSONATE,  // impersonation level
		NULL,                         // client identity
		EOAC_NONE                     // proxy capabilities
		);

	if (FAILED(hres))
	{
		printf("CoSetProxyBlanket error\n");
		//m_pSvc->Release();
		//m_pLoc->Release();    
		//CoUninitialize();
		return EXIT_FAILURE;               // Program has failed.
	}

	return EXIT_SUCCESS;
}

DWORD CNewMirror::CheckDiskIsEIMDisk(int index)
{
	/*try
	{*/
		DWORD dw = OSNInitWMI();
		if(dw == EXIT_FAILURE)
		{
			printf("Init WMI error!\n");
			return -1;
		}

		HRESULT hres;
		IEnumWbemClassObject* pEnumerator = NULL;
		wchar_t  pWQL[32];
		swprintf_s(pWQL,_countof(pWQL),L"Select * from Win32_DiskDrive where Index=%s",index);

		hres = m_pSvc->ExecQuery(
			bstr_t("WQL"),
			pWQL,
			WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
			NULL,
			&pEnumerator
			);
		if (FAILED(hres))
		{
			printf("pSvc->ExecQuery error\n");
			return -1;               // Program has failed.
		}
		IWbemClassObject *pclsObj;
		ULONG uReturn = 0;

		while(pEnumerator)
		{
			// 推出下一个对象
			pEnumerator->Next(WBEM_INFINITE, 1,&pclsObj, &uReturn);
			if(0 == uReturn)
			{
				break;
			}

			VARIANT vtProp;
			pclsObj->Get(L"PNPDeviceID", 0, &vtProp, 0, 0);
			wstring *VolumeLabel	= new wstring(vtProp.bstrVal);	//C:, D:, E:, etc.

			if(VolumeLabel->compare(L"EIM") == 0)
			{
				delete(VolumeLabel);
				return 1;
			}
			else
			{
				delete(VolumeLabel);
				return 2;
			}
		}
		return -1;
	/*}
	catch(...)
	{
		MessageBox::Show("获取磁盘的提供商信息出错，请稍后重试");
	}*/
}

DWORD CNewMirror::InitalizeDisk(int index)
{
	/*Process^ proc=gcnew Process();
	try
	{
		proc->StartInfo->FileName="diskpart.exe";
		proc->StartInfo->RedirectStandardError=true;
		proc->StartInfo->RedirectStandardInput=true;
		proc->StartInfo->RedirectStandardOutput=true;
		proc->StartInfo->CreateNoWindow=true;
		proc->StartInfo->UseShellExecute=false;
		proc->Start();
		proc->StandardInput->WriteLine(String::Concat("select disk ",Convert::ToString(index)));
		proc->StandardInput->Flush();
		proc->StandardInput->WriteLine("clean");
		proc->StandardInput->Flush();
		proc->StandardInput->WriteLine("create partition primary");
		proc->StandardInput->Flush();
		proc->StandardInput->WriteLine("delete partition");
		proc->StandardInput->Flush();
		proc->StandardInput->WriteLine("exit");
		proc->StandardInput->Flush();
		proc->WaitForExit(2000);
		System::Threading::Thread::Sleep(5*1000);
		return 0;
	}
	catch(...)
	{
		MessageBox::Show(String::Concat("初始化磁盘出现异常,请手动初始化磁盘",Convert::ToString(index)));
		return 1;
	}*/
	int szf = 1;
	return szf;
}

///1 eim disk 2,don't eim disk ,o get failed
DWORD CNewMirror::CheckVolIsEIMVol(wstring *LabelName)
{
	/*try
	{*/
		DWORD dw = OSNInitWMI();
		if(dw == EXIT_FAILURE)
		{
			printf("Init WMI error!\n");
			return -1;
		}

		HRESULT hres;
		IEnumWbemClassObject* pEnumerator = NULL;
		wchar_t  pWQL[32];
		swprintf_s(pWQL,_countof(pWQL),L"Associators of {win32_LogicalDisk='%s'} where resultClass = Win32_DiskPartition",LabelName->c_str());

		hres = m_pSvc->ExecQuery(
			bstr_t("WQL"),
			pWQL,
			WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
			NULL,
			&pEnumerator
			);
		if (FAILED(hres))
		{
			printf("pSvc->ExecQuery error\n");
			return -1;               // Program has failed.
		}
		IWbemClassObject *pclsObj;
		ULONG uReturn = 0;

		while(pEnumerator)
		{
			// 推出下一个对象
			pEnumerator->Next(WBEM_INFINITE, 1,&pclsObj, &uReturn);
			if(0 == uReturn)
			{
				break;
			}

			VARIANT vtProp;
			pclsObj->Get(L"DiskIndex", 0, &vtProp, 0, 0);
			wstring *VolumeLabel	= new wstring(vtProp.bstrVal);	//C:, D:, E:, etc.           
			int Index2 = _wtoi(VolumeLabel->c_str());
			delete(VolumeLabel);

			return CheckDiskIsEIMDisk(Index2);
		}
	/*}
	catch(...)
	{
		MessageBox::Show("获取卷的提供商信息失败，请稍后重试");

	}*/
	return -1;
}
void CNewMirror::VolumeMirrorClick(wstring *pSrcGuid,wstring *pDesGuid)
{
	bool isEIMMirror = false;

	//ListViewItem ^SelectItem = this->listView1->SelectedItems[0];

	this->pTargetVolume = this->pSystemVolumeList->GetVolumeInfoByString(pDesGuid);

	if(pTargetVolume == NULL)
	{
		printf("无法获取目标驱动器");
		return;
	}

	CRegKey *pRegKey = new CRegKey();
	DWORD Value;
	if(pRegKey->Open(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Services\\OSNVSS\\Parameters")!= ERROR_SUCCESS)
	{
		isEIMMirror = false;
	}
	else 
	{
		pRegKey->QueryDWORDValue("EIMMirror",Value);
		if(Value == 0)
		{
			isEIMMirror = false;
			pRegKey->SetDWORDValue("EIMMirror",2);
		}
		else 
		{
			if(Value == 1)
			{
				isEIMMirror = true;
			}
		}
	}

	int ret=CheckVolIsEIMVol(this->pTargetVolume->m_VolumeLable);
	int ret1=CheckVolIsEIMVol(this->pSourceVolume->m_VolumeLable);

	if(!isEIMMirror&&2==ret&&2==ret1)
	{
		printf("源卷和目标卷都不是EIM卷，不能建镜像关系！");
		delete(pRegKey);
		return ;
	}

	//if(1==this->m_EIMMode)//source
	//{
	//	int ret=CheckVolIsEIMVol(this->pSourceVolume->m_VolumeLable);
	//	if(-1==ret)
	//	{
	//		return;
	//	}
	//	else if(2==ret)
	//	{
	//		printf("源卷不是EIM卷");
	//		/*if(MessageBox::Show("源卷不是EIM卷，最好不要选source模式，是否继续","",MessageBoxButtons::YesNo)!=System::Windows::Forms::DialogResult::Yes)
	//		{
	//			return;
	//		}*/
	//	}
	//}
	if(2==this->m_EIMMode)//target
	{
		int ret=CheckVolIsEIMVol(this->pTargetVolume->m_VolumeLable);
		if(-1==ret)
		{
			delete(pRegKey);
			return;
		}
		else if(2==ret)
		{
			printf("目标卷不是EIM卷");
		}
	}
	//else if(4==this->m_EIMMode)//both
	//{
	//	int ret=CheckVolIsEIMVol(this->pTargetVolume->m_VolumeLable);
	//	int ret1=CheckVolIsEIMVol(this->pSourceVolume->m_VolumeLable);
	//	if(-1==ret||-1==ret)
	//	{
	//		return;
	//	}
	//	else if(2==ret||2==ret1)
	//	{
	//		if(MessageBox::Show("目标卷或者是源卷不是EIM卷，最好不要选both模式，是否继续？","",MessageBoxButtons::YesNo)!=System::Windows::Forms::DialogResult::Yes)
	//		{
	//			return;
	//		}
	//	}
	//}
	int tgtret= OsnCheckGptVolume((const char)*this->pTargetVolume->m_VolumeLable->c_str());
	int srcret= OsnCheckGptVolume((const char)*this->pSourceVolume->m_VolumeLable->c_str());

	if(tgtret>1||srcret>1)
	{
		printf("源卷和目标卷中有一个卷处于被保护状态或者其他未知状态，请检查是否处于镜像关系中，或者是已经删除镜像关系但要重启后生效");
		delete(pRegKey);
		return;
	}

	if(this->pSourceVolume->m_VolumeSize>pTargetVolume->m_VolumeSize)
	{
		printf("镜像卷比源卷小");
	}
	int isbootvol=CheckVolIsBootableOrSys(this->pTargetVolume->m_VolumeLable);
	if(isbootvol)
	{
		printf("镜像卷是系统卷或者是启动卷，不能设置为镜像卷，请选择其他卷");
		delete(pRegKey);
		return;
	}

	// check the file system
	int ErrorCode = CheckFileSystem(this->pTargetVolume->m_VolumeLable);
	if(ErrorCode!=0)
	{
		printf("镜像卷已经有文件系统存在");
	}

	//this->DialogResult =System::Windows::Forms::DialogResult::Yes;
	delete(pRegKey);
	return;
}

void CNewMirror::DiskMirrorClick(wstring *pSrcGuid,wstring *pDesGuid)
{
	bool isEIMMirror = false;

	this->pTargetDisk = this->pSystemDiskList ->GetDiskInfoByString(pDesGuid);
	if(pTargetDisk == NULL)
	{
		printf("无法获取目标驱动器");
		return;
	}

	CRegKey *pRegKey = new CRegKey();
	DWORD Value;
	if(pRegKey->Open(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Services\\OSNDSS\\Parameters")!= ERROR_SUCCESS)
	{
		isEIMMirror = false;
	}
	else 
	{
		pRegKey->QueryDWORDValue("EIMMirror",Value);
		if(Value == 0)
		{
			isEIMMirror = false;
			pRegKey->SetDWORDValue("EIMMirror",2);
		}
		else 
		{
			if(Value == 1)
			{
				isEIMMirror = true;
			}
		}
	}

	int ret=CheckDiskIsEIMDisk(this->pTargetDisk->m_DiskIndex);
	int ret1=CheckDiskIsEIMDisk(this->pSourceDisk->m_DiskIndex);
	if(!isEIMMirror&&2==ret&&2==ret1)
	{
		printf("源磁盘和目标磁盘都不是EIM卷，不能建镜像关系！");
		delete(pRegKey);
		return ;
	}

	//if(1==this->m_EIMMode)//source
	//{
	//	int ret=CheckDiskIsEIMDisk(this->pSourceDisk->m_DiskIndex);
	//	if(-1==ret)
	//	{
	//		return;
	//	}
	//	else if(2==ret)
	//	{
	//		if(MessageBox::Show("源磁盘不是EIM卷，最好不要选source模式，是否继续？","",MessageBoxButtons::YesNo)!=System::Windows::Forms::DialogResult::Yes)
	//		{
	//			return;
	//		}
	//	}
	//}
	if(2==this->m_EIMMode)//target
	{
		int ret=CheckDiskIsEIMDisk(this->pTargetDisk->m_DiskIndex);
		if(-1==ret)
		{
			delete(pRegKey);
			return;
		}
		else if(2==ret)
		{
			printf("目标卷不是EIM卷");
		}
	}
	//else if(4==this->m_EIMMode)//both
	//{
	//	int ret=CheckDiskIsEIMDisk(this->pTargetDisk->m_DiskIndex);
	//	int ret1=CheckDiskIsEIMDisk(this->pSourceDisk->m_DiskIndex);
	//	if(-1==ret||-1==ret)
	//	{
	//		return;
	//	}
	//	else if(2==ret||2==ret1)
	//	{
	//		if(MessageBox::Show("目标磁盘或者是源磁盘不是EIM卷，最好不要选both模式，是否继续？","",MessageBoxButtons::YesNo)!=System::Windows::Forms::DialogResult::Yes)
	//		{
	//			return;
	//		}
	//	}
	//}

	// check the partition information on this disk
	int tgtret= OsnCheckGptDisk(this->pTargetDisk->m_DiskIndex);
	int srcret= OsnCheckGptDisk(this->pSourceDisk->m_DiskIndex);
	// MessageBox::Show(String::Concat("target=",tgtret.ToString()));
	//MessageBox::Show(String::Concat("source=",srcret.ToString()));
	if(tgtret>1||srcret>1)
	{
		printf("源卷和目标卷中有一个卷处于被保护状态或者其他未知状态，请检查是否处于镜像关系中，或者是已经删除镜像关系但要重启后生效");
		delete(pRegKey);
		return;
	}
	if(tgtret!=srcret)
	{
		printf("源卷和目标卷一个是MBR磁盘，一个是GPT磁盘，不能创建镜像关系,请重新选择目标卷");
		delete(pRegKey);
		return;
	}

	int ErrorCode =OSNCheckDiskRawProperty(pTargetDisk->m_DiskIndex);
	if(ErrorCode!=0)
	{
		printf("镜像磁盘已经有分区存在");
	}
	if(this->pTargetDisk->m_Guid->compare(L"00000000-0000-0000-0000-000000000000"))//没有初始化，需要执行初始化操作
	{
		int ret=InitalizeDisk(this->pTargetDisk->m_DiskIndex);
		if(1==ret)
		{
			delete(pRegKey);
			return;
		}
		else
		{
			DISK_INFO pDisk;
			int ErrorCode;
			memset(&pDisk,0,sizeof(DISK_INFO));
			pDisk.m_DiskIndex=this->pTargetDisk->m_DiskIndex;
			ErrorCode=OsnGetDisk(&pDisk);
			if(ErrorCode==0)
			{     
				wstring *chguid = NULL;
				char guidC[64];
				wchar_t guidW[64];
				OsnGUIDToString(guidC,pDisk.m_DiskID.SAN_VolumeID.m_VolumeGuid);
				CharToWchar(guidC,guidW,_countof(guidW));
				chguid=new wstring(guidW);
				this->pTargetDisk->m_Guid=chguid;
				this->pTargetDisk->m_DiskSize=pDisk.m_DiskSize;
			}
		}
	}

	if(this->pSourceDisk->m_DiskSize>this->pTargetDisk->m_DiskSize)
	{
		printf("镜像磁盘比源磁盘要小");
	}

	delete(pRegKey);

	return;
}


void CNewMirror::MirrorOKClick(wstring *pSrcGuid,wstring *pDesGuid) 
{
	/*if(this->comboBoxMode->Text->Equals(String::Empty))
	{
		printf("请选择EIM卷的角色。");
		return;
	}
	if(this->comboBoxMode->Text->Equals("Source"))
	{
		this->m_EIMMode = 1;
	}
	else if(this->comboBoxMode->Text->Equals("Target"))
	{
		this->m_EIMMode = 2;
	}
	else if(this->comboBoxMode->Text->Equals("Both"))
	{
		this->m_EIMMode = 4;			
	}*/

	this->m_EIMMode = 2;
	
	/*if(this->ckbCluster->Checked)
	{

		RegistryKey ^pRegKey=Registry::LocalMachine;
		RegistryKey ^pParmeterKey;

		pParmeterKey=pRegKey->OpenSubKey(String::Concat("SYSTEM\\CurrentControlSet\\Services\\OSNHBService","\\Parameters"),true);
		if(pParmeterKey==nullptr)
		{
			printf("请先设置集群IP！");
			return ;
		}
		else if(pParmeterKey->GetValue("PartnerName")==nullptr||pParmeterKey->GetValue("PriIPAddress")==nullptr||
			pParmeterKey->GetValue("SecIPAddress")==nullptr)
		{
			printf("请先设置集群IP！");
			return;
		}
		if(bNoClusterflag)
		{
			int ret = ::MessageBox(NULL,TEXT("未检测到集群，请确认是否继续。如果继续，可能镜像会无法正确工作。"),TEXT("Warning"),MB_OKCANCEL);
			if(ret == IDCANCEL)
				return ;
		}
		else
		{
			if(this->cbResource->SelectedValue->ToString() == "")
			{
				printf("请设置源盘在集群中的资源名称！");
				return;
			}
		}
		this->m_IsCluster=1;
		if(bNoClusterflag)
			this->m_ClusterResourceName = "";
		else
			this->m_ClusterResourceName = this->cbResource->SelectedValue->ToString();
	}*/


	if(this->Type ==VolumeCopy)
	{
		VolumeMirrorClick(pSrcGuid,pDesGuid);
	}
	else
	{
		DiskMirrorClick(pSrcGuid,pDesGuid);
	}
}
//void CNewMirror::ckbCluster_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
//{
//	if(ckbCluster->Checked)
//	{
//		this->cbResource->Enabled =true;
//		if(CheckClusterResourceNameBind())
//		{
//			this->cbResource->DataSource = arraylist;
//		}
//	}
//	else if(!ckbCluster->Checked)
//		this->cbResource->Enabled = false;
//}
//
//bool CNewMirror::CheckClusterResourceNameBind()
//{
//	LPCLUSTERVERSIONINFO lpClusterInfo;
//	HCLUSENUM hclusenum;
//	HCLUSTER hcluster;
//	WCHAR	clusterName[128] ;	
//	WCHAR	nodeName[10][128];
//	WCHAR	*pResourceName;
//	DWORD	chClusterName =128;
//	DWORD	dwtype;
//	DWORD	enumCount;
//	DWORD	chNodeNameLen;
//	WCHAR *szbuffer;
//	DWORD ret = 0;
//
//	memset(clusterName,0,128);
//
//	for(int i = 0;i<10;i++)
//	{
//		memset(nodeName[i],0,128);
//	}
//	DWORD nodeState = ClusterNodeStateUnknown;
//
//	lpClusterInfo = (LPCLUSTERVERSIONINFO)malloc(sizeof(CLUSTERVERSIONINFO));
//	try
//	{
//		hcluster = OpenCluster(NULL);//name is NULL ,through LPC get the local cluster handle
//		if(hcluster == NULL)
//		{
//			free(lpClusterInfo);
//			bNoClusterflag = true;
//			return  false;
//		}
//		else
//		{
//			bNoClusterflag = false;
//		}
//
//		ret = GetClusterInformation(hcluster,clusterName,&chClusterName,lpClusterInfo);
//		if(ret !=ERROR_SUCCESS)
//		{
//			free(lpClusterInfo);
//			return false;
//		}
//
//		hclusenum = ClusterOpenEnum(hcluster,CLUSTER_ENUM_RESOURCE);
//		if(!hclusenum)
//		{
//			free(lpClusterInfo);
//			CloseCluster(hcluster);
//			return false;
//		}
//
//		dwtype = CLUSTER_ENUM_RESOURCE;
//		pResourceName = (LPWSTR)malloc(256);
//		szbuffer = (WCHAR *)malloc(256);
//		enumCount = ClusterGetEnumCount(hclusenum);
//		for(DWORD i= 0 ;i<enumCount ;++i)
//		{
//			chNodeNameLen = 256;
//			memset(szbuffer,0,sizeof(szbuffer));
//
//			ret = ClusterEnum(hclusenum,i,&dwtype,pResourceName,&chNodeNameLen);//get enum something
//			if(ret == ERROR_SUCCESS)
//			{
//				setlocale(LC_CTYPE, "chs");
//				String ^dd = Marshal::PtrToStringUni((IntPtr)pResourceName);
//				arraylist->Add(dd);
//				setlocale(LC_CTYPE, "C");
//			}
//
//		}
//	}catch(...)
//	{
//
//	}		
//	free(szbuffer);
//	free(pResourceName);
//
//	free(lpClusterInfo);
//	//CloseClusterNode(hnode);
//	ClusterCloseEnum(hclusenum);
//	CloseCluster(hcluster);
//	return true;
//}