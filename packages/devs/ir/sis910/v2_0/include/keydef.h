/* $Id: keydef.h 7080 2007-10-22 10:32:53Z william $ */
#ifndef _KEYDEF_H_
#define _KEYDEF_H_

/**************************************************************
* NOTE :
* If KEYS definitions are changed, the KEYS definitions
*    in the play.c should also changed.
**************************************************************/
/*- key definations ---------------------------------------------------------*/
#define _NO_KEY                  (-1)
#define _KEY_0                   0x00
#define _KEY_1                   0x01
#define _KEY_2                   0x02
#define _KEY_3                   0x03
#define _KEY_4                   0x04
#define _KEY_5                   0x05
#define _KEY_6                   0x06
#define _KEY_7                   0x07
#define _KEY_8                   0x08
#define _KEY_9                   0x09
#define _KEY_PLUS_10             0x0a
 
#define _KEY_PLUS                0x0b
#define _KEY_MINUS               0x0c
#define _KEY_RESUME_KEY          0x0d
#define _ECHO_PLUS               0x0e
#define _ECHO_MINUS              0x0f
#define _SURROUND_KEY            0x10
#define _VOCAL_ASSIST_KEY        0x11
#define _REPEAT_KEY              0x12
#define _PBC_KEY                 0x13
#define _TIME_KEY                0x14
#define _SCREEN_ON_KEY           0x15
#define _RESUME_KEY              0x16
#define _MUTE_KEY                0x17
#define _VOLUME_UP_KEY           0x18
#define _VOLUME_DOWN_KEY         0x19
#define _CLEAR_KEY               0x1a
#define _PROGRAM_KEY             0x1b
#define _SHUFFLE_KEY             0x1c
#define _INTRO_KEY               0x1d
#define _MODE_KEY                0x1e
#define _PLAY_KEY                0x1f
#define _STOP_KEY                0x20
#define _GOTO_KEY                0x21
#define _FB_KEY                  0x22
#define _FF_KEY                  0x23
#define _PAUSE_KEY               0x24
#define _SLOW_KEY                0x25
#define _STEP_KEY                0x26
#define _ADVANCE_KEY             0x27
#define _INDEX_MINUS_KEY         0x28
#define _INDEX_PLUS_KEY          0x29
#define _SET_A_KEY               0x2a
#define _SET_B_KEY               0x2b
#define _SETUP_KEY               0x2c
#define _TITLE_KEY               0x2d
#define _DVD_DIGEST_KEY          0x2e
#define _AUDIO_KEY               0x2f
#define _SUB_TITLE_KEY           0x30
#define _ANGLE_KEY               0x31
#define _ENTER_KEY               0x32
#define _TIME_SEARCH_KEY         0x33
#define _PAL_NTSC_KEY            0x34

/* KEYS are not in the switch statements. */
#define _DEFAULT_KEY             0x50
#define _TRACK_SCAN_KEY          0x51
#define _DISC_SCAN_KEY           0x52
#define _ROTATE_CLOCK_KEY        0x53
#define _ROTATE_ANTI_CLOCK_KEY   0x54
#define _FIRST_DISC_KEY          0x55
#define _SECOND_DISC_KEY         0x56
#define _THIRD_DISC_KEY          0x57
#define _BOOK_MARK_KEY           0x58
#define _OSD_LANGUAGE_KEY        0x59
#define _POWER_KEY               0x5a
#define _EJECT_KEY               0x5b
#define _PREVIOUS_KEY            0x5c
#define _NEXT_KEY                0x5d
#define _RETURN_KEY              0x60
#define _ZOOM_IN_KEY             0x61
#define _ZOOM_OUT_KEY            0x62
#define _ZOOM_UP_KEY             0x63
#define _ZOOM_LEFT_KEY           0x64
#define _ZOOM_RIGHT_KEY          0x65
#define _ZOOM_DOWN_KEY           0x66
#define _TV_MODE_KEY             0x67

#define _DISPLAY_KEY            _SCREEN_ON_KEY

#ifdef RECORDER
#define _MIC_RECORD_KEY         0x63
#define _MIC_CONTRAST_KEY       0x64
#define _MIC_REPEAT_KEY         0x65
#define _MIC_MODE_KEY           0x66
#endif

#ifdef DANCER
#define _START_KEY              0x73
#define _EXIT_KEY               0x74
#define _RIGHT_LOW_KEY          0x75
#define _LEFT_LOW_KEY           0x76
#define _RIGHT_MID_KEY          0x77
#define _LEFT_MID_KEY           0x78
#define _RIGHR_TOP_KEY          0x79
#define _LEFT_TOP_KEY           0x7a
#endif

#if defined(NEW_ESS_REMOTE) || defined(ESS_REMOTE) || defined(SAST_REMOTE) || defined(HS_KEY)
#define _DIGEST_KEY              0x0b
#define _AUDIOMODE_KEY           0x13
#define _OSD_KEY                 0x26
#define _SETAB_KEY               0x58
#endif

#define _RESET_KEY               0x72

#define _UP_KEY                  0x63
#define _LEFT_KEY                0x64
#define _RIGHT_KEY               0x65
#define _DOWN_KEY                0x66

#define _SLOW_MINUS_KEY          0x68
#define _STEP_MINUS_KEY          0x69 
#define _DYN_RNG_KEY             0x6A
#define _SUR_CONFIG_KEY          0x6B
#ifdef BOTH_DVE_BT656
#define	_SWITCH_DVE_BT656_KEY	 0x6c
#endif
#if defined(VFD_ZEC1301)
#define _VFD_KEY                 0x68
#define _KEY_PLUS_5              0x69
#define _EXCHANGE_KEY            0x6C
#define _REPEAT_AB_KEY           _SET_A_KEY
#define _ZOOM_CENTER_KEY         0x6B
#define _MENU_KEY                _DVD_DIGEST_KEY 
#define _EFFECT_KEY              0x6E
#define _SPD_ANL_KEY             0x6F
#define _FAST_PLAY_KEY           0x73

#define _EMAIL_KEY               0x75              
#define _INTERNET_KEY            0x76
#define _IPHONE_KEY              0x77

#define _DVD_KEY		0x7C

#define _STEP_PLUS_KEY		0x78
#define _STEP_MINUS_KEY		0x79
#define _SLOW_PLUS_KEY		0x7a
#define _SLOW_MINUS_KEY		0x7b
#endif

#ifdef TDM_SPDIF_IN
#define _SPDIF_IN_KEY           _CLEAR_KEY           
#endif

#ifdef PROLOGIC
#define _PLG_MODE_KEY           _GOTO_KEY
#endif

#ifdef OBJ_MENU
#define _DISPLAY_MENU_KEY       _GOTO_KEY
#endif

#ifdef HDMV
#define _POP_UP_KEY                 0x80
#define _PIP_KEY                 0x81
#define _TEXT_STYLE_KEY     0x82
#endif

#endif //keydef.h
