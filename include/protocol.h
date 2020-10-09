/**
 * Протокол общения пульта с роботом.
 */

#ifndef _PROTOCOL_H
#define _PROTOCOL_H

#include <stm8s.h>

/** Возможные направления вращения моторов робота. */
typedef enum {
    MOVEDIR_NONE = 0,  // в никуда
    MOVEDIR_FORWARD,   // оба вперёд
    MOVEDIR_BACKWARD,  // оба назад
    MOVEDIR_LEFTWARD,  // левый назад, правый вперёд
    MOVEDIR_RIGHTWARD  // левый вперёд, правый назад
} move_direction_t;

/** Возможные состояния плеча манипулятора. */
typedef enum {
    ARMCTL_STOP = 0,
    ARMCTL_UP,
    ARMCTL_DOWN
} arm_control_t;

/** Возможные состояние клешни манипулятора. */
typedef enum {
    CLAWCTL_STOP = 0,
    CLAWCTL_SQUEEZE,
    CLAWCTL_RELEASE
} claw_control_t;


/**
 * Частотный канал, а который настроены робот и пульт сразу после включения,
 * и который потом меняется.
 */
#define RADIO_INITIAL_CHANNEL	125

/**
 * Структура пакетов, идущих от пульта к роботу.
 */
typedef struct {
    union {
        struct {
            uint8_t move_dir  :3;
            uint8_t arm_ctrl  :2;
            uint8_t claw_ctrl :2;
            uint8_t lights_en :1;

            uint8_t buzzer_en :1;
            uint8_t reserved  :7;  /* резервируем байт на будущее */
        } bf;
        uint16_t reg;
    } ctrl;

    union {
    	struct {
    		uint8_t switched :1;
			uint8_t channel	 :7;
    	} bf;
    	uint8_t reg;
    } radio;

    uint8_t speed_left;
    uint8_t speed_right;
} data_to_robot_t;

/** Возможные реалии расстояния от кормы робота до поверхности. */
typedef enum {
    DIST_NOTHING = 0,
    DIST_CLIFF,
    DIST_OBSTACLE,
    DIST_ERROR  // если сонар сорвало
} distance_case_t;

/**
 * Невозможное для датчика температуры значение, свидетельствующее об ошибке.
 */
#define TEMPERATURE_ERROR_VALUE     (-128)

/**
 * Структура пакетов, идущих от робота к пульту.
 */
typedef struct {
    union {
        struct {
            uint8_t back_distance :2;
            uint8_t reserved      :6;
        } bf;
        uint8_t reg;
    } status;
    uint8_t battery_brains;
    uint8_t battery_motors;
    int8_t  temp_ambient;
    int8_t  temp_radiators;
} data_from_robot_t;

_Static_assert(sizeof(data_to_robot_t) == 5, "Unexpected payload size.");
_Static_assert(sizeof(data_from_robot_t) == 5, "Unexpected payload size.");

#endif /* _PROTOCOL_H */