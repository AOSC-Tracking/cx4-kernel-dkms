#ifndef _CBIOS_EFUSE_H_
#define _CBIOS_EFUSE_H_

CBIOS_STATUS cbEfuseReadByte(PCBIOS_VOID pvcbe, CBIOS_U32 byteaddr, PCBIOS_UCHAR pdata);
CBIOS_STATUS cbEfuseWriteByteWithMask(PCBIOS_VOID pcbe, CBIOS_U32 byte_addr, CBIOS_UCHAR indata, CBIOS_UCHAR mask);


#endif
