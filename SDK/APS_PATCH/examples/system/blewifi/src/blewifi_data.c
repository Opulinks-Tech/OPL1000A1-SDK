/******************************************************************************
*  Copyright 2017 - 2018, Opulinks Technology Ltd.
*  ----------------------------------------------------------------------------
*  Statement:
*  ----------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of Opulinks Technology Ltd. (C) 2018
******************************************************************************/

/**
 * @file blewifi_data.c
 * @author Michael Liao
 * @date 20 Feb 2018
 * @brief File creates the wifible_data task architecture.
 *
 */

#include "sys_os_config.h"
#include "blewifi_app.h"
#include "blewifi_data.h"
#include "blewifi_ctrl.h"
#include "blewifi_server_app.h"
#include "blewifi_wifi_api.h"
#include "blewifi_ble_api.h"
#include "wifi_api.h"
#include "lwip/netif.h"
#include "mw_ota.h"
#include "blewifi_ctrl_http_ota.h"
#include "hal_auxadc_patch.h"
#include "hal_system.h"

#define HI_UINT16(a) (((a) >> 8) & 0xFF)
#define LO_UINT16(a) ((a) & 0xFF)

typedef struct {
    uint16_t total_len;
    uint16_t remain;
    uint16_t offset;
    uint8_t *aggr_buf;
} blewifi_rx_packet_t;

blewifi_rx_packet_t g_rx_packet = {0};

static void BleWifi_OtaSendVersionRsp(uint8_t status, uint16_t pid, uint16_t cid, uint16_t fid)
{
	uint8_t data[7];
	uint8_t *p = (uint8_t *)data;

	*p++ = status;
	*p++ = LO_UINT16(pid);
	*p++ = HI_UINT16(pid);
	*p++ = LO_UINT16(cid);
	*p++ = HI_UINT16(cid);
	*p++ = LO_UINT16(fid);
	*p++ = HI_UINT16(fid);

	BleWifi_Ble_DataSendEncap(BLEWIFI_RSP_OTA_VERSION, data, 7);
}

static void BleWifi_OtaSendUpgradeRsp(uint8_t status)
{
	BleWifi_Ble_DataSendEncap(BLEWIFI_RSP_OTA_UPGRADE, &status, 1);
}

static void BleWifi_OtaSendEndRsp(uint8_t status, uint8_t stop)
{
	BleWifi_Ble_DataSendEncap(BLEWIFI_RSP_OTA_END, &status, 1);

    if (stop)
    {
        if (gTheOta)
        {
            if (status != BLEWIFI_OTA_SUCCESS)
                MwOta_DataGiveUp();
            free(gTheOta);
            gTheOta = 0;

            if (status != BLEWIFI_OTA_SUCCESS)
                BleWifi_Ctrl_MsgSend(BLEWIFI_CTRL_MSG_OTHER_OTA_OFF_FAIL, NULL, 0);
            else
                BleWifi_Ctrl_MsgSend(BLEWIFI_CTRL_MSG_OTHER_OTA_OFF, NULL, 0);
        }
    }
}

static void BleWifi_HandleOtaVersionReq(uint8_t *data, int len)
{
	uint16_t pid;
	uint16_t cid;
	uint16_t fid;
	uint8_t state = MwOta_VersionGet(&pid, &cid, &fid);
    
	BLEWIFI_INFO("BLEWIFI: BLEWIFI_REQ_OTA_VERSION\r\n");

	if (state != MW_OTA_OK) 
		BleWifi_OtaSendVersionRsp(BLEWIFI_OTA_ERR_HW_FAILURE, 0, 0, 0);
	else
		BleWifi_OtaSendVersionRsp(BLEWIFI_OTA_SUCCESS, pid, cid, fid);
}

static uint8_t BleWifi_MwOtaPrepare(uint16_t uwProjectId, uint16_t uwChipId, uint16_t uwFirmwareId, uint32_t ulImageSize, uint32_t ulImageSum)
{
	uint8_t state = MW_OTA_OK;

	state = MwOta_Prepare(uwProjectId, uwChipId, uwFirmwareId, ulImageSize, ulImageSum);
	return state;
}

static uint8_t BleWifi_MwOtaDatain(uint8_t *pubAddr, uint32_t ulSize)
{
	uint8_t state = MW_OTA_OK;

	state = MwOta_DataIn(pubAddr, ulSize);
	return state;
}

static uint8_t BleWifi_MwOtaDataFinish(void)
{
	uint8_t state = MW_OTA_OK;

	state = MwOta_DataFinish();
	return state;
}

static void BleWifi_HandleOtaUpgradeReq(uint8_t *data, int len)
{
	blewifi_ota_t *ota = gTheOta;
	uint8_t state = MW_OTA_OK;

	BLEWIFI_INFO("BLEWIFI: BLEWIFI_REQ_OTA_UPGRADE\r\n");

	if (len != 26)
	{
		BleWifi_OtaSendUpgradeRsp(BLEWIFI_OTA_ERR_INVALID_LEN);
		return;
	}

	if (ota)
	{
		BleWifi_OtaSendUpgradeRsp(BLEWIFI_OTA_ERR_IN_PROGRESS);
		return;
	}

	ota = malloc(sizeof(blewifi_ota_t));

	if (ota)
	{
		T_MwOtaFlashHeader *ota_hdr= (T_MwOtaFlashHeader*) &data[2];
		
		ota->pkt_idx = 0;
		ota->idx     = 0;		
        ota->rx_pkt  = *(uint16_t *)&data[0];
        ota->proj_id = ota_hdr->uwProjectId;
        ota->chip_id = ota_hdr->uwChipId;
        ota->fw_id   = ota_hdr->uwFirmwareId;
        ota->total   = ota_hdr->ulImageSize;
        ota->chksum  = ota_hdr->ulImageSum;		
		ota->curr 	 = 0;

		state = BleWifi_MwOtaPrepare(ota->proj_id, ota->chip_id, ota->fw_id, ota->total, ota->chksum);

        if (state == MW_OTA_OK) 
        {
	        BleWifi_OtaSendUpgradeRsp(BLEWIFI_OTA_SUCCESS);
	        gTheOta = ota;

	        BleWifi_Ctrl_MsgSend(BLEWIFI_CTRL_MSG_OTHER_OTA_ON, NULL, 0);
        }
        else
            BleWifi_OtaSendEndRsp(BLEWIFI_OTA_ERR_HW_FAILURE, TRUE);
    }
	else
	{
		BleWifi_OtaSendUpgradeRsp(BLEWIFI_OTA_ERR_MEM_CAPACITY_EXCEED);
	}
}

static uint32_t BleWifi_OtaAdd(uint8_t *data, int len)
{
	uint16_t i;
	uint32_t sum = 0;

	for (i = 0; i < len; i++)
    {
		sum += data[i];
    }

    return sum;
}

static void BleWifi_HandleOtaRawReq(uint8_t *data, int len)
{
	blewifi_ota_t *ota = gTheOta;
	uint8_t state = MW_OTA_OK;

	BLEWIFI_INFO("BLEWIFI: BLEWIFI_REQ_OTA_RAW\r\n");

	if (!ota)
	{
		BleWifi_OtaSendEndRsp(BLEWIFI_OTA_ERR_NOT_ACTIVE, FALSE);
        goto err;
	}

	if ((ota->curr + len) > ota->total)
	{
		BleWifi_OtaSendEndRsp(BLEWIFI_OTA_ERR_INVALID_LEN, TRUE);
		goto err;
    }

	ota->pkt_idx++;
	ota->curr += len;
	ota->curr_chksum += BleWifi_OtaAdd(data, len);

	if ((ota->idx + len) >= 256)
	{
		UINT16 total = ota->idx + len;
		UINT8 *s = data;
		UINT8 *e = data + len;
		UINT16 cpyLen = 256 - ota->idx;

		if (ota->idx)
		{
			MemCopy(&ota->buf[ota->idx], s, cpyLen);
			s += cpyLen;
			total -= 256;
			ota->idx = 0;

			state = BleWifi_MwOtaDatain(ota->buf, 256);
		}

		if (state == MW_OTA_OK)
		{
			while (total >= 256)
			{
				state = BleWifi_MwOtaDatain(s, 256);
				s += 256;
				total -= 256;

				if (state != MW_OTA_OK) break;
			}

			if (state == MW_OTA_OK)
			{
				MemCopy(ota->buf, s, e - s);
				ota->idx = e - s;

				if ((ota->curr == ota->total) && ota->idx)
				{
					state = BleWifi_MwOtaDatain(ota->buf, ota->idx);
				}
			}
		}
	}
	else
	{
		MemCopy(&ota->buf[ota->idx], data, len);
		ota->idx += len;

		if ((ota->curr == ota->total) && ota->idx)
		{
			state = BleWifi_MwOtaDatain(ota->buf, ota->idx);
		}
	}

	if (state == MW_OTA_OK)
	{
		if (ota->rx_pkt && (ota->pkt_idx >= ota->rx_pkt))
		{
	        BleWifi_Ble_DataSendEncap(BLEWIFI_RSP_OTA_RAW, 0, 0);
	    		ota->pkt_idx = 0;
    }
  }
    else
		BleWifi_OtaSendEndRsp(BLEWIFI_OTA_ERR_HW_FAILURE, TRUE);

err:
	return;
}

static void BleWifi_HandleOtaEndReq(uint8_t *data, int len)
{
	blewifi_ota_t *ota = gTheOta;
	uint8_t status = data[0];

	BLEWIFI_INFO("BLEWIFI: BLEWIFI_REQ_OTA_END\r\n");

	if (!ota)
	{
		BleWifi_OtaSendEndRsp(BLEWIFI_OTA_ERR_NOT_ACTIVE, FALSE);
        goto err;
    }

		if (status == BLEWIFI_OTA_SUCCESS)
		{
		if (ota->curr == ota->total)
				{
					if (BleWifi_MwOtaDataFinish() == MW_OTA_OK)
						BleWifi_OtaSendEndRsp(BLEWIFI_OTA_SUCCESS, TRUE);
                    else
						BleWifi_OtaSendEndRsp(BLEWIFI_OTA_ERR_CHECKSUM, TRUE);
	            }
	            else
				{
					BleWifi_OtaSendEndRsp(BLEWIFI_OTA_ERR_INVALID_LEN, TRUE);
	            }
	        }
			else
			{
		if (ota) MwOta_DataGiveUp();

			// APP stop OTA
			BleWifi_OtaSendEndRsp(BLEWIFI_OTA_SUCCESS, TRUE);
		}

err:
	return;
}

void BleWifi_Wifi_OtaTrigReq(void)
{
    blewifi_ctrl_http_ota_msg_send(BLEWIFI_CTRL_HTTP_OTA_MSG_TRIG, NULL, 0);
}

void BleWifi_Wifi_OtaTrigRsp(uint8_t status)
{
    BleWifi_Ble_DataSendEncap(BLEWIFI_RSP_HTTP_OTA_TRIG, &status, 1);
}

void BleWifi_Wifi_OtaDeviceVersionReq(void)        
{
    blewifi_ctrl_http_ota_msg_send(BLEWIFI_CTRL_HTTP_OTA_MSG_DEVICE_VERSION, NULL, 0);
}

void BleWifi_Wifi_OtaDeviceVersionRsp(uint16_t fid)        
{
    uint8_t data[2];
    uint8_t *p = (uint8_t *)data;

    *p++ = LO_UINT16(fid);
    *p++ = HI_UINT16(fid);

    BleWifi_Ble_DataSendEncap(BLEWIFI_RSP_HTTP_OTA_DEVICE_VERSION, data, 2);
}

void BleWifi_Wifi_OtaServerVersionReq(void)
{
    blewifi_ctrl_http_ota_msg_send(BLEWIFI_CTRL_HTTP_OTA_MSG_SERVER_VERSION, NULL, 0);
}

void BleWifi_Wifi_OtaServerVersionRsp(uint16_t fid)
{
    uint8_t data[2];
    uint8_t *p = (uint8_t *)data;

    *p++ = LO_UINT16(fid);
    *p++ = HI_UINT16(fid);

    BleWifi_Ble_DataSendEncap(BLEWIFI_RSP_HTTP_OTA_SERVER_VERSION, data, 2);
}

static void BleWifi_MP_CalVbat(uint8_t *data, int len)
{
    float fTargetVbat;

    memcpy(&fTargetVbat, &data[0], 4);
    Hal_Aux_VbatCalibration(fTargetVbat);
    BleWifi_Ble_SendResponse(BLEWIFI_RSP_MP_CAL_VBAT, 0);
}

static void BleWifi_MP_CalIoVoltage(uint8_t *data, int len)
{
    float fTargetIoVoltage;
    uint8_t ubGpioIdx;

    memcpy(&ubGpioIdx, &data[0], 1);
    memcpy(&fTargetIoVoltage, &data[1], 4);
    Hal_Aux_IoVoltageCalibration(ubGpioIdx, fTargetIoVoltage);
    BleWifi_Ble_SendResponse(BLEWIFI_RSP_MP_CAL_IO_VOLTAGE, 0);
}

static void BleWifi_MP_CalTmpr(uint8_t *data, int len)
{
    BleWifi_Ble_SendResponse(BLEWIFI_RSP_MP_CAL_TMPR, 0);
}

static void BleWifi_Eng_SysReset(uint8_t *data, int len)
{
    BleWifi_Ble_SendResponse(BLEWIFI_RSP_ENG_SYS_RESET, 0);

    // wait the BLE response, then reset the system
    osDelay(3000);
    Hal_Sys_SwResetAll();
}

// it is used in the ctrl task
void BleWifi_Ble_ProtocolHandler(uint16_t type, uint8_t *data, int len)
{
    /* parsing header type and do action, ex: scan, connection, disconnect */

    switch(type)
    {
        case BLEWIFI_REQ_SCAN:
            BLEWIFI_INFO("BLEWIFI: Recv BLEWIFI_REQ_SCAN \r\n");
            BleWifi_Wifi_DoScan(data, len);
            break;

        case BLEWIFI_REQ_CONNECT:
            BLEWIFI_INFO("BLEWIFI: Recv BLEWIFI_REQ_CONNECT \r\n");
            BleWifi_Wifi_DoConnect(data, len);
            break;

        case BLEWIFI_REQ_DISCONNECT:
            BLEWIFI_INFO("BLEWIFI: Recv BLEWIFI_REQ_DISCONNECT \r\n");
            BleWifi_Wifi_DoDisconnect();
            break;

        case BLEWIFI_REQ_RECONNECT:
            BLEWIFI_INFO("BLEWIFI: Recv BLEWIFI_REQ_RECONNECT \r\n");
            break;

        case BLEWIFI_REQ_READ_DEVICE_INFO:
            BLEWIFI_INFO("BLEWIFI: Recv BLEWIFI_REQ_READ_DEVICE_INFO \r\n");
            BleWifi_Wifi_ReadDeviceInfo();
            break;

        case BLEWIFI_REQ_WRITE_DEVICE_INFO:
            BLEWIFI_INFO("BLEWIFI: Recv BLEWIFI_REQ_WRITE_DEVICE_INFO \r\n");
            BleWifi_Wifi_WriteDeviceInfo(data, len);
            break;

        case BLEWIFI_REQ_WIFI_STATUS:
            BLEWIFI_INFO("BLEWIFI: Recv BLEWIFI_REQ_WIFI_STATUS \r\n");
            BleWifi_Wifi_SendStatusInfo(BLEWIFI_RSP_WIFI_STATUS);
            break;

        case BLEWIFI_REQ_RESET:
            BLEWIFI_INFO("BLEWIFI: Recv BLEWIFI_REQ_RESET \r\n");
            BleWifi_Wifi_ResetRecord();
            break;

        case BLEWIFI_REQ_OTA_VERSION:
            BLEWIFI_INFO("BLEWIFI: Recv BLEWIFI_REQ_OTA_VERSION \r\n");
			BleWifi_HandleOtaVersionReq(data, len);
            break;
    
		case BLEWIFI_REQ_OTA_UPGRADE:
		    BLEWIFI_INFO("BLEWIFI: Recv BLEWIFI_REQ_OTA_UPGRADE \r\n");
			BleWifi_HandleOtaUpgradeReq(data, len);
            break;

		case BLEWIFI_REQ_OTA_RAW:
		    BLEWIFI_INFO("BLEWIFI: Recv BLEWIFI_REQ_OTA_RAW \r\n");
			BleWifi_HandleOtaRawReq(data, len);
		    break;
    
		case BLEWIFI_REQ_OTA_END:
		    BLEWIFI_INFO("BLEWIFI: Recv BLEWIFI_REQ_OTA_END \r\n");
			BleWifi_HandleOtaEndReq(data, len);
            break;

        case BLEWIFI_REQ_HTTP_OTA_TRIG:
            BLEWIFI_INFO("BLEWIFI: Recv BLEWIFI_REQ_HTTP_OTA_TRIG \r\n");
            BleWifi_Wifi_OtaTrigReq();
            break;

        case BLEWIFI_REQ_HTTP_OTA_DEVICE_VERSION:
            BLEWIFI_INFO("BLEWIFI: Recv BLEWIFI_REQ_HTTP_OTA_DEVICE_VERSION \r\n");
            BleWifi_Wifi_OtaDeviceVersionReq();
            break;

        case BLEWIFI_REQ_HTTP_OTA_SERVER_VERSION:
            BLEWIFI_INFO("BLEWIFI: Recv BLEWIFI_REQ_HTTP_OTA_SERVER_VERSION \r\n");
            BleWifi_Wifi_OtaServerVersionReq();
            break;

        case BLEWIFI_REQ_MP_CAL_VBAT:
            BLEWIFI_INFO("BLEWIFI: Recv BLEWIFI_REQ_MP_CAL_VBAT \r\n");
            BleWifi_MP_CalVbat(data, len);
            break;

        case BLEWIFI_REQ_MP_CAL_IO_VOLTAGE:
            BLEWIFI_INFO("BLEWIFI: Recv BLEWIFI_REQ_MP_CAL_IO_VOLTAGE \r\n");
            BleWifi_MP_CalIoVoltage(data, len);
            break;

        case BLEWIFI_REQ_MP_CAL_TMPR:
            BLEWIFI_INFO("BLEWIFI: Recv BLEWIFI_REQ_MP_CAL_TMPR \r\n");
            BleWifi_MP_CalTmpr(data, len);
            break;

        case BLEWIFI_REQ_ENG_SYS_RESET:
            BLEWIFI_INFO("BLEWIFI: Recv BLEWIFI_REQ_ENG_SYS_RESET \r\n");
            BleWifi_Eng_SysReset(data, len);
            break;

        case BLEWIFI_REQ_ENG_WIFI_MAC_WRITE:
            BLEWIFI_INFO("BLEWIFI: Recv BLEWIFI_REQ_ENG_WIFI_MAC_WRITE \r\n");
            BleWifi_Wifi_MacAddrWrite(data, len);
            break;

        case BLEWIFI_REQ_ENG_WIFI_MAC_READ:
            BLEWIFI_INFO("BLEWIFI: Recv BLEWIFI_REQ_ENG_WIFI_MAC_READ \r\n");
            BleWifi_Wifi_MacAddrRead(data, len);
            break;

        case BLEWIFI_REQ_ENG_BLE_MAC_WRITE:
            BLEWIFI_INFO("BLEWIFI: Recv BLEWIFI_REQ_ENG_BLE_MAC_WRITE \r\n");
            BleWifi_Ble_MacAddrWrite(data, len);
            break;

        case BLEWIFI_REQ_ENG_BLE_MAC_READ:
            BLEWIFI_INFO("BLEWIFI: Recv BLEWIFI_REQ_ENG_BLE_MAC_READ \r\n");
            BleWifi_Ble_MacAddrRead(data, len);
            break;

        default:
            break;
    }
}

// it is used in the ctrl task
void BleWifi_Ble_DataRecvHandler(uint8_t *data, int data_len)
{
    blewifi_hdr_t *hdr = NULL;
    int hdr_len = sizeof(blewifi_hdr_t);

    /* 1.aggregate fragment data packet, only first frag packet has header */
    /* 2.handle blewifi data packet, if data frame is aggregated completely */
    if (g_rx_packet.offset == 0)
    {
        hdr = (blewifi_hdr_t*)data;
        g_rx_packet.total_len = hdr->data_len + hdr_len;
        g_rx_packet.remain = g_rx_packet.total_len;
        g_rx_packet.aggr_buf = malloc(g_rx_packet.total_len);

        if (g_rx_packet.aggr_buf == NULL) {
           BLEWIFI_ERROR("%s no mem, len %d\n", __func__, g_rx_packet.total_len);
           return;
        }
    }

    // error handle
    // if the size is overflow, don't copy the whole data
    if (data_len > g_rx_packet.remain)
        data_len = g_rx_packet.remain;

    memcpy(g_rx_packet.aggr_buf + g_rx_packet.offset, data, data_len);
    g_rx_packet.offset += data_len;
    g_rx_packet.remain -= data_len;

    /* no frag or last frag packet */
    if (g_rx_packet.remain == 0)
    {
        hdr = (blewifi_hdr_t*)g_rx_packet.aggr_buf;
        BleWifi_Ble_ProtocolHandler(hdr->type, g_rx_packet.aggr_buf + hdr_len,  (g_rx_packet.total_len - hdr_len));
        g_rx_packet.offset = 0;
        g_rx_packet.remain = 0;
        free(g_rx_packet.aggr_buf);
        g_rx_packet.aggr_buf = NULL;
    }
}

void BleWifi_Ble_DataSendEncap(uint16_t type_id, uint8_t *data, int total_data_len)
{
    blewifi_hdr_t *hdr = NULL;
    int remain_len = total_data_len;

    /* 1.fragment data packet to fit MTU size */

    /* 2.Pack blewifi header */
    hdr = malloc(sizeof(blewifi_hdr_t) + remain_len);
    if (hdr == NULL)
    {
        BLEWIFI_ERROR("BLEWIFI: memory alloc fail\r\n");
        return;
    }

    hdr->type = type_id;
    hdr->data_len = remain_len;
    if (hdr->data_len)
        memcpy(hdr->data, data, hdr->data_len);

    BLEWIFI_DUMP("[BLEWIFI]:out packet", (uint8_t*)hdr, (hdr->data_len + sizeof(blewifi_hdr_t)));

    /* 3.send app data to BLE stack */
    BleWifi_Ble_SendAppMsgToBle(BLEWIFI_APP_MSG_SEND_DATA, (hdr->data_len + sizeof(blewifi_hdr_t)), (uint8_t *)hdr);

    free(hdr);
}

void BleWifi_Ble_SendResponse(uint16_t type_id, uint8_t status)
{
    BleWifi_Ble_DataSendEncap(type_id, &status, 1);
}
