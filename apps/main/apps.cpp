/***************************************************************************
 *
 * Copyright 2015-2019 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#include "stdio.h"
#include "cmsis_os.h"
#include "list.h"
#include "string.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "hal_bootmode.h"
#include "hal_sleep.h"
#include "pmu.h"
#include "audioflinger.h"
#include "apps.h"
#include "app_thread.h"
#include "app_key.h"
#include "app_bt_media_manager.h"
#include "app_pwl.h"
#include "app_audio.h"
#include "app_overlay.h"
#include "app_battery.h"
#include "app_trace_rx.h"
#include "app_utils.h"
#include "app_status_ind.h"
#include "bt_drv_interface.h"
#include "besbt.h"
#include "norflash_api.h"
#include "nvrecord.h"
#include "nvrecord_dev.h"
#include "nvrecord_env.h"
#include "crash_dump_section.h"
#include "log_section.h"
#include "factory_section.h"
#include "a2dp_api.h"
#include "me_api.h"
#include "os_api.h"
#include "btapp.h"
#include "app_bt.h"
#include "app_hfp.h"
#include "bt_if.h"

#ifdef __IAG_BLE_INCLUDE__
#include "app_ble_include.h"
#include "app_ble_uart.h"
#ifdef BTIF_BLE_APP_DATAPATH_SERVER
#include "app_ble_cmd_handler.h"
#endif
#endif

#include "app_bt_func.h"
#include "app_bt_cmd.h"
#if defined(BISTO_ENABLED) || defined(__AI_VOICE__)
#include "app_ai_if.h"
#include "app_ai_tws.h"
#include "app_ai_manager_api.h"
#endif
#if defined(__IAG_BLE_INCLUDE__) && defined(BLE_V2)
#include "app_ble_rx_handler.h"
#endif
#include "audio_process.h"
#ifdef BLE_V2
#ifdef AOB_UX_ENABLED
#include "aob_earphone_info.h"
#include "aob_ux_test.h"
#include "aob_stm_if.h"
#endif
#endif

#if defined(BT_WATCH_MASTER) || defined(BT_WATCH_SLAVE)
#include "app_bt_watch.h"
#endif

#ifdef __PC_CMD_UART__
#include "app_cmd.h"
#endif

#ifdef __FACTORY_MODE_SUPPORT__
#include "app_factory.h"
#include "app_factory_bt.h"
#endif

#ifdef __INTERCONNECTION__
#include "app_interconnection.h"
#include "app_interconnection_logic_protocol.h"
#include "app_ble_mode_switch.h"
#include "app_interconnection_ble.h"
#endif

#ifdef __INTERACTION__
#include "app_interaction.h"
#endif

#ifdef BISTO_ENABLED
#include "gsound_custom_reset.h"
#include "nvrecord_gsound.h"
#include "gsound_custom_actions.h"
#include "gsound_custom_ota.h"
#endif

#ifdef PROMPT_IN_FLASH
#include "nvrecord_prompt.h"
#endif

#ifdef OTA_ENABLE
#include "ota_basic.h"
#endif

#ifdef BES_OTA
#include "ota_bes.h"
#include "ota_control.h"
#include "ota_config.h"
#endif

#ifdef MEDIA_PLAYER_SUPPORT
#include "resources.h"
#include "app_media_player.h"
#endif

#ifdef BT_USB_AUDIO_DUAL_MODE
#include "btusb_audio.h"
#endif

#ifdef TILE_DATAPATH
#include "tile_target_ble.h"
#endif

#if defined(IBRT)
#include "app_ibrt_if.h"
#include "app_ibrt_customif_ui.h"
#include "app_ibrt_voice_report.h"

#if defined(IBRT_UI_V1)
#include "app_ibrt_ui_test.h"
#endif

#if defined(IBRT_CORE_V2_ENABLE)
#include "app_tws_ibrt_raw_ui_test.h"
#include "app_ibrt_tws_ext_cmd.h"
#include "app_tws_ibrt_conn_api.h"
#include "app_custom_api.h"
#endif

#endif

#ifdef GFPS_ENABLED
#include "app_gfps.h"
#endif

#ifdef ANC_APP
#include "app_anc.h"
#endif

#ifdef __THIRDPARTY
#include "app_thirdparty.h"
#endif

#ifdef AI_OTA
#include "nvrecord_ota.h"
#include "ota_common.h"
#endif

#include "watchdog/watchdog.h"
#ifdef VOICE_DETECTOR_EN
#include "app_voice_detector.h"
#endif

#ifdef IBRT_UI_V2
#include "ibrt_mgr.h"
#endif

#ifdef SENSOR_HUB
#include "mcu_sensor_hub_app.h"
#endif

#if defined(ESHELL_ON)
#include "eshell.h"
#endif

#if defined(__USE_SW_I2C__) || defined(__USE_HW_I2C__)
#include "xspace_i2c_master.h"
#endif

#if defined(__DEV_THREAD__)
#include "dev_thread.h"
#endif

#if defined(__XSPACE_XBUS_MANAGER__)  
#include "xspace_xbus_manager.h"
#endif

#if defined(__XSPACE_UI__)
#include "xspace_ui.h"
#include "xspace_dev_info.h"
#include "xspace_interface.h"
#endif

#if defined(__XSPACE_BATTERY_MANAGER__)
#include "xspace_battery_manager.h"
#endif

#if defined(__XSPACE_FLASH_ACCESS__)
#include "xspace_flash_access.h"
#endif

#if defined(ANC_ASSIST_ENABLED)
#include "app_anc_assist.h"
#include "app_voice_assist_ai_voice.h"
#include "app_voice_assist_wd.h"
#include "app_voice_assist_ultrasound.h"
#include "app_voice_assist_prompt_leak_detect.h"
#if defined(VOICE_ASSIST_ONESHOT_ADAPTIVE_ANC)
#include "app_voice_assist_oneshot_adapt_anc.h"
#endif
#endif

#ifdef AUDIO_OUTPUT_DC_AUTO_CALIB
#include "codec_calib.h"
#endif
#ifdef SECURE_BOOT_WORKAROUND_SOLUTION
#include "hal_wdt.h"
#include "watchdog/watchdog.h"
#endif

#ifdef AUDIO_DEBUG
extern "C" int32_t audio_test_cmd_init(void);
extern "C" int speech_tuning_init(void);
#endif

#ifdef AUDIO_HEARING_COMPSATN
extern "C" int32_t hearing_det_cmd_init(void);
#endif

#if (defined(BTUSB_AUDIO_MODE) || defined(BT_USB_AUDIO_DUAL_MODE))
extern "C"  bool app_usbaudio_mode_on(void) ;
#endif

#ifdef VOICE_DEV
extern "C" int32_t voice_dev_init(void);
#endif

#ifdef APP_UART_MODULE
#include "app_uart_dma_thread.h"
#endif

#if defined (__AI_VOICE__)
#include "app_tws_ibrt_raw_ui_test.h"
#endif

#define APP_BATTERY_LEVEL_LOWPOWERTHRESHOLD (1)
#define POWERON_PRESSMAXTIME_THRESHOLD_MS  (5000)

#ifdef FPGA
uint32_t __ota_upgrade_log_start[100];
#endif

enum APP_POWERON_CASE_T {
    APP_POWERON_CASE_NORMAL = 0,
    APP_POWERON_CASE_DITHERING,
    APP_POWERON_CASE_REBOOT,
    APP_POWERON_CASE_ALARM,
    APP_POWERON_CASE_CALIB,
    APP_POWERON_CASE_BOTHSCAN,
    APP_POWERON_CASE_CHARGING,
    APP_POWERON_CASE_FACTORY,
    APP_POWERON_CASE_TEST,
    APP_POWERON_CASE_INVALID,

    APP_POWERON_CASE_NUM
};

#ifdef RB_CODEC
extern int rb_ctl_init();
extern bool rb_ctl_is_init_done(void);
extern void app_rbplay_audio_reset_pause_status(void);
#endif

#ifdef GFPS_ENABLED
extern "C" void app_fast_pairing_timeout_timehandler(void);
#endif

uint8_t  app_poweroff_flag = 0;
static enum APP_POWERON_CASE_T g_pwron_case = APP_POWERON_CASE_INVALID;

#if 0//ndef APP_TEST_MODE
static uint8_t app_status_indication_init(void)
{
    struct APP_PWL_CFG_T cfg;
    memset(&cfg, 0, sizeof(struct APP_PWL_CFG_T));
    app_pwl_open();
    app_pwl_setup(APP_PWL_ID_0, &cfg);
    app_pwl_setup(APP_PWL_ID_1, &cfg);
    return 0;
}
#endif

#if defined(__BTIF_EARPHONE__) || defined(__BTIF_AUTOPOWEROFF__)

void PairingTransferToConnectable(void);

typedef void (*APP_10_SECOND_TIMER_CB_T)(void);

void app_pair_timerout(void);
void app_poweroff_timerout(void);
void CloseEarphone(void);
#if defined(__XSPACE_UI__)
static void app_timeout_poweroff_handler(void);
#endif

typedef struct
{
    uint8_t timer_id;
    uint8_t timer_en;
    uint8_t timer_count;
    uint8_t timer_period;
    APP_10_SECOND_TIMER_CB_T cb;
}APP_10_SECOND_TIMER_STRUCT;

#define INIT_APP_TIMER(_id, _en, _count, _period, _cb) \
    { \
        .timer_id = _id, \
        .timer_en = _en, \
        .timer_count = _count, \
        .timer_period = _period, \
        .cb = _cb, \
    }

APP_10_SECOND_TIMER_STRUCT app_10_second_array[] = {
    INIT_APP_TIMER(APP_PAIR_TIMER_ID, 0, 0, 6, PairingTransferToConnectable),
    INIT_APP_TIMER(APP_POWEROFF_TIMER_ID, 0, 0, 90, NULL),
#ifdef GFPS_ENABLED
    INIT_APP_TIMER(APP_FASTPAIR_LASTING_TIMER_ID, 0, 0, APP_FAST_PAIRING_TIMEOUT_IN_SECOND / 10, app_fast_pairing_timeout_timehandler),
#endif
#if defined(__XSPACE_UI__)
    INIT_APP_TIMER(APP_PAIRINGTIMEOUT_POWEROFF_TIMER_ID, 0, 0, 18, app_timeout_poweroff_handler),
    INIT_APP_TIMER(APP_TWSPAIRING_POWEROFF_TIMER_ID, 0, 0, 3, app_timeout_poweroff_handler),
#endif
};

void app_stop_10_second_timer(uint8_t timer_id)
{
    APP_10_SECOND_TIMER_STRUCT *timer = &app_10_second_array[timer_id];

    timer->timer_en = 0;
    timer->timer_count = 0;
}

void app_start_10_second_timer(uint8_t timer_id)
{
    APP_10_SECOND_TIMER_STRUCT *timer = &app_10_second_array[timer_id];

    timer->timer_en = 1;
    timer->timer_count = 0;
}

void app_set_10_second_timer(uint8_t timer_id, uint8_t enable, uint8_t period)
{
    APP_10_SECOND_TIMER_STRUCT *timer = &app_10_second_array[timer_id];

    timer->timer_en = enable;
    timer->timer_count = period;
}

void app_10_second_timer_check(void)
{
    APP_10_SECOND_TIMER_STRUCT *timer = app_10_second_array;
    unsigned int i;

    for(i = 0; i < ARRAY_SIZE(app_10_second_array); i++) {
        if (timer->timer_en) {
            timer->timer_count++;
            if (timer->timer_count >= timer->timer_period) {
                timer->timer_en = 0;
                if (timer->cb)
                    timer->cb();
            }
        }
        timer++;
    }
}

void CloseEarphone(void)
{
#if !defined(__XSPACE_UI__)
    int activeCons = 0;
    int activeSourceCons = 0;

    activeCons = btif_me_get_activeCons();
    activeSourceCons = btif_me_get_source_activeCons();
#endif

#ifdef ANC_APP
    if (app_anc_work_status()) {
        app_set_10_second_timer(APP_POWEROFF_TIMER_ID, 1, 30);
        return;
    }
#endif /* ANC_APP */
#if defined(__XSPACE_UI__)
    TRACE(0, "!!!CloseEarphone\n");
    app_shutdown();
#else
    if (activeCons == 0 && activeSourceCons == 0) {
        TRACE(0, "!!!CloseEarphone\n");
        app_shutdown();
    }
#endif
}
#if defined(__XSPACE_UI__)
static void app_timeout_poweroff_handler(void)
{
   
    TRACE(1, "%s", __func__);
    app_status_indication_set(APP_STATUS_INDICATION_POWEROFF);
    osDelay(100);
    xspace_ui_tws_send_info_sync_cmd(XUI_SYS_INFO_SYNC_SHUTDOWN, (uint8_t *)0, 0);
    TRACE(0, "Pairing Timeout,CloseEarphone\n");
    xspace_interface_delay_timer_start(3000, (uint32_t)xspace_interface_shutdown, 0, 0, 0);
}
#endif
#endif /* #if defined(__BTIF_EARPHONE__) && defined(__BTIF_AUTOPOWEROFF__) */

#ifdef FPGA
extern "C"
#endif
int signal_send_to_main_thread(uint32_t signals);
uint8_t stack_ready_flag = 0;
void app_notify_stack_ready(uint8_t ready_flag)
{
    TRACE(2,"app_notify_stack_ready %d %d", stack_ready_flag, ready_flag);//在log里面没有输出

    stack_ready_flag |= ready_flag;

#ifdef __IAG_BLE_INCLUDE__
    if(stack_ready_flag == (STACK_READY_BT|STACK_READY_BLE))
#endif
    {
        signal_send_to_main_thread(STACK_READY_BLE | STACK_READY_BT);
#ifdef IBRT
        app_ibrt_if_stack_is_ready();
#endif
    }
}

bool app_is_stack_ready(void)
{
    bool ret = false;

    if (stack_ready_flag == (STACK_READY_BT
#ifdef __IAG_BLE_INCLUDE__
                             | STACK_READY_BLE
#endif
                             ))
    {
        ret = true;
    }

    return ret;
}

__attribute__((weak)) void ble_uart_cmd_init(void)
{

}

static void app_stack_ready_cb(void)
{
    TRACE(0,"stack init done");
#ifdef __IAG_BLE_INCLUDE__
#ifdef IBRT
    app_ble_force_switch_adv(BLE_SWITCH_USER_IBRT, false);
#endif // #ifdef IBRT
    app_ble_stub_user_init();

    app_ble_start_connectable_adv(BLE_ADVERTISING_INTERVAL);
    ble_uart_cmd_init();
#if defined(AOB_UX_ENABLED) && defined(BLE_V2)
    aob_earphone_info_init();
    aob_stm_if_init();
    aob_uart_cmd_init();
#endif
#endif
}

#if defined(__XSPACE_BATTERY_MANAGER__)
static void app_10second_timer_timeout_handler(void const *param);
osTimerDef(APP_10SECOND_TIMER, app_10second_timer_timeout_handler);
static osTimerId app_10second_timer = NULL;
extern "C" uint8_t is_sco_mode(void);   //bobby moodify
static int app_10_second_event_handle(APP_MESSAGE_BODY *msg_body)
{
    xspace_battery_manager_timing_todo();
    xspace_ui_get_system_status();

#ifdef __SUPPORT_ANC_SINGLE_MODE_WITHOUT_BT__
    if (anc_single_mode_on)
        return 0;
#endif

#if defined(__BTIF_EARPHONE__)
    if (app_is_stack_ready() && (0 == is_sco_mode())) {   //bobby modify

        app_bt_state_checker();
    }
    if (0 == is_sco_mode()) {   //bobby add
        app_10_second_timer_check();
    }
#endif
    osapi_notify_evm();

    return 0;
}

static void app_10_second_event_process(void)
{
    APP_MESSAGE_BLOCK msg;

    msg.mod_id = APP_MODUAL_10_SEC_TIMER;
    msg.msg_body.message_id = (uint32_t)0;
    msg.msg_body.message_ptr = (uint32_t)0;
    msg.msg_body.message_Param0 = (uint32_t)0;
    msg.msg_body.message_Param1 = (uint32_t)0;
    app_mailbox_put(&msg);
}

static void app_10second_timer_timeout_handler(void const *param)
{
    app_10_second_event_process();//时间到，清零
}

static void app_10second_timer_init(void)
{
    TRACE(1, "%s.", __func__);

    if (app_10second_timer == NULL) {
        app_10second_timer = osTimerCreate(osTimer(APP_10SECOND_TIMER), osTimerPeriodic, NULL);
    }

    app_set_threadhandle(APP_MODUAL_10_SEC_TIMER, app_10_second_event_handle);
    osTimerStart(app_10second_timer, 10000);
}
#else
int app_status_battery_report(uint8_t level)
{
#if defined(__BTIF_EARPHONE__)
    app_10_second_timer_check();
#endif
    if (level <= APP_BATTERY_LEVEL_LOWPOWERTHRESHOLD)//低于低电量最低限度
    {
        //add something
    }

    if (app_is_stack_ready())
    {
// #if (HF_CUSTOM_FEATURE_SUPPORT & HF_CUSTOM_FEATURE_BATTERY_REPORT) || (HF_SDK_FEATURES & HF_FEATURE_HF_INDICATORS)
#if defined(SUPPORT_BATTERY_REPORT) || defined(SUPPORT_HF_INDICATORS)
#if defined(IBRT)
        uint8_t hfp_device = app_bt_audio_get_curr_hfp_device();
        struct BT_DEVICE_T *curr_device = app_bt_get_device(hfp_device);
        if (curr_device->hf_conn_flag)
        {
            app_hfp_set_battery_level(level);
        }
#else
        app_hfp_set_battery_level(level);
#endif
#else
        TRACE(1,"[%s] Can not enable SUPPORT_BATTERY_REPORT", __func__);
#endif
        osapi_notify_evm();
    }
    return 0;
}
#endif

#ifdef MEDIA_PLAYER_SUPPORT

#if defined(__XSPACE_UI__)
static bool pressure_up_playing = false;
static bool pressure_down_playing = false;
extern "C" uint8_t is_sco_mode (void);
extern "C" uint8_t is_a2dp_mode(void);
#endif

void app_status_set_num(const char* p)
{
    media_Set_IncomingNumber(p);
}

int app_voice_report_handler(APP_STATUS_INDICATION_T status, uint8_t device_id, uint8_t isMerging)
{
#if (defined(BTUSB_AUDIO_MODE) || defined(BT_USB_AUDIO_DUAL_MODE))
    if (app_usbaudio_mode_on())
    {
        return 0;
    }
#endif
    TRACE(3,"%s %d%s",__func__, status, status2str((uint16_t)status));

#if defined(__XSPACE_UI__)
    AUD_ID_ENUM current_prompt = app_prompt_checker();
    TRACE(4,"%s current_prompt:%d%s,role_switch:%d.",
        __func__ , current_prompt, aud_id2str(current_prompt), app_ibrt_customif_ui_is_tws_switching());

    if (app_poweroff_flag == 0)
    {
        if((is_sco_mode() || xspace_interface_call_is_online() || xspace_interface_call_is_incoming() || is_a2dp_mode()) &&
            status == APP_STATUS_INDICATION_CONNECTED)
        {
            XUI_TRACE(0, ">>1,a2dp mode or sco mode,can not play connect prompt voice!");
            return 0;
        }

        if ((is_sco_mode() || xspace_interface_call_is_online() || xspace_interface_call_is_incoming()))
        {
            XUI_TRACE(0, ">>2,sco mode,can not play findme prompt voice!");
            return 0;
        }

        if(
#if defined (__GESTURE_MANAGER_USE_PRESSURE__)
            (status != APP_STATUS_RING_PRESSURE_UP) &&
            (status != APP_STATUS_RING_PRESSURE_DOWN) &&
#endif
            (current_prompt != AUD_ID_INVALID || app_ibrt_customif_ui_is_tws_switching()))

        {
            TRACE(3, "%s,local media playing or role_switching,later play %d%s!!!",
                       __func__, status, status2str((uint16_t)status));
            xspace_interface_delay_timer_stop((uint32_t)app_voice_report_handler);
            xspace_interface_delay_timer_start(300, (uint32_t)app_voice_report_handler, (uint32_t)status, 0, 1);
            return 0;
        }
    
        if (current_prompt == AUD_ID_BT_PRESSURE_DOWN)
            pressure_down_playing = true;
        else
            pressure_down_playing = false;

        if (current_prompt == AUD_ID_BT_PRESSURE_UP)
            pressure_up_playing = true;
        else
            pressure_up_playing = false;
#endif
    }

    AUD_ID_ENUM id = MAX_RECORD_NUM;
    bool isLocalPlay= false;
    if (app_poweroff_flag == 1){
        switch (status) {
            case APP_STATUS_INDICATION_POWEROFF:
                id = AUD_ID_POWER_OFF;
                isLocalPlay = true;
                break;
            default:
                return 0;
                break;
        }
    }else{
        switch (status) {
            case APP_STATUS_INDICATION_POWERON:
                id = AUD_ID_POWER_ON;
                break;
            case APP_STATUS_INDICATION_POWEROFF:
                id = AUD_ID_POWER_OFF;
                isLocalPlay = true;
                isMerging = false;
                break;
            case APP_STATUS_INDICATION_CONNECTED:
                id = AUD_ID_BT_CONNECTED;
                break;
            case APP_STATUS_INDICATION_DISCONNECTED:
                id = AUD_ID_BT_DIS_CONNECT;
                break;
            case APP_STATUS_INDICATION_CALLNUMBER:
                id = AUD_ID_BT_CALL_INCOMING_NUMBER;
                break;
            case APP_STATUS_INDICATION_CHARGENEED:
                id = AUD_ID_BT_CHARGE_PLEASE;
                break;
            case APP_STATUS_INDICATION_FULLCHARGE:
                id = AUD_ID_BT_CHARGE_FINISH;
                break;
            case APP_STATUS_INDICATION_PAIRSUCCEED:
                id = AUD_ID_BT_PAIRING_SUC;
                break;
            case APP_STATUS_INDICATION_PAIRFAIL:
                id = AUD_ID_BT_PAIRING_FAIL;
                break;

            case APP_STATUS_INDICATION_HANGUPCALL:
                id = AUD_ID_BT_CALL_HUNG_UP;
                break;

            case APP_STATUS_INDICATION_REFUSECALL:
                id = AUD_ID_BT_CALL_REFUSE;
                isMerging = false;
                break;

            case APP_STATUS_INDICATION_ANSWERCALL:
                id = AUD_ID_BT_CALL_ANSWER;
                break;

            case APP_STATUS_INDICATION_CLEARSUCCEED:
                id = AUD_ID_BT_CLEAR_SUCCESS;
                break;

            case APP_STATUS_INDICATION_CLEARFAIL:
                id = AUD_ID_BT_CLEAR_FAIL;
                break;
            case APP_STATUS_INDICATION_INCOMINGCALL:
                id = AUD_ID_BT_CALL_INCOMING_CALL;
                break;
            case APP_STATUS_INDICATION_BOTHSCAN:
                id = AUD_ID_BT_PAIR_ENABLE;
                break;
            case APP_STATUS_INDICATION_WARNING:
                id = AUD_ID_BT_WARNING;
                break;
            case APP_STATUS_INDICATION_ALEXA_START:
                id = AUDIO_ID_BT_ALEXA_START;
                break;
            case APP_STATUS_INDICATION_ALEXA_STOP:
                id = AUDIO_ID_BT_ALEXA_STOP;
                break;
            case APP_STATUS_INDICATION_GSOUND_MIC_OPEN:
                id = AUDIO_ID_BT_GSOUND_MIC_OPEN;
                break;
            case APP_STATUS_INDICATION_GSOUND_MIC_CLOSE:
                id = AUDIO_ID_BT_GSOUND_MIC_CLOSE;
                break;
            case APP_STATUS_INDICATION_GSOUND_NC:
                id = AUDIO_ID_BT_GSOUND_NC;
                break;

#ifdef jinyao_learning
            case APP_STATUS_INDICATION_PROMPT_WELCOME:
                id = AUDIO_ID_BT_WELCOME;
                break;
#endif

#ifdef __BT_WARNING_TONE_MERGE_INTO_STREAM_SBC__
            case APP_STATUS_RING_WARNING:
                id = AUD_ID_RING_WARNING;
                break;
#endif
            case APP_STATUS_INDICATION_MUTE:
                id = AUDIO_ID_BT_MUTE;
                break;
#ifdef __INTERACTION__
            case APP_STATUS_INDICATION_FINDME:
                id = AUD_ID_BT_FINDME;
                break;
#endif

#if defined (__GESTURE_MANAGER_USE_PRESSURE__)
            case APP_STATUS_RING_PRESSURE_DOWN:
                id = AUD_ID_BT_PRESSURE_DOWN;
                isLocalPlay = true;
                isMerging = false;
                break;

            case APP_STATUS_RING_PRESSURE_UP:
                id = AUD_ID_BT_PRESSURE_UP;
                isLocalPlay = true;
                isMerging = false;

                break;
#endif
            case APP_STATUS_INDICATION_FIND_MY_BUDS:
                id = AUDIO_ID_FIND_MY_BUDS;
                isMerging = false;
                break;
            case APP_STATUS_INDICATION_TILE_FIND:
                id = AUDIO_ID_FIND_TILE;
                break;
#ifdef __TENCENT_VOICE__
            case APP_STATUS_INDICATION_TENCENT_START:
                id = AUDIO_ID_BT_TENCENT_START;
                break;
            case APP_STATUS_INDICATION_TENCENT_STOP:
                id = AUDIO_ID_BT_TENCENT_STOP;
                break;
#endif
            default:
                break;
        }
    }

#if defined(__XSPACE_UI__)  /**add by chenjunjie,20210121**/
    if ((pressure_down_playing) && (id == AUD_ID_BT_PRESSURE_DOWN))
    {
        return 0;
    }
    if ((pressure_up_playing) && (id == AUD_ID_BT_PRESSURE_UP))
    {
        return 0;
    }
#endif

    if(isLocalPlay)
    {
        media_PlayAudio_locally(id, device_id);//本地播放
    }
#ifndef AUDIO_PROMPT_USE_DAC2_ENABLED
    else if (isMerging)
    {
        media_PlayAudio(id, device_id);//混合播放
    }
#endif
    else
    {
        media_PlayAudio_standalone(id, device_id);//打断播放
    }
    return 0;
}

int app_voice_report(APP_STATUS_INDICATION_T status, uint8_t device_id)
{
    return app_voice_report_handler(status, device_id, true);
}

extern "C" int app_voice_report_generic(APP_STATUS_INDICATION_T status, uint8_t device_id, uint8_t isMerging)
{
    return app_voice_report_handler(status, device_id, isMerging);
}
extern "C" int app_voice_stop(APP_STATUS_INDICATION_T status, uint8_t device_id)
{
    AUD_ID_ENUM id = MAX_RECORD_NUM;

    TRACE(2,"%s %d", __func__, status);

    if (status == APP_STATUS_INDICATION_FIND_MY_BUDS)
        id = AUDIO_ID_FIND_MY_BUDS;

    if (id != MAX_RECORD_NUM)
        trigger_media_stop(id, device_id);

    return 0;
}
#endif

#ifndef __POWERKEY_CTRL_ONOFF_ONLY__
static void app_poweron_normal(APP_KEY_STATUS *status, void *param)
{
    TRACE(3,"%s %d,%d",__func__, status->code, status->event);
    g_pwron_case = APP_POWERON_CASE_NORMAL;

    signal_send_to_main_thread(0x2);
}

#if !defined(BLE_ONLY_ENABLED)
static void app_poweron_scan(APP_KEY_STATUS *status, void *param)
{
    TRACE(3,"%s %d,%d",__func__, status->code, status->event);
    g_pwron_case = APP_POWERON_CASE_BOTHSCAN;

    signal_send_to_main_thread(0x2);
}
#endif

#ifdef __ENGINEER_MODE_SUPPORT__
#if !defined(BLE_ONLY_ENABLED)
static void app_poweron_factorymode(APP_KEY_STATUS *status, void *param)
{
    TRACE(3,"%s %d,%d",__func__, status->code, status->event);
    hal_sw_bootmode_clear(HAL_SW_BOOTMODE_REBOOT);
    app_factorymode_enter();
}
#endif
#endif
#endif

void app_enter_signalingtest_mode(void)
{
    TRACE(0, "APP: enter signaling test");
    hal_sw_bootmode_set(HAL_SW_BOOTMODE_TEST_MODE|HAL_SW_BOOTMODE_TEST_SIGNALINGMODE);
    app_reset();
}

void app_enter_non_signalingtest_mode(void)
{
    TRACE(0, "APP: enter non-signaling test");
    hal_sw_bootmode_set(HAL_SW_BOOTMODE_TEST_MODE|HAL_SW_BOOTMODE_TEST_NOSIGNALINGMODE);
    app_reset();
}

#ifndef __POWERKEY_CTRL_ONOFF_ONLY__
static bool g_pwron_finished = false;
static void app_poweron_finished(APP_KEY_STATUS *status, void *param)
{
    TRACE(3,"%s %d,%d",__func__, status->code, status->event);
    g_pwron_finished = true;
    signal_send_to_main_thread(0x2);
}

void app_poweron_wait_finished(void)
{
    if (!g_pwron_finished){
        osSignalWait(0x2, osWaitForever);
    }
}
#endif

#if  defined(__POWERKEY_CTRL_ONOFF_ONLY__)
void app_bt_key_shutdown(APP_KEY_STATUS *status, void *param);
const  APP_KEY_HANDLE  pwron_key_handle_cfg[] = {
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_UP},           "power on: shutdown"     , app_bt_key_shutdown, NULL},
};
#elif defined(__ENGINEER_MODE_SUPPORT__)
const  APP_KEY_HANDLE  pwron_key_handle_cfg[] = {
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_INITUP},           "power on: normal"     , app_poweron_normal, NULL},
#if !defined(BLE_ONLY_ENABLED)
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_INITLONGPRESS},    "power on: both scan"  , app_poweron_scan  , NULL},
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_INITLONGLONGPRESS},"power on: factory mode", app_poweron_factorymode  , NULL},
#endif
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_INITFINISHED},     "power on: finished"   , app_poweron_finished  , NULL},
};
#else
const  APP_KEY_HANDLE  pwron_key_handle_cfg[] = {
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_INITUP},           "power on: normal"     , app_poweron_normal, NULL},
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_INITLONGPRESS},    "power on: both scan"  , app_poweron_scan  , NULL},
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_INITFINISHED},     "power on: finished"   , app_poweron_finished  , NULL},
};
#endif

#ifndef APP_TEST_MODE
#if defined(__OPEN_BES_KEY_HANDLE__)
static void app_poweron_key_init(void)
{
    uint8_t i = 0;
    TRACE(1, "%s", __func__);

    for (i = 0; i < (sizeof(pwron_key_handle_cfg) / sizeof(APP_KEY_HANDLE)); i++) {
        app_key_handle_registration(&pwron_key_handle_cfg[i]);
    }
}

static uint8_t app_poweron_wait_case(void)
{
    uint32_t stime = 0, etime = 0;

#ifdef __POWERKEY_CTRL_ONOFF_ONLY__
    g_pwron_case = APP_POWERON_CASE_NORMAL;
#else
    TRACE(1, "poweron_wait_case enter:%d", g_pwron_case);
    if (g_pwron_case == APP_POWERON_CASE_INVALID) {
        stime = hal_sys_timer_get();
        osSignalWait(0x2, POWERON_PRESSMAXTIME_THRESHOLD_MS);
        etime = hal_sys_timer_get();
    }
    TRACE(2, "powon raw case:%d time:%d", g_pwron_case, TICKS_TO_MS(etime - stime));
#endif
    return g_pwron_case;
}
#endif

#endif   // __OPEN_BES_KEY_HANDLE__

static void app_wait_stack_ready(void)
{
    uint32_t stime, etime;
    stime = hal_sys_timer_get();
    osSignalWait(0x3, osWaitForever);
    etime = hal_sys_timer_get();
    TRACE(1,"app_wait_stack_ready: wait:%d ms", TICKS_TO_MS(etime - stime));

    app_stack_ready_cb();
}

extern "C" int system_shutdown(void);
int app_shutdown(void)
{
    system_shutdown();
    return 0;
}

int system_reset(void);
int app_reset(void)
{
    system_reset();
    return 0;
}

static void app_postponed_reset_timer_handler(void const *param);
osTimerDef(APP_POSTPONED_RESET_TIMER, app_postponed_reset_timer_handler);
static osTimerId app_postponed_reset_timer = NULL;
#define APP_RESET_PONTPONED_TIME_IN_MS  2000
static void app_postponed_reset_timer_handler(void const *param)
{
    app_reset();
}

void app_start_postponed_reset(void)
{
    if (NULL == app_postponed_reset_timer)
    {
        app_postponed_reset_timer = osTimerCreate(osTimer(APP_POSTPONED_RESET_TIMER), osTimerOnce, NULL);
    }

#ifndef FLASH_REMAP
    hal_sw_bootmode_set(HAL_SW_BOOTMODE_ENTER_HIDE_BOOT);
#endif
    osTimerStart(app_postponed_reset_timer, APP_RESET_PONTPONED_TIME_IN_MS);
}

void app_bt_key_shutdown(APP_KEY_STATUS *status, void *param)
{
    TRACE(3,"%s %d,%d",__func__, status->code, status->event);
#ifdef __POWERKEY_CTRL_ONOFF_ONLY__
    hal_sw_bootmode_clear(HAL_SW_BOOTMODE_REBOOT);
    app_reset();
#else
    app_shutdown();
#endif
}

void app_bt_key_enter_testmode(APP_KEY_STATUS *status, void *param)
{
    TRACE(1,"%s\n",__FUNCTION__);

    if(app_status_indication_get() == APP_STATUS_INDICATION_BOTHSCAN){
#ifdef __FACTORY_MODE_SUPPORT__
        app_factorymode_bt_signalingtest(status, param);
#endif
    }
}

void app_bt_key_enter_nosignal_mode(APP_KEY_STATUS *status, void *param)
{
    TRACE(1,"%s\n",__FUNCTION__);
    if(app_status_indication_get() == APP_STATUS_INDICATION_BOTHSCAN){
#ifdef __FACTORY_MODE_SUPPORT__
        app_factorymode_bt_nosignalingtest(status, param);
#endif
    }
}

extern "C" void OS_NotifyEvm(void);

#define PRESS_KEY_TO_ENTER_OTA_INTERVEL    (15000)          // press key 15s enter to ota
#define PRESS_KEY_TO_ENTER_OTA_REPEAT_CNT    ((PRESS_KEY_TO_ENTER_OTA_INTERVEL - 2000) / 500)
void app_otaMode_enter(APP_KEY_STATUS *status, void *param)
{
    TRACE(1,"%s",__func__);

    hal_norflash_disable_protection(HAL_FLASH_ID_0);

    hal_sw_bootmode_set(HAL_SW_BOOTMODE_ENTER_HIDE_BOOT);
#ifdef __KMATE106__
    app_status_indication_set(APP_STATUS_INDICATION_OTA);
    app_voice_report(APP_STATUS_INDICATION_WARNING, 0);
    osDelay(1200);
#endif
#ifdef SECURE_BOOT_WORKAROUND_SOLUTION
    app_wdt_reopen(10);
    hal_cmu_sys_reboot();
#else
    pmu_reboot();
#endif
}

#ifdef __USB_COMM__
void app_usb_cdc_comm_key_handler(APP_KEY_STATUS *status, void *param)
{
    TRACE(3,"%s %d,%d", __func__, status->code, status->event);
    hal_sw_bootmode_clear(HAL_SW_BOOTMODE_REBOOT);
    hal_sw_bootmode_set(HAL_SW_BOOTMODE_CDC_COMM);
    pmu_usb_config(PMU_USB_CONFIG_TYPE_DEVICE);
    hal_cmu_reset_set(HAL_CMU_MOD_GLOBAL);
}
#endif

#if 0
void app_dfu_key_handler(APP_KEY_STATUS *status, void *param)
{
    TRACE(1,"%s ",__func__);
    hal_sw_bootmode_clear(HAL_SW_BOOTMODE_REBOOT);
    hal_sw_bootmode_set(HAL_SW_BOOTMODE_FORCE_USB_DLD);
    pmu_usb_config(PMU_USB_CONFIG_TYPE_DEVICE);
    hal_cmu_reset_set(HAL_CMU_MOD_GLOBAL);
}
#else
void app_dfu_key_handler(APP_KEY_STATUS *status, void *param)
{
    TRACE(1,"%s ",__func__);
    hal_sw_bootmode_clear(0xffffffff);
    hal_sw_bootmode_set(HAL_SW_BOOTMODE_FORCE_USB_DLD | HAL_SW_BOOTMODE_SKIP_FLASH_BOOT);
    pmu_usb_config(PMU_USB_CONFIG_TYPE_DEVICE);
    hal_cmu_reset_set(HAL_CMU_MOD_GLOBAL);
}
#endif

void app_ota_key_handler(APP_KEY_STATUS *status, void *param)
{
    static uint32_t time = hal_sys_timer_get();
    static uint16_t cnt = 0;

    TRACE(3,"%s %d,%d",__func__, status->code, status->event);

    if (TICKS_TO_MS(hal_sys_timer_get() - time) > 600) // 600 = (repeat key intervel)500 + (margin)100
        cnt = 0;
    else
        cnt++;

    if (cnt == PRESS_KEY_TO_ENTER_OTA_REPEAT_CNT) {
        app_otaMode_enter(NULL, NULL);
    }

    time = hal_sys_timer_get();
}
extern "C" void app_bt_key(APP_KEY_STATUS *status, void *param)
{
    TRACE(3,"%s %d,%d",__func__, status->code, status->event);
#define DEBUG_CODE_USE 0
    switch(status->event)
    {
        case APP_KEY_EVENT_CLICK:
            TRACE(0,"first blood!");
#if DEBUG_CODE_USE
            if (status->code == APP_KEY_CODE_PWR)
            {
#ifdef __INTERCONNECTION__
                // add favorite music
                // app_interconnection_handle_favorite_music_through_ccmp(1);

                // ask for ota update
                ota_update_request();
                return;
#else
                static int m = 0;
                if (m == 0) {
                    m = 1;
                    hal_iomux_set_analog_i2c();
                }
                else {
                    m = 0;
                    hal_iomux_set_uart0();
                }
#endif
            }
#endif
            break;
        case APP_KEY_EVENT_DOUBLECLICK:
            TRACE(0,"double kill");
#if DEBUG_CODE_USE
            if (status->code == APP_KEY_CODE_PWR)
            {
#ifdef __INTERCONNECTION__
                // play favorite music
                app_interconnection_handle_favorite_music_through_ccmp(2);
#else
                app_otaMode_enter(NULL, NULL);
#endif
                return;
            }
#endif
			break;
        case APP_KEY_EVENT_TRIPLECLICK:
            TRACE(0,"triple kill");
            if (status->code == APP_KEY_CODE_PWR)
            {
				if(btif_me_get_activeCons() < BT_DEVICE_NUM){
	                app_bt_accessmode_set(BTIF_BT_DEFAULT_ACCESS_MODE_PAIR);
#ifdef __INTERCONNECTION__
	                app_interceonnection_start_discoverable_adv(INTERCONNECTION_BLE_FAST_ADVERTISING_INTERVAL,
	                                                            APP_INTERCONNECTION_FAST_ADV_TIMEOUT_IN_MS);
	                return;
#endif
#ifdef GFPS_ENABLED
	                app_enter_fastpairing_mode();
#endif
					app_voice_report(APP_STATUS_INDICATION_BOTHSCAN,0);
            	}
                return;
            }
            break;
        case APP_KEY_EVENT_ULTRACLICK:
            TRACE(0,"ultra kill");
            break;
        case APP_KEY_EVENT_RAMPAGECLICK:
            TRACE(0,"rampage kill!you are crazy!");
            break;

        case APP_KEY_EVENT_UP:
            break;
    }
#ifdef __FACTORY_MODE_SUPPORT__
    if (app_status_indication_get() == APP_STATUS_INDICATION_BOTHSCAN && (status->event == APP_KEY_EVENT_DOUBLECLICK)){
        app_factorymode_languageswitch_proc();
    }else
#endif
    {
        bt_key_send(status);
    }
}

#ifdef RB_CODEC
extern bool app_rbcodec_check_hfp_active(void );
void app_switch_player_key(APP_KEY_STATUS *status, void *param)
{
    TRACE(3,"%s %d,%d",__func__, status->code, status->event);

    if(!rb_ctl_is_init_done()) {
        TRACE(0,"rb ctl not init done");
        return ;
    }

    if( app_rbcodec_check_hfp_active() ) {
        app_bt_key(status,param);
        return;
    }

    app_rbplay_audio_reset_pause_status();

    if(app_rbplay_mode_switch()) {
        app_voice_report(APP_STATUS_INDICATION_POWERON, 0);
        app_rbcodec_ctr_play_onoff(true);
    } else {
        app_rbcodec_ctr_play_onoff(false);
        app_voice_report(APP_STATUS_INDICATION_POWEROFF, 0);
    }
    return ;

}
#endif

#ifdef IS_MULTI_AI_ENABLED
void app_voice_gva_onoff_key(APP_KEY_STATUS *status, void *param)
{
    uint8_t current_ai_spec = app_ai_manager_get_current_spec();

    TRACE(2,"%s current_ai_spec %d", __func__, current_ai_spec);
    if (current_ai_spec == AI_SPEC_INIT)
    {
        app_ai_manager_enable(true, AI_SPEC_GSOUND);
    }
    else if(current_ai_spec == AI_SPEC_GSOUND)
    {
        app_ai_manager_enable(false, AI_SPEC_GSOUND);
    }
    else if(current_ai_spec == AI_SPEC_AMA)
    {
        app_ai_manager_switch_spec(AI_SPEC_GSOUND);
    }
    app_ble_refresh_adv_state(BLE_ADVERTISING_INTERVAL);
}

void app_voice_ama_onoff_key(APP_KEY_STATUS *status, void *param)
{
    uint8_t current_ai_spec = app_ai_manager_get_current_spec();

    TRACE(2,"%s current_ai_spec %d", __func__, current_ai_spec);
    if (current_ai_spec == AI_SPEC_INIT)
    {
        app_ai_manager_enable(true, AI_SPEC_AMA);
    }
    else if(current_ai_spec == AI_SPEC_AMA)
    {
        app_ai_manager_enable(false, AI_SPEC_AMA);
    }
    else if(current_ai_spec == AI_SPEC_GSOUND)
    {
        app_ai_manager_switch_spec(AI_SPEC_AMA);
    }
    app_ble_refresh_adv_state(BLE_ADVERTISING_INTERVAL);
}
#endif

#if defined(BT_USB_AUDIO_DUAL_MODE_TEST) && defined(BT_USB_AUDIO_DUAL_MODE)
extern "C" void test_btusb_switch(void);
void app_btusb_audio_dual_mode_test(APP_KEY_STATUS *status, void *param)
{
    TRACE(0,"test_btusb_switch");
    test_btusb_switch();
}
#endif

extern void switch_dualmic_status(void);

void app_switch_dualmic_key(APP_KEY_STATUS *status, void *param)
{
    switch_dualmic_status();
}

#if defined(ANC_APP)
void app_anc_key(APP_KEY_STATUS *status, void *param)
{
    app_anc_loop_switch();
}
#endif

#ifdef POWERKEY_I2C_SWITCH
extern void app_factorymode_i2c_switch(APP_KEY_STATUS *status, void *param);
#endif

#ifdef __POWERKEY_CTRL_ONOFF_ONLY__
#if defined(__APP_KEY_FN_STYLE_A__)
const APP_KEY_HANDLE  app_key_handle_cfg[] = {
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_UP},"bt function key",app_bt_key_shutdown, NULL},
    {{APP_KEY_CODE_FN1,APP_KEY_EVENT_LONGPRESS},"bt function key",app_bt_key, NULL},
    {{APP_KEY_CODE_FN1,APP_KEY_EVENT_UP},"bt function key",app_bt_key, NULL},
    {{APP_KEY_CODE_FN1,APP_KEY_EVENT_DOUBLECLICK},"bt function key",app_bt_key, NULL},
    {{APP_KEY_CODE_FN2,APP_KEY_EVENT_UP},"bt volume up key",app_bt_key, NULL},
    {{APP_KEY_CODE_FN2,APP_KEY_EVENT_LONGPRESS},"bt play backward key",app_bt_key, NULL},
    {{APP_KEY_CODE_FN3,APP_KEY_EVENT_UP},"bt volume down key",app_bt_key, NULL},
    {{APP_KEY_CODE_FN3,APP_KEY_EVENT_LONGPRESS},"bt play forward key",app_bt_key, NULL},
#ifdef SUPPORT_SIRI
    {{APP_KEY_CODE_NONE ,APP_KEY_EVENT_NONE},"none function key",app_bt_key, NULL},
#endif

};
#else //#elif defined(__APP_KEY_FN_STYLE_B__)
const APP_KEY_HANDLE  app_key_handle_cfg[] = {
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_UP},"bt function key",app_bt_key_shutdown, NULL},
    {{APP_KEY_CODE_FN1,APP_KEY_EVENT_LONGPRESS},"bt function key",app_bt_key, NULL},
    {{APP_KEY_CODE_FN1,APP_KEY_EVENT_UP},"bt function key",app_bt_key, NULL},
    {{APP_KEY_CODE_FN1,APP_KEY_EVENT_DOUBLECLICK},"bt function key",app_bt_key, NULL},
    {{APP_KEY_CODE_FN2,APP_KEY_EVENT_REPEAT},"bt volume up key",app_bt_key, NULL},
    {{APP_KEY_CODE_FN2,APP_KEY_EVENT_UP},"bt play backward key",app_bt_key, NULL},
    {{APP_KEY_CODE_FN3,APP_KEY_EVENT_REPEAT},"bt volume down key",app_bt_key, NULL},
    {{APP_KEY_CODE_FN3,APP_KEY_EVENT_UP},"bt play forward key",app_bt_key, NULL},
#ifdef SUPPORT_SIRI
    {{APP_KEY_CODE_NONE ,APP_KEY_EVENT_NONE},"none function key",app_bt_key, NULL},
#endif

};
#endif
#else
#if defined(__APP_KEY_FN_STYLE_A__)
//--
const APP_KEY_HANDLE  app_key_handle_cfg[] = {
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_LONGLONGPRESS},"bt function key",app_bt_key_shutdown, NULL},
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_LONGPRESS},"bt function key",app_bt_key, NULL},
#if defined(BT_USB_AUDIO_DUAL_MODE_TEST) && defined(BT_USB_AUDIO_DUAL_MODE)
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_CLICK},"bt function key",app_bt_key, NULL},
#ifdef RB_CODEC
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_CLICK},"bt function key",app_switch_player_key, NULL},
#else
    //{{APP_KEY_CODE_PWR,APP_KEY_EVENT_CLICK},"btusb mode switch key.",app_btusb_audio_dual_mode_test, NULL},
#endif
#endif
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_DOUBLECLICK},"bt function key",app_bt_key, NULL},
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_TRIPLECLICK},"bt function key",app_bt_key, NULL},
#if RAMPAGECLICK_TEST_MODE
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_ULTRACLICK},"bt function key",app_bt_key_enter_nosignal_mode, NULL},
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_RAMPAGECLICK},"bt function key",app_bt_key_enter_testmode, NULL},
#endif
#ifdef POWERKEY_I2C_SWITCH
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_RAMPAGECLICK},"bt i2c key",app_factorymode_i2c_switch, NULL},
#endif
    //{{APP_KEY_CODE_FN1,APP_KEY_EVENT_UP},"bt volume up key",app_bt_key, NULL},
    //{{APP_KEY_CODE_FN1,APP_KEY_EVENT_LONGPRESS},"bt play backward key",app_bt_key, NULL},
#if defined(BT_SOURCE)
    {{APP_KEY_CODE_FN1,APP_KEY_EVENT_DOUBLECLICK},"bt mode src snk key",app_bt_key, NULL},
#endif
    //{{APP_KEY_CODE_FN2,APP_KEY_EVENT_UP},"bt volume down key",app_bt_key, NULL},
    //{{APP_KEY_CODE_FN2,APP_KEY_EVENT_LONGPRESS},"bt play forward key",app_bt_key, NULL},
    //{{APP_KEY_CODE_FN15,APP_KEY_EVENT_UP},"bt volume down key",app_bt_key, NULL},
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_CLICK},"bt function key",app_bt_key, NULL},

#ifdef SUPPORT_SIRI
    {{APP_KEY_CODE_NONE ,APP_KEY_EVENT_NONE},"none function key",app_bt_key, NULL},
#endif
#if defined( __BT_ANC_KEY__)&&defined(ANC_APP)
	{{APP_KEY_CODE_PWR,APP_KEY_EVENT_CLICK},"bt anc key",app_anc_key, NULL},
#endif
#if defined(VOICE_DATAPATH) || defined(__AI_VOICE__) || defined(BISTO_ENABLED)
    {{APP_KEY_CODE_GOOGLE, APP_KEY_EVENT_FIRST_DOWN}, "google assistant key", app_ai_manager_key_event_handle, NULL},
#if defined(IS_GSOUND_BUTTION_HANDLER_WORKAROUND_ENABLED) || defined(PUSH_AND_HOLD_ENABLED) || defined(__TENCENT_VOICE__)
    {{APP_KEY_CODE_GOOGLE, APP_KEY_EVENT_UP}, "google assistant key", app_ai_manager_key_event_handle, NULL},
#endif
    {{APP_KEY_CODE_GOOGLE, APP_KEY_EVENT_UP_AFTER_LONGPRESS}, "google assistant key", app_ai_manager_key_event_handle, NULL},
    {{APP_KEY_CODE_GOOGLE, APP_KEY_EVENT_LONGPRESS}, "google assistant key", app_ai_manager_key_event_handle, NULL},
    {{APP_KEY_CODE_GOOGLE, APP_KEY_EVENT_CLICK}, "google assistant key", app_ai_manager_key_event_handle, NULL},
    {{APP_KEY_CODE_GOOGLE, APP_KEY_EVENT_DOUBLECLICK}, "google assistant key", app_ai_manager_key_event_handle, NULL},
#endif
#ifdef IS_MULTI_AI_ENABLED
    {{APP_KEY_CODE_FN13, APP_KEY_EVENT_CLICK}, "gva on-off key", app_voice_gva_onoff_key, NULL},
    {{APP_KEY_CODE_FN14, APP_KEY_EVENT_CLICK}, "ama on-off key", app_voice_ama_onoff_key, NULL},
#endif
#if defined(BT_USB_AUDIO_DUAL_MODE_TEST) && defined(BT_USB_AUDIO_DUAL_MODE)
    {{APP_KEY_CODE_FN15, APP_KEY_EVENT_CLICK}, "btusb mode switch key.", app_btusb_audio_dual_mode_test, NULL},
#endif
};
#else //#elif defined(__APP_KEY_FN_STYLE_B__)
const APP_KEY_HANDLE  app_key_handle_cfg[] = {
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_LONGLONGPRESS},"bt function key",app_bt_key_shutdown, NULL},
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_LONGPRESS},"bt function key",app_bt_key, NULL},
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_CLICK},"bt function key",app_bt_key, NULL},
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_DOUBLECLICK},"bt function key",app_bt_key, NULL},
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_TRIPLECLICK},"bt function key",app_bt_key, NULL},
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_ULTRACLICK},"bt function key",app_bt_key_enter_nosignal_mode, NULL},
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_RAMPAGECLICK},"bt function key",app_bt_key_enter_testmode, NULL},
    {{APP_KEY_CODE_FN1,APP_KEY_EVENT_REPEAT},"bt volume up key",app_bt_key, NULL},
    {{APP_KEY_CODE_FN1,APP_KEY_EVENT_UP},"bt play backward key",app_bt_key, NULL},
    {{APP_KEY_CODE_FN2,APP_KEY_EVENT_REPEAT},"bt volume down key",app_bt_key, NULL},
    {{APP_KEY_CODE_FN2,APP_KEY_EVENT_UP},"bt play forward key",app_bt_key, NULL},
#ifdef SUPPORT_SIRI
    {{APP_KEY_CODE_NONE ,APP_KEY_EVENT_NONE},"none function key",app_bt_key, NULL},
#endif

#if defined(VOICE_DATAPATH) || defined(__AI_VOICE__) || defined(BISTO_ENABLED)
    {{APP_KEY_CODE_GOOGLE, APP_KEY_EVENT_FIRST_DOWN}, "google assistant key", app_ai_manager_key_event_handle, NULL},
#if defined(IS_GSOUND_BUTTION_HANDLER_WORKAROUND_ENABLED) || defined(PUSH_AND_HOLD_ENABLED)
    {{APP_KEY_CODE_GOOGLE, APP_KEY_EVENT_UP}, "google assistant key", app_ai_manager_key_event_handle, NULL},
#endif
    {{APP_KEY_CODE_GOOGLE, APP_KEY_EVENT_UP_AFTER_LONGPRESS}, "google assistant key", app_ai_manager_key_event_handle, NULL},
    {{APP_KEY_CODE_GOOGLE, APP_KEY_EVENT_LONGPRESS}, "google assistant key", app_ai_manager_key_event_handle, NULL},
    {{APP_KEY_CODE_GOOGLE, APP_KEY_EVENT_CLICK}, "google assistant key", app_ai_manager_key_event_handle, NULL},
    {{APP_KEY_CODE_GOOGLE, APP_KEY_EVENT_DOUBLECLICK}, "google assistant key", app_ai_manager_key_event_handle, NULL},
#endif
};
#endif
#endif

#if defined (__AI_VOICE__)
void app_ibrt_ui_test_wakeup_ai_voice(void)
{
    APP_KEY_STATUS keyEvent;
    keyEvent.code = APP_KEY_CODE_GOOGLE;
    keyEvent.event = HAL_KEY_EVENT_FIRST_DOWN;
    app_ai_manager_key_event_handle(&keyEvent, NULL);

    keyEvent.code = APP_KEY_CODE_GOOGLE;
    keyEvent.event = HAL_KEY_EVENT_UP;
    app_ai_manager_key_event_handle(&keyEvent, NULL);

    keyEvent.code = APP_KEY_CODE_GOOGLE;
    keyEvent.event = HAL_KEY_EVENT_CLICK;
    app_ai_manager_key_event_handle(&keyEvent, NULL);
}
#endif

void app_key_init(void)
{
#if defined(AOB_UX_ENABLED)&&defined(__IAG_BLE_INCLUDE__)&&defined(BLE_V2)
    aob_ux_test_key_init();
#elif defined(IBRT) && defined(IBRT_UI_V1)
    app_ibrt_ui_test_key_init();
#elif defined(IBRT) && defined(IBRT_CORE_V2_ENABLE)
    app_tws_ibrt_raw_ui_test_key_init();
#else
    uint8_t i = 0;
    TRACE(1,"%s",__func__);

    app_key_handle_clear();
    for (i=0; i<(sizeof(app_key_handle_cfg)/sizeof(APP_KEY_HANDLE)); i++){
        app_key_handle_registration(&app_key_handle_cfg[i]);
    }
#endif
}
void app_key_init_on_charging(void)
{
    uint8_t i = 0;
    const APP_KEY_HANDLE  key_cfg[] = {
        {{APP_KEY_CODE_PWR,APP_KEY_EVENT_REPEAT},"ota function key",app_ota_key_handler, NULL},
        {{APP_KEY_CODE_PWR,APP_KEY_EVENT_CLICK},"bt function key",app_dfu_key_handler, NULL},
#ifdef __USB_COMM__
        {{APP_KEY_CODE_PWR,APP_KEY_EVENT_LONGPRESS},"usb cdc key",app_usb_cdc_comm_key_handler, NULL},
#endif
    };

    TRACE(1,"%s",__func__);
    for (i=0; i<(sizeof(key_cfg)/sizeof(APP_KEY_HANDLE)); i++){
        app_key_handle_registration(&key_cfg[i]);
    }
}

extern bt_status_t LinkDisconnectDirectly(bool PowerOffFlag);

bool app_is_power_off_in_progress(void)
{
    return app_poweroff_flag?TRUE:FALSE;
}

#ifdef APP_SYSFREQ_BOOST
#define HOLD_FREQUENCY_TIMER_DELAY_TIME_MS  (10000)
static enum APP_SYSFREQ_USER_T hold_frequency_user;
static enum APP_SYSFREQ_FREQ_T hold_frequency;
static void app_bt_streaming_hold_frequency_run_timehandler(void const *param);
osTimerDef (BT_STREAM_FREQUENCY_HOLD_TIMER_ID, app_bt_streaming_hold_frequency_run_timehandler);
osTimerId bt_stream_frequency_hold_timer_id = NULL;
static void app_bt_streaming_hold_frequency_run_timehandler(void const *param)
{
    TRACE(1, "release hold frequency");

	app_sysfreq_req(hold_frequency_user, APP_SYSFREQ_32K);
}

void app_bt_streaming_hold_frequency_run_timer_create(void)
{
    if(bt_stream_frequency_hold_timer_id == NULL){
        bt_stream_frequency_hold_timer_id = osTimerCreate(osTimer(BT_STREAM_FREQUENCY_HOLD_TIMER_ID), osTimerOnce, NULL);
    }
}

void app_bt_streaming_hold_frequency_run_timer_start(uint8_t ratio)
{
    if(bt_stream_frequency_hold_timer_id != NULL){
        hold_frequency_user = APP_SYSFREQ_USER_BOOST;
        uint8_t freq = (uint8_t)APP_SYSFREQ_104M;
        freq = ratio+freq;
        hold_frequency = (enum APP_SYSFREQ_FREQ_T)freq;
        TRACE(1, "hold frequency to %d", freq);
        app_sysfreq_req(hold_frequency_user, hold_frequency);

        osTimerStart(bt_stream_frequency_hold_timer_id, HOLD_FREQUENCY_TIMER_DELAY_TIME_MS);//1s
    }else{
        TRACE(1,"%s bt_stream_frequency_hold_timer_id == NULL",__func__);
    }
}

void app_bt_streaming_hold_frequency_run_timer_stop(void)
{
    if(bt_stream_frequency_hold_timer_id != NULL){
        osTimerStop(bt_stream_frequency_hold_timer_id);
    }
}
#endif


#if GFPS_ENABLED
#define APP_GFPS_BATTERY_TIMEROUT_VALUE             (10000)
static void app_gfps_battery_show_timeout_timer_cb(void const *n);
osTimerDef (GFPS_BATTERY_SHOW_TIMEOUT_TIMER, app_gfps_battery_show_timeout_timer_cb);
static osTimerId app_gfps_battery_show_timer_id = NULL;
#include "app_gfps.h"
static void app_gfps_battery_show_timeout_timer_cb(void const *n)
{
    TRACE(1,"%s", __func__);
    app_gfps_set_battery_datatype(HIDE_UI_INDICATION);
}

void app_gfps_battery_show_timer_start()
{
    if (app_gfps_battery_show_timer_id == NULL)
        app_gfps_battery_show_timer_id = osTimerCreate(osTimer(GFPS_BATTERY_SHOW_TIMEOUT_TIMER), osTimerOnce, NULL);
    osTimerStart(app_gfps_battery_show_timer_id, APP_GFPS_BATTERY_TIMEROUT_VALUE);
}

void app_gfps_battery_show_timer_stop()
{
    if (app_gfps_battery_show_timer_id)
        osTimerStop(app_gfps_battery_show_timer_id);
}

void app_voice_gfps_find(void)
{
    app_voice_report(APP_STATUS_INDICATION_TILE_FIND, 0);
}
#endif
int app_deinit(int deinit_case)
{
    int nRet = 0;
    TRACE(2,"%s case:%d",__func__, deinit_case);

#ifdef __PC_CMD_UART__
    app_cmd_close();
#endif
#if (defined(BTUSB_AUDIO_MODE) || defined(BT_USB_AUDIO_DUAL_MODE))
    if(app_usbaudio_mode_on())
    {
        return 0;
    }
#endif
    if (!deinit_case){
#ifdef MEDIA_PLAYER_SUPPORT
        app_prompt_flush_pending_prompts();
#endif
#if defined(ANC_APP)
        app_anc_deinit();
#endif
#if defined(ANC_ASSIST_ENABLED)
        app_anc_assist_deinit();
#endif
        app_poweroff_flag = 1;
#if defined(APP_LINEIN_A2DP_SOURCE)
        app_audio_sendrequest(APP_A2DP_SOURCE_LINEIN_AUDIO, (uint8_t)APP_BT_SETTING_CLOSE,0);
#endif
#if defined(APP_I2S_A2DP_SOURCE)
        app_audio_sendrequest(APP_A2DP_SOURCE_I2S_AUDIO, (uint8_t)APP_BT_SETTING_CLOSE,0);
#endif
        app_status_indication_filter_set(APP_STATUS_INDICATION_BOTHSCAN);
        app_audio_sendrequest(APP_BT_STREAM_INVALID, (uint8_t)APP_BT_SETTING_CLOSEALL, 0);
        osDelay(500);
        LinkDisconnectDirectly(true);
        osDelay(500);
        app_status_indication_set(APP_STATUS_INDICATION_POWEROFF);
#ifdef MEDIA_PLAYER_SUPPORT
        app_voice_report(APP_STATUS_INDICATION_POWEROFF, 0);
#endif
        osDelay(1000);
        af_close();
#ifdef __THIRDPARTY
        app_thirdparty_specific_lib_event_handle(THIRDPARTY_FUNC_NO1, THIRDPARTY_DEINIT, AI_SPEC_INIT);
#endif
#if defined(__XSPACE_UI__)
        xspace_interface_write_data_to_flash_befor_shutdown();
#endif

#if defined(__XSPACE_FLASH_ACCESS__)
        hw_rst_use_bat_data_flag_write_flash(true);
#endif
#if FPGA==0
        nv_record_flash_flush();
        norflash_api_flush_all();
#if defined(DUMP_LOG_ENABLE)
        log_dump_flush_all();
#endif
#endif
    }

    return nRet;
}

#ifdef APP_TEST_MODE
extern void app_test_init(void);
int app_init(void)
{
    int nRet = 0;
    //uint8_t pwron_case = APP_POWERON_CASE_INVALID;
    TRACE(1,"%s",__func__);
    app_poweroff_flag = 0;

#ifdef APP_TRACE_RX_ENABLE
    app_trace_rx_open();
#endif

    app_sysfreq_req(APP_SYSFREQ_USER_APP_INIT, APP_SYSFREQ_52M);
    list_init();
    af_open();//承上启下，向上提供接口，向下访问硬件
    app_os_init();
    app_pwl_open();
    app_audio_open();
    app_audio_manager_open();
    app_overlay_open();
    if (app_key_open(true))
    {
        nRet = -1;
        goto exit;
    }

    app_test_init();
exit:
    app_sysfreq_req(APP_SYSFREQ_USER_APP_INIT, APP_SYSFREQ_32K);
    return nRet;
}
#else /* !defined(APP_TEST_MODE) */

int app_bt_connect2tester_init(void)
{
    btif_device_record_t rec;
    bt_bdaddr_t tester_addr;
    uint8_t i;
    bool find_tester = false;
    struct nvrecord_env_t *nvrecord_env;
    btdevice_profile *btdevice_plf_p;
    nv_record_env_get(&nvrecord_env);

    if (nvrecord_env->factory_tester_status.status != NVRAM_ENV_FACTORY_TESTER_STATUS_DEFAULT)
        return 0;

    if (!nvrec_dev_get_dongleaddr(&tester_addr)){
        nv_record_open(section_usrdata_ddbrecord);
        for (i = 0; nv_record_enum_dev_records(i, &rec) == BT_STS_SUCCESS; i++) {
            if (!memcmp(rec.bdAddr.address, tester_addr.address, BTIF_BD_ADDR_SIZE)){
                find_tester = true;
            }
        }
        if(i==0 && !find_tester){
            memset(&rec, 0, sizeof(btif_device_record_t));
            memcpy(rec.bdAddr.address, tester_addr.address, BTIF_BD_ADDR_SIZE);
            nv_record_add(section_usrdata_ddbrecord, &rec);
            btdevice_plf_p = (btdevice_profile *)app_bt_profile_active_store_ptr_get(rec.bdAddr.address);
            nv_record_btdevicerecord_set_hfp_profile_active_state(btdevice_plf_p, true);
            nv_record_btdevicerecord_set_a2dp_profile_active_state(btdevice_plf_p, true);
        }
        if (find_tester && i>2){
            nv_record_ddbrec_delete(&tester_addr);
            nvrecord_env->factory_tester_status.status = NVRAM_ENV_FACTORY_TESTER_STATUS_TEST_PASS;
            nv_record_env_set(nvrecord_env);
        }
    }

    return 0;
}

int app_nvrecord_rebuild(void)
{
    struct nvrecord_env_t *nvrecord_env;
    nv_record_env_get(&nvrecord_env);

    nv_record_sector_clear();
    nv_record_env_init();
    nv_record_update_factory_tester_status(NVRAM_ENV_FACTORY_TESTER_STATUS_TEST_PASS);
    nv_record_env_set(nvrecord_env);
    nv_record_flash_flush();

    return 0;
}

#if (defined(BTUSB_AUDIO_MODE) || defined(BT_USB_AUDIO_DUAL_MODE))
#include "app_audio.h"
#include "usb_audio_frm_defs.h"
#include "usb_audio_app.h"

static bool app_usbaudio_mode = false;

extern "C" void btusbaudio_entry(void);
void app_usbaudio_entry(void)
{
    btusbaudio_entry();
    app_usbaudio_mode = true ;
}

bool app_usbaudio_mode_on(void)
{
    return app_usbaudio_mode;
}

void app_usb_key(APP_KEY_STATUS *status, void *param)
{
    TRACE(3,"%s %d,%d",__func__, status->code, status->event);

}

#if GFPS_ENABLED
#define APP_GFPS_BATTERY_TIMEROUT_VALUE             (10000)
static void app_gfps_battery_show_timeout_timer_cb(void const *n);
osTimerDef (GFPS_BATTERY_SHOW_TIMEOUT_TIMER, app_gfps_battery_show_timeout_timer_cb);
static osTimerId app_gfps_battery_show_timer_id = NULL;
#include "app_gfps.h"
static void app_gfps_battery_show_timeout_timer_cb(void const *n)
{
    TRACE(1,"%s", __func__);
    app_gfps_set_battery_datatype(HIDE_UI_INDICATION);
}

void app_gfps_battery_show_timer_start()
{
    if (app_gfps_battery_show_timer_id == NULL)
        app_gfps_battery_show_timer_id = osTimerCreate(osTimer(GFPS_BATTERY_SHOW_TIMEOUT_TIMER), osTimerOnce, NULL);
    osTimerStart(app_gfps_battery_show_timer_id, APP_GFPS_BATTERY_TIMEROUT_VALUE);
}

void app_gfps_battery_show_timer_stop()
{
    if (app_gfps_battery_show_timer_id)
        osTimerStop(app_gfps_battery_show_timer_id);
}

void app_gfps_battery_hide_show(void)
{
    TRACE(1,"%s", __func__);
    app_gfps_set_battery_datatype(HIDE_UI_INDICATION);
}

void app_voice_gfps_find(void)
{
    app_voice_report(APP_STATUS_INDICATION_TILE_FIND, 0);
}
#endif


const APP_KEY_HANDLE  app_usb_handle_cfg[] = {
    {{APP_KEY_CODE_FN1,APP_KEY_EVENT_UP},"USB HID FN1 UP key",app_usb_key, NULL},
    {{APP_KEY_CODE_FN2,APP_KEY_EVENT_UP},"USB HID FN2 UP key",app_usb_key, NULL},
    {{APP_KEY_CODE_PWR,APP_KEY_EVENT_UP},"USB HID PWR UP key",app_usb_key, NULL},
};

void app_usb_key_init(void)
{
    uint8_t i = 0;
    TRACE(1,"%s",__func__);
    for (i=0; i<(sizeof(app_usb_handle_cfg)/sizeof(APP_KEY_HANDLE)); i++){
        app_key_handle_registration(&app_usb_handle_cfg[i]);
    }
}
#endif /* (defined(BTUSB_AUDIO_MODE) || defined(BT_USB_AUDIO_DUAL_MODE)) */

#ifdef OS_THREAD_TIMING_STATISTICS_ENABLE
#ifdef DEBUG
uint8_t isSwitchToUart = false;
#endif
extern "C" void rtx_show_all_threads_usage(void);
static void cpu_usage_timer_handler(void const *param);
osTimerDef(cpu_usage_timer, cpu_usage_timer_handler);
static osTimerId cpu_usage_timer_id = NULL;
static void cpu_usage_timer_handler(void const *param)
{
#ifdef DEBUG
    if (isSwitchToUart)
    {
        hal_cmu_jtag_clock_disable();
#if (DEBUG_PORT == 1)
        hal_iomux_set_uart0();
#elif (DEBUG_PORT == 2)
        hal_iomux_set_uart1();
#endif
        isSwitchToUart = false;
    }
#endif
    rtx_show_all_threads_usage();
}
#endif

int btdrv_tportopen(void);

#ifdef __XSPACE_UI__
bool ibrt_stack_init_finish = false;
bool app_get_ibrt_stack_init_result(void)
{
    TRACE(2, "%s %d", __func__, ibrt_stack_init_finish);
    return ibrt_stack_init_finish;
}
#endif

void app_ibrt_start_power_on_tws_pairing(void);
void app_ibrt_start_power_on_freeman_pairing(void);

void app_ibrt_init(void)
{
    if (app_bt_sink_is_enabled())
    {
        app_bt_global_handle_init();
#if defined(IBRT)
        ibrt_config_t config;
        app_tws_ibrt_init();
        app_tws_ibrt_init_default_tws_bandwidth_config(
            IBRT_TWS_LINK_DEFAULT_DURATION,
            IBRT_UI_DEFAULT_POLL_INTERVAL,
            IBRT_UI_DEFAULT_POLL_INTERVAL_IN_SCO);
#if defined(IBRT_UI_V1)
        app_ibrt_ui_init();
        app_ibrt_ui_test_init();
        app_ibrt_if_config_load(&config);
        app_ibrt_customif_ui_start();
#ifdef IBRT_SEARCH_UI
        app_tws_ibrt_start(&config, true);
        app_ibrt_search_ui_init(false,IBRT_NONE_EVENT);
#else
        app_tws_ibrt_start(&config, false);
#endif
#endif
#if defined(IBRT_CORE_V2_ENABLE)
#if !defined(__XSPACE_UI__)
        app_ibrt_raw_ui_test_config_load(&config);
#endif
        app_ibrt_if_register_ibrt_cbs();
        app_ibrt_tws_ext_cmd_init();
        app_ibrt_if_init_newly_paired_mobile_callback();
        app_custom_ui_ctx_init();
        app_ibrt_conn_init();
#ifdef SEARCH_UI_COMPATIBLE_UI_V2
    app_ibrt_search_ui_load_config(&config);
    app_tws_ibrt_start(&config,true);
#else
#if defined(__XSPACE_UI__)
        xspace_interface_ibrt_nvrecord_config_load(&config);
#else
        app_ibrt_raw_ui_test_config_load(&config);
#endif
    app_tws_ibrt_start(&config,false);
#endif

#endif

#if defined(IBRT_UI_V2)
        ibrt_mgr_init();
        app_ibrt_customif_ui_start();
#ifdef SEARCH_UI_COMPATIBLE_UI_V2
        app_ibrt_search_ui_init(false,IBRT_MGR_EV_NONE);
#endif
#endif

        __attribute__((unused)) bool isAllowFollowingHandler = true;

        isAllowFollowingHandler = app_ibrt_if_post_custom_reboot_handler();

        if (isAllowFollowingHandler)
        {
#ifdef POWER_ON_OPEN_BOX_ENABLED
            app_ibrt_if_event_entry(IBRT_MGR_EV_CASE_OPEN);
#else
    #ifdef POWER_ON_ENTER_TWS_PAIRING_ENABLED
        #if defined(IBRT_UI_V1)
            app_ibrt_ui_event_entry(IBRT_TWS_PAIRING_EVENT);
        #else
            app_ibrt_start_power_on_tws_pairing();
        #endif
    #elif defined(POWER_ON_ENTER_FREEMAN_PAIRING_ENABLED)
        #if defined(IBRT_UI_V1)
            app_ibrt_ui_event_entry(IBRT_FREEMAN_PAIRING_EVENT);
        #else
            app_ibrt_start_power_on_freeman_pairing();
        #endif
    #endif
#endif
        }
        
        app_ibrt_if_register_retrigger_prompt_id(AUDIO_ID_BT_MUTE);
#endif
    ibrt_stack_init_finish = true;
    }
}

#ifdef GFPS_ENABLED
static void app_tell_battery_info_handler(uint8_t *batteryValueCount,
                                          uint8_t *batteryValue)
{
     /*
    GFPS_BATTERY_STATUS_E status = BATTERY_NOT_CHARGING;
    if (app_battery_is_charging())
    {
        status = BATTERY_CHARGING;
    }
    else
    {
        status = BATTERY_NOT_CHARGING;
    }
*/
    // TODO: add the charger case's battery level
    /*
#ifdef IBRT
    if (app_tws_ibrt_tws_link_connected())
    {
        *batteryValueCount = 2;
    }
    else
    {
        *batteryValueCount = 1;
    }
#else
    *batteryValueCount = 1;
#endif

    TRACE(2,"%s,*batteryValueCount is %d",__func__,*batteryValueCount);
    if (1 == *batteryValueCount)
    {
        batteryValue[0] = ((app_battery_current_level()+1) * 10) | (status << 7);
    }
    else
    {
        batteryValue[0] = ((app_battery_current_level()+1) * 10) | (status << 7);
        batteryValue[1] = ((app_battery_current_level()+1) * 10) | (status << 7);

    }
    */
}
#endif

extern uint32_t __ota_app_boot_info_start[];
extern uint32_t __combo_start[];
extern uint32_t __prompt_start[];
extern uint32_t __hotword_model_start[];
extern uint32_t __coredump_section_start[];
extern uint32_t __ota_upgrade_log_start[];
extern uint32_t __log_dump_start[];
extern uint32_t __crash_dump_start[];
extern uint32_t __custom_parameter_start[];
extern uint32_t __lhdc_license_start[];
extern uint32_t __userdata_start[];
extern uint32_t __aud_start[];
extern uint32_t __reserved_start[];
extern uint32_t __factory_start[];
#if defined(__XSPACE_UI__)
extern uint32_t __xspace_reserved_start[];
extern uint32_t __xspace_parameter_start[];
extern uint32_t __xspace_backup_data_start[];
extern uint32_t __xspace_meditation_section_start[];
#endif
#if defined(__XSPACE_UI__)
//#define __ENTER_FREEMAN_PAIRING__
/**add by lmzhang,20210613*/
uint8_t sys_abnormal_reboot_flag = 0;
extern "C" bool get_sys_abnormal_reboot_flag(void)
{
    return sys_abnormal_reboot_flag;
}

extern "C" void clear_sys_abnormal_reboot_flag(void)
{
    sys_abnormal_reboot_flag = 0;
}
/**add by lmzhang,20210613*/
#endif
#if defined(__FORCE_OTA_BIN_UPDATE__)
extern "C" int apps_ota_bin_ver_check(void);
#endif
//czh add 20201221
#if defined(__XSPACE_RAM_ACCESS__) && (defined(__TOUCH_GH61X__) || defined(__TOUCH_GH621X__))
extern "C" uint32_t xra_spp_mode_read_noInit_data(void);
#endif
#if defined(__XSPACE_INEAR_DETECT_MANAGER__) && defined(PO_QUICKLY_OUT_BOX_WITHOUT_WEAR_FIX)
#include "xspace_inear_detect_manager.h"
extern void xspace_ui_inear_detect_manager_status_change(inear_detect_manager_status_e inear_status);
extern bool xspace_ui_inear_detect_need_execute(inear_detect_manager_status_e wear_status);
#endif
#ifdef __XSPACE_UI__
static uint8_t app_status_indication_init(void)
{
    struct APP_PWL_CFG_T cfg;
    memset(&cfg, 0, sizeof(struct APP_PWL_CFG_T));
    app_pwl_open();
    return 0;
}
#endif
extern uint32_t g_sw_bootmode;
extern uint8_t g_hw_bootmode;

int app_init(void)
{
    int nRet = 0;
    struct nvrecord_env_t *nvrecord_env;
#if defined(IGNORE_POWER_ON_KEY_DURING_BOOT_UP)
    bool need_check_key = false;
#else
#if defined(__XSPACE_UI__)
    bool need_check_key = false;
#else
    bool need_check_key = true;
#endif
#endif
    uint8_t pwron_case = APP_POWERON_CASE_INVALID;
#ifdef BT_USB_AUDIO_DUAL_MODE
    uint8_t usb_plugin = 0;
#endif
#ifdef IBRT_SEARCH_UI
    bool is_charging_poweron=false;
#endif
    TRACE(0,"please check all sections sizes and heads is correct ........");
    TRACE(2,"__ota_app_boot_info_start: %p length = 0x%02x", __ota_app_boot_info_start, OTA_APP_BOOT_INFO_SIZE);
    TRACE(2,"__combo_start: %p length = 0x%02x", __combo_start, COMBO_SECTION_SIZE);
    TRACE(2,"__prompt_start: %p length = 0x%02x", __prompt_start, PROMPT_SECTION_SIZE);
    TRACE(2,"__hotword_model_start: %p length = 0x%02x", __hotword_model_start, HOTWORD_SECTION_SIZE);
    TRACE(2,"__coredump_section_start: %p length: 0x%x", __coredump_section_start, CORE_DUMP_SECTION_SIZE);
    TRACE(2,"__ota_upgrade_log_start: %p length: 0x%x", __ota_upgrade_log_start, OTA_UPGRADE_LOG_SIZE);
    TRACE(2,"__log_dump_start: %p length: 0x%x", __log_dump_start, LOG_DUMP_SECTION_SIZE);
    TRACE(2,"__crash_dump_start: %p length: 0x%x", __crash_dump_start, CRASH_DUMP_SECTION_SIZE);
    TRACE(2,"__custom_parameter_start: %p length: 0x%x", __custom_parameter_start, CUSTOM_PARAMETER_SECTION_SIZE);
    TRACE(2,"__lhdc_license_start: %p length: 0x%x", __lhdc_license_start, LHDC_LICENSE_SECTION_SIZE);
    TRACE(2,"__userdata_start: %p length: 0x%x", __userdata_start, USERDATA_SECTION_SIZE*2);
    TRACE(2,"__aud_start: %p length: 0x%x", __aud_start, AUD_SECTION_SIZE);
    TRACE(2,"__reserved_start: %p length = 0x%02x", __reserved_start, RESERVED_SECTION_SIZE);
    TRACE(2,"__factory_start: %p length: 0x%x", __factory_start, FACTORY_SECTION_SIZE);
#if defined(__XSPACE_UI__)
    TRACE(2, "__xspace_reserved_start: %p length: 0x%x", __xspace_reserved_start, XSPACE_RESERVED_SECTION_SIZE);
    TRACE(2, "__xspace_parameter_start: %p length: 0x%x", __xspace_parameter_start, XSPACE_PARAMETER_SECTION_SIZE);
    TRACE(2, "__xspace_backup_data: %p length: 0x%x", __xspace_backup_data_start, XSPACE_PARAMETER_SECTION_SIZE);

    TRACE(7, "\r\napp_init,SW_VER = V%d.%d.%d.%d, HW_VER = V%d.%d, I am %s.\n", xui_dev_sw_version[0], xui_dev_sw_version[1], xui_dev_sw_version[2],
          xui_dev_sw_version[3], xui_dev_hw_version[0], xui_dev_hw_version[1], (LEFT_SIDE == app_tws_get_earside()) ? "left" : "right");
#else
    TRACE(0, "app_init\n");
#endif
    TRACE(0, "****************************");
    TRACE(1, "sw_bootmode:0x%x", g_sw_bootmode);
    TRACE(1, "hw_bootmode:0x%x", g_hw_bootmode);
    TRACE(0, "****************************\n");
#if defined(__XSPACE_UI__)
    app_sysfreq_req(APP_SYSFREQ_USER_APP_INIT, APP_SYSFREQ_104M);
#else
    app_sysfreq_req(APP_SYSFREQ_USER_APP_INIT, APP_SYSFREQ_52M);
#endif

#ifdef APP_TRACE_RX_ENABLE
    app_trace_rx_open();
    app_bt_cmd_init();
#endif
    app_bt_add_string_test_table();

#ifdef __RPC_ENABLE__
extern int rpc_service_setup(void);
    rpc_service_setup();
#endif

#if defined(ESHELL_ON)
    eshell_open();
#endif

#ifdef VOICE_DEV
    voice_dev_init();
#endif

#ifdef IBRT
    // init tws interface
    app_ibrt_middleware_init();
#endif // #ifdef IBRT

#ifdef VOICE_DETECTOR_EN
    app_voice_detector_init();
#endif // #ifdef VOICE_DETECTOR_EN

    nv_record_init();
    factory_section_init();
#if defined(__XSPACE_FLASH_ACCESS__)
    xspace_section_init();
#endif

    nv_record_env_init();
    nvrec_dev_data_open();
    factory_section_open();
    nv_record_env_get(&nvrecord_env);

#if defined(__XSPACE_UI__)
    if (nvrecord_env->ibrt_mode.mode != IBRT_UNKNOW) {
        memcpy((void *)ble_addr, (void *)nvrecord_env->ibrt_mode.record.bdAddr.address, 6);
    }
#endif
#ifdef AUDIO_OUTPUT_DC_AUTO_CALIB
    /* DAC DC and Dig gain calibration
     * This subroutine will try to load DC and gain calibration
     * parameters from user data section located at NV record;
     * If failed to loading parameters, It will open AF and start
     * to calibration DC and gain;
     * After accomplished new calibrtion parameters, it will try
     * to save these data into user data section at NV record if
     * CODEC_DAC_DC_NV_DATA defined;
     */
    codec_dac_dc_auto_load(true, false, false);
#endif

    app_sysfreq_req(APP_SYSFREQ_USER_APP_INIT, APP_SYSFREQ_104M);
#if defined(MCU_HIGH_PERFORMANCE_MODE)
    TRACE(1,"sys freq calc : %d\n", hal_sys_timer_calc_cpu_freq(5, 0));
#endif
    list_init();
    nRet = app_os_init();
    if (nRet) {
        goto exit;
    }
#ifdef OS_THREAD_TIMING_STATISTICS_ENABLE
    cpu_usage_timer_id = osTimerCreate(osTimer(cpu_usage_timer), osTimerPeriodic, NULL);
    if (cpu_usage_timer_id != NULL) {
        osTimerStart(cpu_usage_timer_id, OS_THREAD_TIMING_STATISTICS_PEROID_MS);
    }
#endif

#ifdef __XSPACE_UI__
    app_status_indication_init();
#endif

#if !defined(__XSPACE_UI__)
#ifdef BTADDR_FOR_DEBUG
    gen_bt_addr_for_debug();
#endif
#else
    xspace_interface_init();
#endif

#if defined(__USE_SW_I2C__) || defined(__USE_HW_I2C__)
    xspace_i2c_init();
#endif

#ifdef FORCE_SIGNALINGMODE
    hal_sw_bootmode_clear(HAL_SW_BOOTMODE_TEST_NOSIGNALINGMODE);
    hal_sw_bootmode_set(HAL_SW_BOOTMODE_TEST_MODE | HAL_SW_BOOTMODE_TEST_SIGNALINGMODE);
#elif defined FORCE_NOSIGNALINGMODE
    hal_sw_bootmode_clear(HAL_SW_BOOTMODE_TEST_SIGNALINGMODE);
    hal_sw_bootmode_set(HAL_SW_BOOTMODE_TEST_MODE | HAL_SW_BOOTMODE_TEST_NOSIGNALINGMODE);
#endif
#if defined(__ENTER_FREEMAN_PAIRING__)
    hal_sw_bootmode_set(HAL_SW_BOOTMODE_REBOOT_ENTER_FREEMAN);
#endif

    if (hal_sw_bootmode_get() & HAL_SW_BOOTMODE_REBOOT_FROM_CRASH){
        hal_sw_bootmode_clear(HAL_SW_BOOTMODE_REBOOT_FROM_CRASH);
        TRACE(0,"Crash happened!!!");
    #ifdef VOICE_DATAPATH
        gsound_dump_set_flag(true);
    #endif
    #if defined(__XSPACE_UI__)
        sys_abnormal_reboot_flag = 1;
#endif
    }

    if (hal_sw_bootmode_get() & HAL_SW_BOOTMODE_REBOOT){
        hal_sw_bootmode_clear(HAL_SW_BOOTMODE_REBOOT);
#if defined(__XSPACE_UI__)
        pwron_case = APP_POWERON_CASE_NORMAL;
#else
        pwron_case = APP_POWERON_CASE_REBOOT;
#endif
        need_check_key = false;
        TRACE(0,"Initiative REBOOT happens!!!");
#ifdef USER_REBOOT_PLAY_MUSIC_AUTO
        if (hal_sw_bootmode_get() & HAL_SW_BOOTMODE_LOCAL_PLAYER) {
            hal_sw_bootmode_clear(HAL_SW_BOOTMODE_LOCAL_PLAYER);
            a2dp_need_to_play = true;
            TRACE(0, "a2dp_need_to_play = true");
        }
#endif
    }

    if (hal_sw_bootmode_get() & HAL_SW_BOOTMODE_TEST_MODE){
        hal_sw_bootmode_clear(HAL_SW_BOOTMODE_TEST_MODE);
        pwron_case = APP_POWERON_CASE_TEST;
        need_check_key = false;
        TRACE(0,"To enter test mode!!!");
    }
    #if defined(__XSPACE_UI__)
    if (hal_sw_bootmode_get() & HAL_SW_BOOTMODE_REBOOT_ENTER_PAIRING) {
        hal_sw_bootmode_clear(HAL_SW_BOOTMODE_REBOOT_ENTER_PAIRING);
        need_check_key = false;
        TRACE(0, " Force Eenter TWS Pairing!!!");
        xspace_ui_set_system_status(XUI_SYS_FORCE_TWS_PARING);
#if defined(__XSPACE_UI__)
        app_start_10_second_timer(APP_TWSPAIRING_POWEROFF_TIMER_ID);
#endif
    } else if (hal_sw_bootmode_get() & HAL_SW_BOOTMODE_REBOOT_ENTER_FREEMAN) {
        hal_sw_bootmode_clear(HAL_SW_BOOTMODE_REBOOT_ENTER_FREEMAN);
        need_check_key = false;
        xspace_interface_freeman_config();
        TRACE(0, " Force Eenter Freeman Pairing!!!");
        xspace_ui_set_system_status(XUI_SYS_FORCE_FREEMAN_PARING);
    }
#endif

#ifdef APP_UART_MODULE
    app_uart_init();
#endif
#if defined(__DEV_THREAD__)
    dev_thread_init();
    osDelay(30);
#endif
#if defined(__XSPACE_INEAR_DETECT_MANAGER__) && defined(PO_QUICKLY_OUT_BOX_WITHOUT_WEAR_FIX)
    xspace_inear_detect_manager_register_ui_cb(xspace_ui_inear_detect_manager_status_change);
    xspace_inear_detect_manager_register_ui_need_execute_cb(xspace_ui_inear_detect_need_execute);
    xspace_inear_detect_manager_init();
#endif
#if defined(__XSPACE_BATTERY_MANAGER__)
    xspace_ui_battery_init();
#else
    nRet = app_battery_open();
    TRACE(1,"BATTERY %d",nRet);
    if (pwron_case != APP_POWERON_CASE_TEST){
        #ifdef USER_REBOOT_PLAY_MUSIC_AUTO
        TRACE(0, "hal_sw_bootmode_clear HAL_SW_BOOTMODE_LOCAL_PLAYER!!!!!!");
        hal_sw_bootmode_clear(HAL_SW_BOOTMODE_LOCAL_PLAYER);
#endif
        switch (nRet) {
            case APP_BATTERY_OPEN_MODE_NORMAL:
                nRet = 0;
                break;
            case APP_BATTERY_OPEN_MODE_CHARGING:
                app_status_indication_set(APP_STATUS_INDICATION_CHARGING);
                TRACE(0,"CHARGING!");
                app_battery_start();

                app_key_open(false);
                app_key_init_on_charging();
                nRet = 0;
#if defined(BT_USB_AUDIO_DUAL_MODE)
                usb_plugin = 1;
#elif defined(BTUSB_AUDIO_MODE)
                goto exit;
#endif
                break;
            case APP_BATTERY_OPEN_MODE_CHARGING_PWRON:
                TRACE(0,"CHARGING PWRON!");
#ifdef IBRT_SEARCH_UI
                is_charging_poweron=true;
#endif
                need_check_key = false;
                nRet = 0;
                break;
            case APP_BATTERY_OPEN_MODE_INVALID:
            default:
                nRet = -1;
                goto exit;
                break;
        }
    }
#endif

#if defined(__OPEN_BES_KEY_HANDLE__)

    if (app_key_open(need_check_key)) {
        TRACE(0, "PWR KEY DITHER!");
        pwron_case = APP_POWERON_CASE_BOTHSCAN;
    }
    app_poweron_key_init();
#endif

    hal_sw_bootmode_set(HAL_SW_BOOTMODE_REBOOT);

#if defined(_AUTO_TEST_)
    AUTO_TEST_SEND("Power on.");
#endif
#if defined(__FORCE_OTA_BIN_UPDATE__)
    apps_ota_bin_ver_check();
#endif
    app_bt_init();
#if defined(__IAG_BLE_INCLUDE__) && defined(BLE_V2)
    app_ble_rx_handler_init();
#endif

    af_open();
    app_audio_open();
    app_audio_manager_open();
    app_overlay_open();

//    app_bt_connect2tester_init();

#ifdef BISTO_ENABLED
    nv_record_gsound_rec_init();
#endif

#ifdef PROMPT_IN_FLASH
    nv_record_prompt_rec_init();
#endif

#ifdef __IAG_BLE_INCLUDE__
    app_ble_mode_init();
    app_ble_customif_init();
#endif

    audio_process_init();
#ifdef __PC_CMD_UART__
    app_cmd_open();
#endif

#ifdef AUDIO_DEBUG
    speech_tuning_init();
    audio_test_cmd_init();
#endif

#ifdef AUDIO_HEARING_COMPSATN
    hearing_det_cmd_init();
#endif

#if defined(ANC_ASSIST_ENABLED)
    app_anc_assist_init();
    app_voice_assist_wd_init();
    app_voice_assist_ai_voice_init();
    app_voice_assist_prompt_leak_detect_init();
#if defined(VOICE_ASSIST_ONESHOT_ADAPTIVE_ANC)
    app_voice_assist_oneshot_adapt_anc_init();
#endif
#if defined(VOICE_ASSIST_WD_ENABLED)
	app_voice_assist_ultrasound_init();
#endif
    // app_voice_assist_wd_open();
#endif

#ifdef ANC_APP
    app_anc_init();
#endif

#if defined(MEDIA_PLAYER_SUPPORT) && !defined(PROMPT_IN_FLASH)
    app_play_audio_set_lang(nvrecord_env->media_language.language);
#endif
    app_bt_stream_volume_ptr_update(NULL);

#ifdef __THIRDPARTY
    app_thirdparty_init();
    app_thirdparty_specific_lib_event_handle(THIRDPARTY_FUNC_NO2,THIRDPARTY_INIT, AI_SPEC_INIT);
#endif

    // TODO: freddie->unify all of the OTA modules
#if defined(BES_OTA)
    bes_ota_init_handler();
#endif

#ifdef AI_OTA
    /// init OTA common module
    ota_common_init_handler();
#endif // AI_OTA

#ifdef SENSOR_HUB
    app_sensor_hub_init();
#endif

    btdrv_start_bt();
#if defined (__GMA_VOICE__) && defined(IBRT_SEARCH_UI)
    app_ibrt_reconfig_btAddr_from_nv();
#endif

#if defined(BT_WATCH_MASTER) || defined(BT_WATCH_SLAVE)
    app_bt_watch_open();
#endif

#ifdef SPECIFIC_FREQ_POWER_CONSUMPTION_MEASUREMENT_ENABLE
    app_start_power_consumption_thread();
#endif

#ifdef __CONNECTIVITY_LOG_REPORT__
    app_ibrt_if_report_connectivity_log_init();
#endif

    if (pwron_case != APP_POWERON_CASE_TEST) {
        BesbtInit();
        app_wait_stack_ready();
        bt_drv_extra_config_after_init();
        bt_generate_ecdh_key_pair();
        app_bt_start_custom_function_in_bt_thread((uint32_t)0,
            0, (uint32_t)app_ibrt_init);
#if defined(__IAG_BLE_INCLUDE__) && defined(IBRT)
        app_ble_force_switch_adv(BLE_SWITCH_USER_IBRT, true);
#endif
    }

#ifdef IBRT
#if defined(BISTO_ENABLED) || defined(__AI_VOICE__)
    ibrt_mgr_get_config()->ibrt_with_ai=true;
#endif
#endif
    
    app_sysfreq_req(APP_SYSFREQ_USER_APP_INIT, APP_SYSFREQ_52M);
    TRACE(2,"\n\n\nbt_stack_init_done:%d\n\n\n need_check_key:%d\n\n\n", pwron_case,need_check_key);

    if (pwron_case == APP_POWERON_CASE_REBOOT){
#if defined(__XSPACE_UI__)
        app_status_indication_set(APP_STATUS_INDICATION_POWERON);
#ifdef MEDIA_PLAYER_SUPPORT
        //app_voice_report(APP_STATUS_INDICATION_POWERON, 0);
#endif
#endif
        app_bt_sleep_init();

#if defined(OTA_ENABLE)
        ota_basic_env_init();
#endif

#if defined(BES_OTA)
        bes_ota_init();
#endif

#if defined(IBRT)
#ifdef IBRT_SEARCH_UI
        if(is_charging_poweron==false){
            if(IBRT_UNKNOW == nvrecord_env->ibrt_mode.mode){
                TRACE(0,"ibrt_ui_log:power on unknow mode");
                app_ibrt_enter_limited_mode();
            }else {
                TRACE(1,"ibrt_ui_log:power on %d fetch out", nvrecord_env->ibrt_mode.mode);
#ifdef SEARCH_UI_COMPATIBLE_UI_V2
                app_ibrt_if_event_entry(IBRT_MGR_EV_UNDOCK);
#else
                app_ibrt_ui_event_entry(IBRT_FETCH_OUT_EVENT);
#endif
            }
        }
#elif defined(IS_MULTI_AI_ENABLED)&&defined(IBRT_UI_V1)
        //when ama and bisto switch, earphone need reconnect with peer, master need reconnect with phone
        uint8_t box_action = app_ai_tws_reboot_get_box_action();
        if (box_action != 0xFF)
        {
            TRACE(2, "%s box_actionstate %d", __func__, box_action);
            app_ibrt_ui_event_entry(box_action|IBRT_SKIP_FALSE_TRIGGER_MASK);
        }
#endif
#else
        app_bt_accessmode_set(BTIF_BAM_NOT_ACCESSIBLE);
#endif
#if defined(__OPEN_BES_KEY_HANDLE__)
        app_key_init();
#endif
#if defined(__XSPACE_BATTERY_MANAGER__)
        app_10second_timer_init();
#else
        app_battery_start();
#endif
#if defined(__BTIF_EARPHONE__) && defined(__BTIF_AUTOPOWEROFF__)
        app_start_10_second_timer(APP_POWEROFF_TIMER_ID);
#endif

#if defined(__IAG_BLE_INCLUDE__) && defined(BTIF_BLE_APP_DATAPATH_SERVER)
        BLE_custom_command_init();
#endif
#ifdef __THIRDPARTY
        app_thirdparty_specific_lib_event_handle(THIRDPARTY_FUNC_NO1,THIRDPARTY_INIT, AI_SPEC_INIT);
        app_thirdparty_specific_lib_event_handle(THIRDPARTY_FUNC_NO1,THIRDPARTY_START, AI_SPEC_INIT);
        app_thirdparty_specific_lib_event_handle(THIRDPARTY_FUNC_NO2,THIRDPARTY_BT_CONNECTABLE, AI_SPEC_INIT);
        app_thirdparty_specific_lib_event_handle(THIRDPARTY_FUNC_NO3,THIRDPARTY_START, AI_SPEC_INIT);
#endif
#if defined( __BTIF_EARPHONE__) && defined(__BTIF_BT_RECONNECT__)
#if !defined(IBRT)
        app_bt_profile_connect_manager_opening_reconnect();
#endif
#endif
    }
#ifdef __ENGINEER_MODE_SUPPORT__
    else if(pwron_case == APP_POWERON_CASE_TEST){
        app_factorymode_set(true);
        //app_status_indication_set(APP_STATUS_INDICATION_POWERON);
#ifdef MEDIA_PLAYER_SUPPORT
        //app_voice_report(APP_STATUS_INDICATION_POWERON, 0);
#endif
#ifdef __WATCHER_DOG_RESET__
        app_wdt_close();
#endif
        TRACE(0,"!!!!!ENGINEER_MODE!!!!!\n");
        nRet = 0;
        app_nvrecord_rebuild();
#if defined(__XSPACE_UI__)
        //app_factorymode_key_init();
#else
        app_factorymode_key_init();
#endif
        if (hal_sw_bootmode_get() & HAL_SW_BOOTMODE_TEST_SIGNALINGMODE){
            hal_sw_bootmode_clear(HAL_SW_BOOTMODE_TEST_MASK);
            app_factorymode_bt_signalingtest(NULL, NULL);
                        TRACE(0, "Enter DUT mode success!");
            // TODO: XBUS init
#if defined(__XSPACE_UI__)
#if defined(__XSPACE_XBUS_MANAGER__)   //longz 20200511 add
            xspace_xbus_manage_init();
#endif
            while (1)
                ;
#endif
        }
        if (hal_sw_bootmode_get() & HAL_SW_BOOTMODE_TEST_NOSIGNALINGMODE){
            hal_sw_bootmode_clear(HAL_SW_BOOTMODE_TEST_MASK);
            app_factorymode_bt_nosignalingtest(NULL, NULL);
        }
    }
#endif
    else {
#if defined(__XSPACE_UI__)
        app_status_indication_set(APP_STATUS_INDICATION_POWERON);
#ifdef MEDIA_PLAYER_SUPPORT
        //app_voice_report(APP_STATUS_INDICATION_POWERON, 0);
#endif
#endif

#if defined(__OPEN_BES_KEY_HANDLE__)
        if (need_check_key) {
            pwron_case = app_poweron_wait_case();
        } else
#endif
        {
            pwron_case = APP_POWERON_CASE_NORMAL;
        }
        if (pwron_case != APP_POWERON_CASE_INVALID && pwron_case != APP_POWERON_CASE_DITHERING) {
            TRACE(1, "power on case:%d\n", pwron_case);
            nRet = 0;
#ifndef __POWERKEY_CTRL_ONOFF_ONLY__
            //app_status_indication_set(APP_STATUS_INDICATION_INITIAL);
#endif
            app_bt_sleep_init();

#if defined(OTA_ENABLE)
            ota_basic_env_init();
#endif

#ifdef BES_OTA
#if defined(__XSPACE_RAM_ACCESS__) && (defined(__TOUCH_GH61X__) || defined(__TOUCH_GH621X__))
            //czh add 20201221
            if (xra_spp_mode_read_noInit_data() == 0x5aa5aa01) {
                //xspace_interface_delay_timer_stop((uint32_t)bes_ota_init);
                //xspace_interface_delay_timer_start(200, (uint32_t)bes_ota_init, 0, 0, 0);
            } else {
                bes_ota_init();
            }
#else
            bes_ota_init();
#endif
#endif

#ifdef __INTERCONNECTION__
            app_interconnection_init();
#endif

#ifdef __INTERACTION__
            app_interaction_init();
#endif

#if defined(__IAG_BLE_INCLUDE__) && defined(BTIF_BLE_APP_DATAPATH_SERVER)
            BLE_custom_command_init();
#endif
#ifdef GFPS_ENABLED
            app_gfps_set_battery_info_acquire_handler(app_tell_battery_info_handler);
            app_gfps_set_battery_datatype(SHOW_UI_INDICATION);
#endif
#if defined(__XSPACE_UI__)
            xspace_ui_init();
#endif
            switch (pwron_case) {
                case APP_POWERON_CASE_CALIB:
                    break;
                case APP_POWERON_CASE_BOTHSCAN:
                    app_status_indication_set(APP_STATUS_INDICATION_BOTHSCAN);
#ifdef MEDIA_PLAYER_SUPPORT
                    app_voice_report(APP_STATUS_INDICATION_BOTHSCAN,0);
#endif
#if defined( __BTIF_EARPHONE__)
#if defined(IBRT)
#ifdef IBRT_SEARCH_UI
                    if(false==is_charging_poweron)
                        app_ibrt_enter_limited_mode();
#endif
#else
                    app_bt_accessmode_set(BTIF_BT_DEFAULT_ACCESS_MODE_PAIR);
#endif
#ifdef GFPS_ENABLED
                    app_enter_fastpairing_mode();
#endif
#if defined(__BTIF_AUTOPOWEROFF__)
                    app_start_10_second_timer(APP_PAIR_TIMER_ID);
#endif
#endif
#ifdef __THIRDPARTY
                    app_thirdparty_specific_lib_event_handle(THIRDPARTY_FUNC_NO2,THIRDPARTY_BT_DISCOVERABLE, AI_SPEC_INIT);
#endif
                    break;
                case APP_POWERON_CASE_NORMAL:
#if defined( __BTIF_EARPHONE__ ) && !defined(__EARPHONE_STAY_BOTH_SCAN__)
#if defined(IBRT)
#ifdef IBRT_SEARCH_UI
                    if(is_charging_poweron==false)
                    {
                        if(IBRT_UNKNOW == nvrecord_env->ibrt_mode.mode)//未配对过对耳
                        {
                            TRACE(0,"ibrt_ui_log:power on unknow mode");
                            app_ibrt_enter_limited_mode();
                        }
                        else//已配对过对耳，接下来出盒与手机配对或连接
                        {
                            TRACE(1,"ibrt_ui_log:power on %d fetch out", nvrecord_env->ibrt_mode.mode);
#ifdef SEARCH_UI_COMPATIBLE_UI_V2//进入与手机的配对或回连模式
                            app_ibrt_if_event_entry(IBRT_MGR_EV_UNDOCK);//只是一个接口
#else
                            app_ibrt_ui_event_entry(IBRT_FETCH_OUT_EVENT);
#endif


                        }
                    }
#elif defined(IS_MULTI_AI_ENABLED)
                    //when ama and bisto switch, earphone need reconnect with peer, master need reconnect with phone
                    //app_ibrt_ui_event_entry(IBRT_OPEN_BOX_EVENT);
                    //TRACE(1,"ibrt_ui_log:power on %d fetch out", nvrecord_env->ibrt_mode.mode);
                    //app_ibrt_ui_event_entry(IBRT_FETCH_OUT_EVENT);
#endif
#else
                    app_bt_accessmode_set(BTIF_BAM_NOT_ACCESSIBLE);
#endif
#endif
                    /* FALL THROUGH*/
                case APP_POWERON_CASE_REBOOT:
                case APP_POWERON_CASE_ALARM:
                default:
                    //app_status_indication_set(APP_STATUS_INDICATION_PAGESCAN);
#if defined(__BTIF_EARPHONE__) && defined(__BTIF_BT_RECONNECT__) && !defined(IBRT)
                    app_bt_profile_connect_manager_opening_reconnect();
#endif
#ifdef __THIRDPARTY
                    app_thirdparty_specific_lib_event_handle(THIRDPARTY_FUNC_NO2,THIRDPARTY_BT_CONNECTABLE, AI_SPEC_INIT);
#endif

                    break;
            }
#if defined(__XSPACE_UI__)
            ibrt_mgr_config_t *mgr_config = ibrt_mgr_get_config();
            if (mgr_config->freeman_enable) {
                xspace_ui_set_system_status(XUI_SYS_FORCE_FREEMAN_PARING);
            }
#endif
#if defined(__OPEN_BES_KEY_HANDLE__)
            if (need_check_key)
            {
#ifndef __POWERKEY_CTRL_ONOFF_ONLY__
                app_poweron_wait_finished();
#endif
            }
            app_key_init();
#endif
#if defined(__XSPACE_BATTERY_MANAGER__)
            app_10second_timer_init();
#else
            app_battery_start();
#endif
#if defined(__BTIF_EARPHONE__) && defined(__BTIF_AUTOPOWEROFF__)
            app_start_10_second_timer(APP_POWEROFF_TIMER_ID);
#endif
#ifdef __THIRDPARTY
            app_thirdparty_specific_lib_event_handle(THIRDPARTY_FUNC_NO1,THIRDPARTY_INIT, AI_SPEC_INIT);
            app_thirdparty_specific_lib_event_handle(THIRDPARTY_FUNC_NO1,THIRDPARTY_START, AI_SPEC_INIT);
            app_thirdparty_specific_lib_event_handle(THIRDPARTY_FUNC_NO3,THIRDPARTY_START, AI_SPEC_INIT);
#endif

#ifdef RB_CODEC
            rb_ctl_init();
#endif
        }else{
            af_close();
            app_key_close();
            nRet = -1;
        }
    }
exit:

#ifdef IS_MULTI_AI_ENABLED
    app_ai_tws_clear_reboot_box_state();
#endif

    btdrv_enable_tports();

#ifdef BT_USB_AUDIO_DUAL_MODE
    btusb_init();
    if(usb_plugin)
    {
        btusb_start_switch_to(BTUSB_MODE_USB);
    }
    else
    {
        btusb_switch(BTUSB_MODE_BT);
    }
#else //BT_USB_AUDIO_DUAL_MODE
#if defined(BTUSB_AUDIO_MODE)
    if(pwron_case == APP_POWERON_CASE_CHARGING) {
        app_wdt_close();
        af_open();
        app_key_handle_clear();
        app_usb_key_init();
        app_usbaudio_entry();
    }

#endif // BTUSB_AUDIO_MODE
#endif // BT_USB_AUDIO_DUAL_MODE
    app_sysfreq_req(APP_SYSFREQ_USER_APP_INIT, APP_SYSFREQ_32K);

    return nRet;
}

#ifdef SPECIFIC_FREQ_POWER_CONSUMPTION_MEASUREMENT_ENABLE

// for power consumption test on different cpu frequency
// test configuation
#define POWER_CONSUMPTION_MEASUREMENT_FREQ  APP_SYSFREQ_26M
#define IS_TEST_MCU_AND_CP

#if defined(CHIP_HAS_CP)&&defined(IS_TEST_MCU_AND_CP)
#include "cp_accel.h"

CP_TEXT_SRAM_LOC
static unsigned int cp_power_consumption_measurement_main(uint8_t event)
{
    return 0;
}

static struct cp_task_desc TASK_DESC_POWER_CONSUMPTION_MEASUREMENT =
    {CP_ACCEL_STATE_CLOSED, cp_power_consumption_measurement_main,
    NULL, NULL, NULL};

static void power_consumption_measurement_start_cp(void)
{
    norflash_api_flush_disable(NORFLASH_API_USER_CP,(uint32_t)cp_accel_init_done);
    cp_accel_open(CP_TASK_POWER_CONSUMPTION_MEASUREMENT, &TASK_DESC_POWER_CONSUMPTION_MEASUREMENT);
    while(cp_accel_init_done() == false) {
        hal_sys_timer_delay_us(100);
    }
    norflash_api_flush_enable(NORFLASH_API_USER_CP);
}
#endif

static void power_consumption_thread(const void *arg);
osThreadDef(power_consumption_thread, osPriorityHigh, 1, (1024), "power_consumption_thread");
static osThreadId power_consumption_thread_id;

static void power_consumption_thread(const void *arg)
{
    // release all the allocaetd frequencies
    for (uint32_t user = APP_SYSFREQ_USER_APP_INIT;
        user < APP_SYSFREQ_USER_QTY; user++)
    {
        app_sysfreq_req((enum APP_SYSFREQ_USER_T)user, APP_SYSFREQ_32K);
    }

#if defined(CHIP_HAS_CP)&&defined(IS_TEST_MCU_AND_CP)
    power_consumption_measurement_start_cp();
#endif

    enum APP_SYSFREQ_FREQ_T testFreq = POWER_CONSUMPTION_MEASUREMENT_FREQ;

    if (APP_SYSFREQ_26M == testFreq)
    {
        TRACE(0, "Test power consumption at sys Freq 24Mhz.");
    }
    else if (APP_SYSFREQ_52M == testFreq)
    {
        TRACE(0, "Test power consumption at sys Freq 48Mhz.");
    }
    else if (APP_SYSFREQ_78M == testFreq)
    {
        TRACE(0, "Test power consumption at sys Freq 72Mhz.");
    }
    else if (APP_SYSFREQ_104M == testFreq)
    {
        TRACE(0, "Test power consumption at sys Freq 96Mhz.");
    }
    else if (APP_SYSFREQ_208M == testFreq)
    {
        TRACE(0, "Test power consumption at sys Freq 192Mhz.");
    }

    app_sysfreq_req(APP_SYSFREQ_USER_APP_15,
        (enum APP_SYSFREQ_FREQ_T)POWER_CONSUMPTION_MEASUREMENT_FREQ);
    while(1);
}

void app_start_power_consumption_thread(void)
{
    power_consumption_thread_id =
        osThreadCreate(osThread(power_consumption_thread), NULL);
}
#endif

#endif /* APP_TEST_MODE */
