/*
 * cli.h
 *
 *  Created on: Oct 4, 2025
 *      Author: Cerberus
 */

#ifndef INC_CLI_H_
#define INC_CLI_H_

#include <stdint.h>

void cli_init(void);
void cli_process(void);
void cli_rx_callback(uint8_t* data, uint32_t len);


#endif /* INC_CLI_H_ */
