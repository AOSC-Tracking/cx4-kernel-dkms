#include "CBiosChipShare.h"
#include "../CBiosHwShare.h"

static CBIOS_VOID cbWriteU32WithMask(PCBIOS_VOID pAdapterContext, CBIOS_U32 RegisterPort, CBIOS_U32 Value, CBIOS_U32 Mask)
{
    CBIOS_U32 Temp = 0;

    Temp = cb_ReadU32(pAdapterContext, RegisterPort) & Mask;
    Value &= (~Mask);
    Value |= Temp;
    cb_WriteU32(pAdapterContext, RegisterPort, Value);

    return;
}

static CBIOS_STATUS cbEfuseReadDWord(PCBIOS_VOID pvcbe, CBIOS_U32 dwordaddr, PCBIOS_U32 pdata)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    PCBIOS_VOID pAdapterContext = pcbe->pAdapterContext;
    REG_MM82B8    EfuseAddressRegValue, EfuseAddressRegMask;
    REG_MM82C0    EfuseA_ReadRegValue, EfuseA_ReadModeRegMask;
    CBIOS_U32     DelayTimes = 0;

    // To read address below 0x6D, need to read lock byte 0x6D first
    if(dwordaddr <= 0x6D)
    {
        // Write lock byte address
        EfuseAddressRegValue.Value = 0;
        EfuseAddressRegValue.EFUSE_read_Address = 0x6D;
        EfuseAddressRegMask.Value = 0xFFFFFFFF;
        EfuseAddressRegMask.EFUSE_read_Address = 0;
        cbWriteU32WithMask(pAdapterContext, 0x82B8,EfuseAddressRegValue.Value,EfuseAddressRegMask.Value);

        // Send read command
        EfuseA_ReadRegValue.Value= 0;
        EfuseA_ReadRegValue.EFUSE_read_request = 1;
        EfuseA_ReadRegValue.Reserved_1= 0x0&0x3; //00
        EfuseA_ReadRegValue.efuse_mode=0x2&0x7;//010
        EfuseA_ReadModeRegMask.Value = 0xFFFFFFFF;
        EfuseA_ReadModeRegMask.EFUSE_read_request = 0;
        EfuseA_ReadModeRegMask.Reserved_1= 0;
        EfuseA_ReadModeRegMask.efuse_mode=0;
        cbWriteU32WithMask(pAdapterContext, 0x82C0,EfuseA_ReadRegValue.Value,EfuseA_ReadModeRegMask.Value);
    }

    // Write read efuse address
    EfuseAddressRegValue.Value = 0;
    EfuseAddressRegValue.EFUSE_read_Address = dwordaddr;
    EfuseAddressRegMask.Value = 0xFFFFFFFF;
    EfuseAddressRegMask.EFUSE_read_Address = 0;
    cbWriteU32WithMask(pAdapterContext, 0x82B8,EfuseAddressRegValue.Value,EfuseAddressRegMask.Value);

    // Send read command
    EfuseA_ReadRegValue.Value= 0;
    EfuseA_ReadRegValue.EFUSE_read_request = 1;
    EfuseA_ReadRegValue.Reserved_1= 0x0&0x3;
    EfuseA_ReadRegValue.efuse_mode=0x2&0x7;//010
    EfuseA_ReadModeRegMask.Value = 0xFFFFFFFF;
    EfuseA_ReadModeRegMask.EFUSE_read_request = 0;
    EfuseA_ReadModeRegMask.Reserved_1= 0;
    EfuseA_ReadModeRegMask.efuse_mode=0;
    cbWriteU32WithMask(pAdapterContext, 0x82C0,EfuseA_ReadRegValue.Value,EfuseA_ReadModeRegMask.Value);

    //check busy bit;[25]
    do
    {
        cb_DelayMicroSeconds(50);
        DelayTimes++;
        EfuseA_ReadRegValue.Value = cb_ReadU32(pAdapterContext, 0x82C0);  // check busy bit
    }while(EfuseA_ReadRegValue.EFUSE_read_request != 0 && DelayTimes < 20);

    if(DelayTimes >= 20)
    {
        return CBIOS_ER_TIMEOUT;
    }

    // read efuse data DWORD
    *pdata = cb_ReadU32(pAdapterContext, 0x82BC);
    
    return CBIOS_OK;
}

CBIOS_STATUS cbEfuseReadByte(PCBIOS_VOID pvcbe, CBIOS_U32 byteaddr, PCBIOS_UCHAR pdata)
{
    CBIOS_U32 dwordaddr = byteaddr >> 2;
    CBIOS_U32 data = 0;
    CBIOS_U32 status;

    status = cbEfuseReadDWord(pvcbe, dwordaddr, &data);

    if(status == CBIOS_OK)
    {
        *pdata = (CBIOS_UCHAR)(data >> ((byteaddr & 0x03) * 8));
    }

    return status;
}

static CBIOS_STATUS cbEfuseWriteByte(PCBIOS_VOID pvcbe, CBIOS_U32 byteaddr, CBIOS_UCHAR indata)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    PCBIOS_VOID pAdapterContext = pcbe->pAdapterContext;
    CBIOS_U32 dwaddr;
    CBIOS_U32 dwkey;
    CBIOS_U32 j=0;
    REG_MM33688    EfuseBeginRegValue, EfuseBeginRegMask;
    REG_MM82C0    EfuseModeRegValue, EfuseModeRegMask;
    REG_MM332DC    EfusePGMAddressRegValue, EfusePGMAddressRegMask;
    CBIOS_U32     DelayTimes = 0;
    // Set MM33688[26] = 1'b1
    EfuseBeginRegValue.Value = 0;
    EfuseBeginRegValue.PGM_OPEN = 1;
    EfuseBeginRegMask.Value = 0xFFFFFFFF;
    EfuseBeginRegMask.PGM_OPEN = 0;
    cbWriteU32WithMask(pAdapterContext, 0x33688,EfuseBeginRegValue.Value,EfuseBeginRegMask.Value);

    // MM82C0[30:28] = 3'b011
    EfuseModeRegValue.Value = 0;
    EfuseModeRegValue.efuse_mode = 3;
    EfuseModeRegMask.Value = 0xFFFFFFFF;
    EfuseModeRegMask.efuse_mode = 0;
    cbWriteU32WithMask(pAdapterContext, 0x82C0,EfuseModeRegValue.Value,EfuseModeRegMask.Value);

    dwaddr = (byteaddr >> 2) & 0xFF;
    dwkey = (CBIOS_U32)indata << (8 * (byteaddr & 0x03));
    for(j = 0; j < 32; j++)
    {
        if((dwkey & (1U << j)) != 0)
        {
            // Set address and start program
            EfusePGMAddressRegValue.Value=0;
            EfusePGMAddressRegValue.PGM_EFUSE_REQ=1;
            EfusePGMAddressRegValue.PGM_EFUSE_ADDR=((j << 8) & 0x000001F00) + dwaddr; 
            EfusePGMAddressRegMask.Value = 0xFFFFFFFF;
            EfusePGMAddressRegMask.PGM_EFUSE_REQ = 0;
            EfusePGMAddressRegMask.PGM_EFUSE_ADDR = 0;
            cbWriteU32WithMask(pAdapterContext, 0x332DC,EfusePGMAddressRegValue.Value,EfusePGMAddressRegMask.Value);

            do
            {
                cb_DelayMicroSeconds(50);
                DelayTimes++;
                EfusePGMAddressRegValue.Value = cb_ReadU32(pAdapterContext, 0x332DC);  // check busy bit
            }while(EfusePGMAddressRegValue.PGM_EFUSE_REQ != 0 && DelayTimes < 20);

            if(DelayTimes >= 20)
            {
                return CBIOS_ER_TIMEOUT;
            }
        }
    }

        // Set MM332DC[30] = 1'b1
    EfusePGMAddressRegValue.Value = 0;
    EfusePGMAddressRegValue.PGM_EFUSE_FINISH = 1;
    EfusePGMAddressRegMask.Value = 0xFFFFFFFF;
    EfusePGMAddressRegMask.PGM_EFUSE_FINISH = 0;
    cbWriteU32WithMask(pAdapterContext, 0x332DC,EfusePGMAddressRegValue.Value,EfusePGMAddressRegMask.Value);

    // Set MM332DC[30] = 1'b0
    EfusePGMAddressRegValue.Value = 0;
    EfusePGMAddressRegValue.PGM_EFUSE_FINISH = 0;
    EfusePGMAddressRegMask.Value = 0xFFFFFFFF;
    EfusePGMAddressRegMask.PGM_EFUSE_FINISH = 0;
    cbWriteU32WithMask(pAdapterContext, 0x332DC,EfusePGMAddressRegValue.Value,EfusePGMAddressRegMask.Value);

    // MM82C0[30:28] = 3'b000 standby mode
    EfuseModeRegValue.Value = 0;
    EfuseModeRegValue.efuse_mode = 0;
    EfuseModeRegMask.Value = 0xFFFFFFFF;
    EfuseModeRegMask.efuse_mode = 0;
    cbWriteU32WithMask(pAdapterContext, 0x82C0,EfuseModeRegValue.Value,EfuseModeRegMask.Value);

    // Set MM33688[26] = 1'b0
    EfuseBeginRegValue.Value = 0;
    EfuseBeginRegValue.PGM_OPEN = 0;
    EfuseBeginRegMask.Value = 0xFFFFFFFF;
    EfuseBeginRegMask.PGM_OPEN = 0;
    cbWriteU32WithMask(pAdapterContext, 0x33688,EfuseBeginRegValue.Value,EfuseBeginRegMask.Value);

    return CBIOS_OK;
}

CBIOS_STATUS cbEfuseWriteByteWithMask(PCBIOS_VOID pvcbe, CBIOS_U32 byteaddr, CBIOS_UCHAR indata, CBIOS_UCHAR mask)
{
    CBIOS_UCHAR tempdata;
    CBIOS_U32 status;

    status = cbEfuseReadByte(pvcbe,  byteaddr, &tempdata);

    if(status == CBIOS_OK)
    {
        tempdata &= mask;
        indata &= (~mask);
        indata |= tempdata;

        status = cbEfuseWriteByte(pvcbe, byteaddr, indata);
    }

    return status;

}

