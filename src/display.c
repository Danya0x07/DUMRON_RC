/**
 * Здесь "биснес-логика"графического интерфейса.
 *
 * На данный момент GUI планируется таким: 
 *
 *             __1___2___3_
 *            |%100 100 100|    1: % заряда аккумулятора силовой части робота;
 *            |            |    2: % заряда батареи мозговой части робота;
 *            |            |    3: % заряда батареи пульта;
 *            |            |
 *            |          ^^|
 *            |          ^^|
 *            TTTTTTTTTTTTTT
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
                    const DataFromRobot* data_from_robot,
                    bool was_conn_error)
{
    
}
