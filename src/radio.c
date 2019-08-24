/*
 * Здесь "бизнес-логика" радиосвязи.
 */

#include "radio.h"
#include "nrf24l01p.h"
#include "delay.h"
#include "debug.h"


void radio_init(void)
{
    static uint8_t address[4] = {0xC7, 0x68, 0xAC, 0x35};
    /* Подали питание. */
    nrf_init();
    delay_ms(100);
    /* Power down --> Standby_1 */
    nrf_overwrite_byte(CONFIG, PWR_UP);
    delay_ms(5);
    
    /* Включаем проверку контрольной суммы, выключаем прерывания на ноге IRQ
       и переводимся в режим передатчика. */
    nrf_rmw_byte(CONFIG, EN_CRC | MASK_TX_DS | MASK_RX_DR | MASK_MAX_RT, SET);

    /* Выбираем частотный канал. */
    nrf_overwrite_byte(RF_CH, 33);
    /* Выбираем скорость работы 1 Мбит/с и мощность 0 dbm. */
    nrf_overwrite_byte(RF_SETUP, 0 | RF_SETUP_0DBM);
    
    /* Выбираем длину адреса в 4 байта. */
    nrf_overwrite_byte(SETUP_AW, SETUP_AW_4BYTES_ADDRESS);
    /* Настраиваем адрес канала. */
    nrf_rw_buff(W_REGISTER | TX_ADDR, address, 4, NRF_OPERATION_WRITE);
    nrf_rw_buff(W_REGISTER | RX_ADDR_P0, address, 4, NRF_OPERATION_WRITE);

    /* Включаем возможность не указывать размер полезной нагрузки
       и отправлять пакеты, не требующие подтверждения. */
    nrf_overwrite_byte(FEATURE, EN_DPL | EN_DYN_ACK | EN_ACK_PAY);
    
    /* Включаем канал 0 для приёма подтверждений. */
    nrf_overwrite_byte(EN_RXADDR, ERX_P0);
    nrf_overwrite_byte(EN_AA, ENAA_P0);
    /* Включаем динамическую длину полезной нагрузки на канале 0. */
    nrf_overwrite_byte(DYNPD, DPL_P0);
    
    /* Настраиваем повторную отправку. */
    nrf_overwrite_byte(SETUP_RETR, SETUP_RETR_DELAY_750MKS
                       | SETUP_RETR_UP_TO_5_RETRANSMIT);

#if 1
    if (nrf_read_byte(CONFIG) != (PWR_UP | MASK_RX_DR | MASK_TX_DS | MASK_MAX_RT | EN_CRC)) {
        logs("config failed\n");
    }
    if (nrf_read_byte(RF_CH) != 33) {
        logs("channel failed\n");
    }
    if (nrf_read_byte(RF_SETUP) != RF_SETUP_0DBM) {
        logs("rf_setup failed\n");
    }
    if (nrf_read_byte(SETUP_AW) != SETUP_AW_4BYTES_ADDRESS) {
        logs("setup_aw failed\n");
    }
    if (nrf_read_byte(FEATURE) != (EN_DPL | EN_DYN_ACK)) {
        logs("feature failed\n");
    }
    if (nrf_read_byte(EN_RXADDR) != ERX_P0) {
        logs("en_rxaddr failed\n");
    }
    if (nrf_read_byte(EN_AA) != ENAA_P0) {
        logs("enaa failed\n");
    }
    if (nrf_read_byte(DYNPD) != DPL_P0) {
        logs("dynpd failed\n");
    }
    if (nrf_read_byte(SETUP_RETR) != (SETUP_RETR_DELAY_750MKS | SETUP_RETR_UP_TO_5_RETRANSMIT)) {
        logs("retr failed\n");
    }
#endif

    /* Чистим буферы на всякий случай. */
    nrf_cmd(FLUSH_TX);
    nrf_cmd(FLUSH_RX);
    /* Настраиваем таймер на тикание примерно 244 раза в секунду. */
    TIM3_TimeBaseInit(TIM3_PRESCALER_32768, 1000);
    TIM3_Cmd(ENABLE);
}

void radio_send_data(DataToRobot* data_to_robot)
{
    uint8_t status = nrf_get_status();
    if (status & TX_FULL_STATUS)  {
        nrf_cmd(FLUSH_TX);
        logs("flush tx\n");
    }
    if (status & MAX_RT) {
        nrf_clear_interrupts();
        logs("max_rt\n");
    }
    nrf_rw_buff(W_TX_PAYLOAD, (uint8_t*) data_to_robot,
                sizeof(DataToRobot), NRF_OPERATION_WRITE);
    nrf_ce_1();
    delay_ms(1);
    nrf_ce_0();
}

void radio_check_for_incoming(DataFromRobot* data_from_robot)
{
    uint8_t fifo_status = nrf_read_byte(FIFO_STATUS);
    logi(fifo_status); logs("\n");
    if (!(fifo_status & RX_EMPTY)) {
        uint8_t data_size;
        logs("smth in rx\n");
        nrf_rw_buff(R_RX_PL_WID, &data_size, 1, NRF_OPERATION_READ);
        if (data_size != sizeof(DataFromRobot)) {
            nrf_cmd(FLUSH_RX);
            logs("size conflict\n");
            return;
        }
        nrf_rw_buff(R_RX_PAYLOAD, (uint8_t*) data_from_robot,
                    sizeof(DataFromRobot), NRF_OPERATION_READ);
        logi(data_from_robot->battery_brains); logs("\n");
        logi(data_from_robot->battery_motors); logs("\n");
        logi(data_from_robot->temperature);    logs("\n");
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
        data_to_robot->direction     != buffer_to_robot.direction   ||
        data_to_robot->speed_left    != buffer_to_robot.speed_left  ||
        data_to_robot->speed_right   != buffer_to_robot.speed_right ||
        data_to_robot->control_flags != buffer_to_robot.control_flags
    );
    if (data_is_new) {
        buffer_to_robot.direction     = data_to_robot->direction;
        buffer_to_robot.speed_left    = data_to_robot->speed_left;
        buffer_to_robot.speed_right   = data_to_robot->speed_right;
        buffer_to_robot.control_flags = data_to_robot->control_flags;
    }
    return data_is_new;
}
