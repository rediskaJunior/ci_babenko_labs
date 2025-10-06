/*
 * cli.c
 *
 *  Created on: Oct 4, 2025
 *      Author: Cerberus
 */

#include "cli.h"
#include "usbd_cdc_if.h"
#include "string.h"
#include "stdio.h"
#include "gps.h"
#include "nmea.h"

static char rxbuf[256];
static int rxpos = 0;
static int raw_mode = 0;

void cli_init(void) {
    memset(rxbuf, 0, sizeof(rxbuf));
    rxpos = 0;
}

void cli_rx_callback(uint8_t* data, uint32_t len) {
    for (uint32_t i=0;i<len;i++) {
        char c = (char)data[i];
        if (c == '\r' || c == '\n') {
            if (rxpos>0) {
                rxbuf[rxpos]=0;
                if (strcmp(rxbuf, "VER")==0) {
                    char v[128];
                    snprintf(v, sizeof(v), "VER: gpsnode v1.0 build: %s %s\r\n", __DATE__, __TIME__);
                    CDC_Transmit_FS((uint8_t*)v, strlen(v));
                } else if (strcmp(rxbuf, "GET POS")==0) {
                    gps_pos_t p = gps_get_last_position();
                    char out[256];
                    if (p.valid) {
                        char lat[64], lon[64], tbuf[64];
                        format_lat_lon(p.lat_deg, p.lon_deg, lat, sizeof(lat), 6);
                        format_utc_time(p.year,p.month,p.day,p.hour,p.min,p.sec,tbuf,sizeof(tbuf));
                        snprintf(out, sizeof(out), "POS,%s,%s,fix=%d,sats=%d,age_ms=%lu\r\n", lat, lon, p.fix, p.sats, (unsigned long)gps_get_last_age_ms());
                    } else {
                        snprintf(out, sizeof(out), "POS,N/A\r\n");
                    }
                    CDC_Transmit_FS((uint8_t*)out, strlen(out));
                } else if (strcmp(rxbuf, "GET TIME")==0) {
                    gps_pos_t p = gps_get_last_position();
                    char out[128];
                    if (p.valid) {
                        char tbuf[64];
                        format_utc_time(p.year,p.month,p.day,p.hour,p.min,p.sec,tbuf,sizeof(tbuf));
                        snprintf(out, sizeof(out), "TIME,%s,stale=%s\r\n", tbuf, (gps_get_last_age_ms()>2000)?"1":"0");
                    } else {
                        snprintf(out, sizeof(out), "TIME,N/A\r\n");
                    }
                    CDC_Transmit_FS((uint8_t*)out, strlen(out));
                } else if (strcmp(rxbuf, "RAW ON")==0) {
                    raw_mode = 1;
                    CDC_Transmit_FS((uint8_t*)"RAW ON\r\n", 7);
                } else if (strcmp(rxbuf, "RAW OFF")==0) {
                    raw_mode = 0;
                    CDC_Transmit_FS((uint8_t*)"RAW OFF\r\n", 8);
                } else {
                    CDC_Transmit_FS((uint8_t*)"ERR,unknown command\r\n", 21);
                }
            }
            rxpos = 0;
        } else {
            if (rxpos < (int)sizeof(rxbuf)-1) rxbuf[rxpos++] = c;
        }
    }
}

void cli_process(void) {
    (void) raw_mode;
}


