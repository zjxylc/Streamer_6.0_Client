
#ifndef _DDOsnCDP_H
#define _DDOsnCDP_H


#include "OSNDefs.h"

typedef struct	_CDP_SNAPSHOTID
{
	VOLUMEID		m_VolumeID;
	unsigned int	m_SnapshotIndex;		//0-based, first-ever snapshot of a volume is 0
	LARGE_INTEGER	m_SnapshotTime;			//system time when snapshot is executed
} CDP_SNAPSHOTID, *PCDP_SNAPSHOTID;

typedef struct _SNAPSHOTSTREAM_ID// indicate cdp snapshotstream
{
    char m_VolumeID[43];//osn_{}
    unsigned int  m_SnapshotCount;
}SNAPSHOTSTREAM_ID,*PSNAPSHOTSTREAM_ID;



#define OSNCDP_DEVICE_TYPE					60009
#define OSNCDP_MAKE_IOCTL(t,c)\
        (ULONG)CTL_CODE((t), 0x800+(c), METHOD_BUFFERED, FILE_ANY_ACCESS)



typedef enum _ACTIVE_IRP_STATE 
{
    STATE_SOURCE_NEW,
	STATE_TARGET_NEW,
    STATE_SOURCE_READ_LOCAL_PENDING,
    STATE_SOURCE_READ_LOCAL_DONE,
	STATE_SOURCE_WRITE_LOCAL_PENDING,
	STATE_SOURCE_WRITE_SNAPSHOT_PENDING,
	STATE_TARGET_READ_PENDING,
	STATE_ALL_COMPLETED
} ACTIVE_IRP_STATE;




typedef struct	_CDP_RESTORATION_INFO
{
	VOLUMEID		m_SourceVolumeID;
	VOLUMEID		m_TargetVolumeID;
	LARGE_INTEGER	m_RestoreSnapshotTime;			//system time of snapshot to start restoration
	LARGE_INTEGER	m_StopSnapshotTime;				//system time of snapshot to stop restoration
} CDP_RESTORATION_INFO, *PCDP_RESTORATION_INFO;


typedef struct _CDP_BUFFER_INFO
{
	VOLUMEID    m_VolumeID;
	VOLUMEID    m_BufferVolumeID;
	BOOLEAN     m_DualPortShared;
	BOOLEAN     m_NewAssigned;
	ULONG		   m_UnitSize;
	ULONG	       m_IncrementPercentage;
	ULONGLONG      m_LimitSize;
} CDP_BUFFER_INFO, *PCDP_BUFFER_INFO;


#define IOCTL_OSNCDP_SET_SNAPSHOT_VOLUME				OSNCDP_MAKE_IOCTL(OSNCDP_DEVICE_TYPE,	201)
#define IOCTL_OSNCDP_CREATE_SNAPSHOT					OSNCDP_MAKE_IOCTL(OSNCDP_DEVICE_TYPE,	202)
#define IOCTL_OSNCDP_DELETE_SNAPSHOT					OSNCDP_MAKE_IOCTL(OSNCDP_DEVICE_TYPE,	203)
#define IOCTL_OSNCDP_GET_SNAPSHOT_LIST					OSNCDP_MAKE_IOCTL(OSNCDP_DEVICE_TYPE,	204)
#define IOCTL_OSNCDP_GET_SNAPSHOT_INFO					OSNCDP_MAKE_IOCTL(OSNCDP_DEVICE_TYPE,	205)
#define IOCTL_OSNCDP_CREATE_VOLUME_LOOPBACK				OSNCDP_MAKE_IOCTL(OSNCDP_DEVICE_TYPE,	206)
#define IOCTL_OSNCDP_DELETE_VOLUME_LOOPBACK				OSNCDP_MAKE_IOCTL(OSNCDP_DEVICE_TYPE,	207)
#define IOCTL_OSNCDP_IS_VOLUME_LOOPBACK					OSNCDP_MAKE_IOCTL(OSNCDP_DEVICE_TYPE,	208)
#define IOCTL_OSNCDP_GET_ASSOCIATED_SNAPSHOT			OSNCDP_MAKE_IOCTL(OSNCDP_DEVICE_TYPE,	209)
#define IOCTL_OSNCDP_GET_SNAPSHOT_COUNT					OSNCDP_MAKE_IOCTL(OSNCDP_DEVICE_TYPE,	210)
#define IOCTL_OSNCDP_GET_EXTENDEDSNAPSHOT_COUNT			OSNCDP_MAKE_IOCTL(OSNCDP_DEVICE_TYPE,	211)
#define IOCTL_OSNCDP_GET_SNAPSHOT_UNITSIZE				OSNCDP_MAKE_IOCTL(OSNCDP_DEVICE_TYPE,	212)

#define IOCTL_OSNCDP_RESTORE							OSNCDP_MAKE_IOCTL(OSNCDP_DEVICE_TYPE,	213)
#define IOCTL_OSNCDP_GET_RESTORE_PROGRESS				OSNCDP_MAKE_IOCTL(OSNCDP_DEVICE_TYPE,	214)
#define IOCTL_OSNCDP_GET_RESTORE_STATE					OSNCDP_MAKE_IOCTL(OSNCDP_DEVICE_TYPE,	215)
#define IOCTL_OSNCDP_GET_RESTORE_TARGET					OSNCDP_MAKE_IOCTL(OSNCDP_DEVICE_TYPE,	216)
#define IOCTL_OSNCDP_UNSET_SNAPSHOT_VOLUME			OSNCDP_MAKE_IOCTL(OSNCDP_DEVICE_TYPE,	217)
#define IOCLT_OSNCDP_REMOVE_RESTORE_TARGET			OSNCDP_MAKE_IOCTL(OSNCDP_DEVICE_TYPE,	218)





#define IOCTL_OSNCDP_INTERNAL_CMD						OSNCDP_MAKE_IOCTL(OSNCDP_DEVICE_TYPE,	240)


#endif
