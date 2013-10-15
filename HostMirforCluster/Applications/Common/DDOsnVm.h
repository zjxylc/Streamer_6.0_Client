
#ifndef _DDOsnVm_H
#define _DDOsnVm_H

#define OSNVM_DEVICE_TYPE					60002

#define IOCTL_OSNVM_CREATE_GROUP	CTL_CODE(OSNVM_DEVICE_TYPE, 1024, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_OSNVM_DELETE_GROUP	CTL_CODE(OSNVM_DEVICE_TYPE, 1025, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_OSNVM_LIST_GROUP		CTL_CODE(OSNVM_DEVICE_TYPE, 1026, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_OSNVM_ADD_DISK		CTL_CODE(OSNVM_DEVICE_TYPE, 1027, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_OSNVM_REMOVE_DISK		CTL_CODE(OSNVM_DEVICE_TYPE, 1028, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_OSNVM_LIST_DISK		CTL_CODE(OSNVM_DEVICE_TYPE, 1029, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_OSNVM_ADD_VOLUME		CTL_CODE(OSNVM_DEVICE_TYPE, 1030, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_OSNVM_REMOVE_VOLUME	CTL_CODE(OSNVM_DEVICE_TYPE, 1031, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_OSNVM_LIST_VOLUME		CTL_CODE(OSNVM_DEVICE_TYPE, 1032, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_OSNVM_RESCAN			CTL_CODE(OSNVM_DEVICE_TYPE, 1033, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_OSNVM_GET_GROUP_INFO	CTL_CODE(OSNVM_DEVICE_TYPE, 1034, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_OSNVM_GET_VOLUME_INFO	CTL_CODE(OSNVM_DEVICE_TYPE, 1035, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_OSNVM_SET_VOLUME_INFO	CTL_CODE(OSNVM_DEVICE_TYPE, 1036, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_OSNVM_CONFIG_VOLUME	CTL_CODE(OSNVM_DEVICE_TYPE, 1037, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_OSNVM_UNCONFIG_VOLUME	CTL_CODE(OSNVM_DEVICE_TYPE, 1038, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_OSN_HA_GET_DM_SKIP_INFO	CTL_CODE(OSNVM_DEVICE_TYPE, 1039, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_OSNVM_FORCE_RELOAD		CTL_CODE(OSNVM_DEVICE_TYPE, 1040, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_OSNVM_CREATE_GROUP_BY_GUID	CTL_CODE(OSNVM_DEVICE_TYPE, 1041, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_OSNVM_ADD_VOLUME_BY_GUID		CTL_CODE(OSNVM_DEVICE_TYPE, 1042, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_OSNTVM_GET_STORAGE_DEVICEID	CTL_CODE(OSNVM_DEVICE_TYPE, 1043, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_OSNVM_FORCE_RELOAD_OFFLINE	CTL_CODE(OSNVM_DEVICE_TYPE, 1044, METHOD_BUFFERED, FILE_ANY_ACCESS)

//note 
//all size/offset/length are in blocks
//

#define VOLUME_GROUP_RUNNING		1
#define VOLUME_GROUP_MISSING_DISKS	2
#define VOLUME_GROUP_FOREIGN		3
#define VOLUME_GROUP_OFFLINE		4

#define VOLUME_ONLINE				    0
#define VOLUME_NOT_READY			1
#define VOLUME_MISSING_DISKS		2

#define VOLUME_NAME_LEN				64

DEFINE_GUID(OsnVolumeClassGuid, 
0x1a229ce3, 0xd242, 0x4821, 0xb0, 0xd5, 0x24, 0x75, 0x3a, 0x26, 0x57, 0x1b);

#endif
