#if (ARDUINO >= 100)
 #include <Arduino.h>
#else
 #include <WProgram.h>
#endif

#include "utils.h"

/**
 * Get Less significant byte from int value
 *
 * @param  integerValue [description]
 *
 * @return              [description]
 */
uint8_t Utils::getLSB( uint16_t integerValue){
    return integerValue & 0xff;
}

/**
 * Get more significant byte from int value
 *
 * @param  integerValue [description]
 *
 * @return              [description]
 */
uint8_t Utils::getMSB( uint16_t integerValue){
    return (integerValue >> 8) & 0xff;
}