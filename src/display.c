/*
 * Здесь "биснес-логика"графического интерфейса.
 */

#include "display.h"
#include "nokia5110lcd.h"

void display_init(void)
{
    lcd_init();
}

void display_test(void)
{
    lcd_set_position(1, 1);
    lcd_print_string("display test.");
}

void display_update(const DataToRobot* data_to_robot,
                    const DataFromRobot* data_from_robot)
{
    
}
