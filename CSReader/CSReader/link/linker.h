//linker.h
#include "algorithm.h"
#include "beep.h"
#include "iso14443.h"
#include "sam.h"
#include "ticket.h"
#include "octcard.h"
#include <stdint.h>
#include "../api/Declares.h"

#pragma once

#define SAM_SOCK_1      0
#define SAM_SOCK_2      1
#define SAM_SOCK_3      2
#define SAM_SOCK_4      3

void ubeep(int beep_time);

uint16_t change_antenna(uint8_t mode);

uint16_t sam_init(int sock_id, char * p_sam_id, char * p_tml_id);

uint16_t metro_svt_read(uint8_t * p_base, uint8_t * p_owner, uint8_t * p_history_last);

uint16_t metro_svt_history(uint8_t * p_history);


uint16_t rf_modify(uint8_t dev_type, uint8_t ant_mode, ETYTKOPER oper_type, int * p_rfmain_Status, int * p_rfsub_Status);

void Beep_Sleep(uint8_t beep_times, int MSTimeBeep = 30, int USTimeNoBeep = 30000);

bool Check_Issued();
