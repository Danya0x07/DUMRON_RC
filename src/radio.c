/**
 * Здесь "бизнес-логика" радиосвязи.
 */

#include "radio.h"
#include "nrf24l01p.h"
#include "delay.h"
#include "debug.h"

void radio_init(void)
{
    static uint8_t address[4] = {0xC7, 0x68, 0xAC, 0x35};
    /* Включаем проверку контрольной суммы, выключаем прерывания на ноге IRQ
       и переводимся в режим передатчика. */
    const uint8_t config = PWR_UP | EN_CRC | MASK_TX_DS | MASK_RX_DR | MASK_MAX_RT;
    /* Выбираем частотный канал. */
    const uint8_t rf_ch = 33;  
    /* Выбираем скорость работы 1 Мбит/с и мощность 0 dbm. */
    const uint8_t rf_setup = 0 | RF_SETUP_0DBM;
    /* Выбираем длину адреса в 4 байта. */
    const uint8_t setup_aw = SETUP_AW_4BYTES_ADDRESS;
    /* Включаем канал 0 для приёма подтверждений. */
    const uint8_t en_rxaddr = ERX_P0;
    /* Включаем возможность не указывать размер полезной нагрузки
       и принимать дополнительные данные вместе с пакетами подтверждения. */
    const uint8_t feature = EN_DPL | EN_ACK_PAY;
    /* Включаем динамическую длину полезной нагрузки на канале 0. */
    const uint8_t dynpd = DPL_P0;
    /* Настраиваем повторную отправку. */
    const uint8_t setup_retr = (SETUP_RETR, SETUP_RETR_DELAY_750MKS
                                | SETUP_RETR_UP_TO_5_RETRANSMIT);
    nrf_init_gpio();
    delay_ms(100);  /* Ожидание после подачи питания. */

    /* Записываем настройки в модуль. */
    nrf_overwrite_byte(CONFIG,     config);
    delay_ms(5);  /* Power down --> Standby_1 */
    nrf_overwrite_byte(RF_CH,      rf_ch);
    nrf_overwrite_byte(RF_SETUP,   rf_setup);
    nrf_overwrite_byte(SETUP_AW,   setup_aw);
    nrf_overwrite_byte(EN_RXADDR,  en_rxaddr);
    nrf_overwrite_byte(FEATURE,    feature);
    nrf_overwrite_byte(DYNPD,      dynpd);
    nrf_overwrite_byte(SETUP_RETR, setup_retr);
    /* Записываем адрес канала. */
    nrf_rw_buff(W_REGISTER | TX_ADDR,    address, 4, NRF_OPERATION_WRITE);
    nrf_rw_buff(W_REGISTER | RX_ADDR_P0, address, 4, NRF_OPERATION_WRITE);

    /* Если с модулем что-то не в порядке, то ни один регистр не запишется,
       поэтому одной проверки должно быть достаточно. */
    if (nrf_read_byte(CONFIG) != config) {
        logs("nrf not responding!\n");
    }
    
    /* Чистим буферы на всякий случай. */
    nrf_cmd(FLUSH_TX);
    nrf_cmd(FLUSH_RX);
    /* Настраиваем таймер на тикание примерно 244 раза в секунду. */
    TIM3_TimeBaseInit(TIM3_PRESCALER_32768, 1000);
    TIM3_Cmd(ENABLE);
}

bool radio_send_data(DataToRobot* data_to_robot)
{
    bool conn_err_occured = FALSE;
    uint8_t status = nrf_get_status();
    if (status & TX_FULL_STATUS)  {
        nrf_cmd(FLUSH_TX);
        conn_err_occured = TRUE;
        logs("tx full\n");
    }
    if (status & MAX_RT) {
        nrf_cmd(FLUSH_TX);
        nrf_clear_interrupts();
        conn_err_occured = TRUE;
        logs("max rt\n");
    }
    status = nrf_read_byte(OBSERVE_TX);
    logi(data_to_robot->direction); logs("\t");
    logi(data_to_robot->speed_left); logs("\t");
    logi(data_to_robot->speed_right); logs("\t");
    logi(data_to_robot->control_reg); logs("\n");
    logi(status); logs("\n");
    nrf_rw_buff(W_TX_PAYLOAD, (uint8_t*) data_to_robot,
                sizeof(DataToRobot), NRF_OPERATION_WRITE);
    nrf_ce_1();
    delay_ms(1);
    nrf_ce_0();
    return conn_err_occured;
}

void radio_check_for_incoming(DataFromRobot* data_from_robot)
{
    uint8_t fifo_status = nrf_read_byte(FIFO_STATUS);
    if (!(fifo_status & RX_EMPTY)) {
        uint8_t data_size;
        nrf_rw_buff(R_RX_PL_WID, &data_size, 1, NRF_OPERATION_READ);
        if (data_size != sizeof(DataFromRobot)) {
            nrf_cmd(FLUSH_RX);
            return;
        }
        nrf_rw_buff(R_RX_PAYLOAD, (uint8_t*) data_from_robot,
                    sizeof(DataFromRobot), NRF_OPERATION_READ);
        logi(data_from_robot->battery_brains);   logs("\t");
        logi(data_from_robot->battery_motors);   logs("\t");
        logi(data_from_robot->temp_manipulator); logs("\t");
        logi(data_from_robot->temp_environment); logs("\t\n");
    }
}

bool radio_is_time_to_update_io_data(void)
{
    bool is_time_to_update_io_data = TIM3_GetCounter() >= 500;
    if (is_time_to_update_io_data)
        TIM3_SetCounter(0);
    return is_time_to_update_io_data;
}

bool radio_data_to_robot_is_new(const DataToRobot* data_to_robot)
{
    static DataToRobot buffer_to_robot;
    
    bool data_is_new = (
        buffer_to_robot.direction     != data_to_robot->direction   ||
        buffer_to_robot.speed_left    != data_to_robot->speed_left  ||
        buffer_to_robot.speed_right   != data_to_robot->speed_right ||
        buffer_to_robot.control_reg != data_to_robot->control_reg
    );
    if (data_is_new) {
        buffer_to_robot.direction     = data_to_robot->direction;
        buffer_to_robot.speed_left    = data_to_robot->speed_left;
        buffer_to_robot.speed_right   = data_to_robot->speed_right;
        buffer_to_robot.control_reg = data_to_robot->control_reg;
    }
    return data_is_new;
}
