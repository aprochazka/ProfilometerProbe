/**
 * @file  vl53l1_platform.h
 * @brief Large portion of this file was taken and edited from the VL53L1 Platform.
 * @author Adam Prochazka <xproch0f>
 */
 
#ifndef _VL53L1_PLATFORM_H_
#define _VL53L1_PLATFORM_H_

#include "vl53l1_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct {
	uint32_t dummy;
} VL53L1_Dev_t;

typedef VL53L1_Dev_t *VL53L1_DEV;

/** 
 * @brief VL53L1_WriteMulti() definition.
 */
int8_t VL53L1_WriteMulti(
		uint16_t 			dev,
		uint16_t      index,
		uint8_t      *pdata,
		uint32_t      count);

/** 
 * @brief VL53L1_ReadMulti() definition.
 */
int8_t VL53L1_ReadMulti(
		uint16_t 			dev,
		uint16_t      index,
		uint8_t      *pdata,
		uint32_t      count);

/** 
 * @brief VL53L1_WrByte() definition.
 */
int8_t VL53L1_WrByte(
		uint16_t dev,
		uint16_t      index,
		uint8_t       data);

/** 
 * @brief VL53L1_WrWord() definition.
 */
int8_t VL53L1_WrWord(
		uint16_t dev,
		uint16_t      index,
		uint16_t      data);

/** 
 * @brief VL53L1_WrDWord() definition.
 */
int8_t VL53L1_WrDWord(
		uint16_t dev,
		uint16_t      index,
		uint32_t      data);

/** 
 * @brief VL53L1_RdByte() definition.
 */
int8_t VL53L1_RdByte(
		uint16_t dev,
		uint16_t      index,
		uint8_t      *pdata);

/** 
 * @brief VL53L1_RdWord() definition.
 */
int8_t VL53L1_RdWord(
		uint16_t dev,
		uint16_t      index,
		uint16_t     *pdata);

/** 
 * @brief VL53L1_RdDWord() definition.
 */
int8_t VL53L1_RdDWord(
		uint16_t dev,
		uint16_t      index,
		uint32_t     *data);
		
/** 
 * @brief VL53L1_WaitMs() definition.
 */
int8_t VL53L1_WaitMs(
		uint16_t dev,
		int32_t       wait_ms);

#ifdef __cplusplus
}
#endif

#endif
