/*
 * Здесь функции для низкоуровневого взаимодействия
 * с модулем NRF24L01+ по интерфейсной шине SPI.
 */

#include "nrf24l01p.h"
#include "debug.h"

#define nrf_read_to_buffer(reg_addr, buf_addr, size) \
    nrf_rw_buff(R_REGISTER | ((reg_addr) & 0x1F), (buf_addr), (size), NRF_OPERATION_READ)
#define nrf_write_from_buffer(reg_addr, buf_addr, size) \
    nrf_rw_buff(W_REGISTER | ((reg_addr) & 0x1F), (buf_addr), (size), NRF_OPERATION_WRITE)

static uint8_t nrf_spi_send_recv(uint8_t);

/**
 * @brief  Инициализирует пины радиомодуля.
 * @note   Эта функция платформозависимая.
 */
void nrf_init(void)
{
    GPIO_Init(NRF_GPIO, NRF_PIN_CSN, GPIO_MODE_OUT_PP_HIGH_FAST);
    GPIO_Init(NRF_GPIO, NRF_PIN_CE, GPIO_MODE_OUT_PP_LOW_FAST);
}

/**
 * @brief  Отправляет простую команду радиомодулю.
 * @param  cmd: Команда радиомодулю.
 * @retval Значение регистра STATUS радиомодуля.
 */
uint8_t nrf_cmd(NrfCommand cmd)
{
    uint8_t status;
    nrf_csn_0();
    status = nrf_spi_send_recv(cmd);
    nrf_csn_1();
    return status;
}

/**
 * @brief  Читает значение из однобайтового регистра радиомодуля.
 * @param  reg_addr: Адрес регистра откуда читать.
 * @retval Прочитанное значение.
 */
uint8_t nrf_read_byte(NrfRegAddress reg_addr)
{
    uint8_t reg_value;
    nrf_csn_0();
    nrf_spi_send_recv(R_REGISTER | (reg_addr & 0x1F));
    reg_value = nrf_spi_send_recv(NOP);
    nrf_csn_1();
    return reg_value;
}

/**
 * @brief  Записывает значение в однобайтовый регистр радиомодуля.
 * @note   При этом предыдущее значение этого регистра перезаписывается.
 * @param  reg_addr: Адрес регистра куда писать.
 * @param  bit_flags: Новое значение для регистра.
 */
void nrf_overwrite_byte(NrfRegAddress reg_addr, uint8_t bit_flags)
{
    nrf_csn_0();
    nrf_spi_send_recv(W_REGISTER | (reg_addr & 0x1F));
    nrf_spi_send_recv(bit_flags);
    nrf_csn_1();
}

/**
 * @brief  Делает read-modify-write с однобайтовым регистром радиомодуля.
 * @param  reg_addr: Адрес регистра для RMW-операции.
 * @param  bit_flags: Битовые флаги для применения к регистру.
 * @param  bit_status: Установить/сбросить указанные флаги.
 */
void nrf_rmw_byte(NrfRegAddress reg_addr, uint8_t bit_flags, BitStatus bit_status)
{
    uint8_t reg_value;
    nrf_read_to_buffer(reg_addr, &reg_value, 1);
    if (bit_status == SET)
        reg_value |= bit_flags;
    else 
        reg_value &= ~bit_flags;
    nrf_write_from_buffer(reg_addr, &reg_value, 1);
}

/**
 * @brief  Отправляет радиомодулю составную команду и 
 *         отправляет или читает дополнительные данные.
 * @param  composite_cmd: Команда радиомодулю.
 * @param  buff: Указатель на буфер для отправки 
 *         или сохранения дополнительных данных.
 * @param  size: Сколько байт доп. данных или ответа.
 * @param  operation: Отправлять доп. данные или принимать ответ.
 */
void nrf_rw_buff(uint8_t composite_cmd, uint8_t* buff, uint8_t size, NrfOperation operation)
{
    nrf_csn_0();
    nrf_spi_send_recv(composite_cmd);
    while (size--) {
        if (operation == NRF_OPERATION_READ)
            *buff++ = nrf_spi_send_recv(NOP);
        else if (operation == NRF_OPERATION_WRITE)
            nrf_spi_send_recv(*buff++);
    }
    nrf_csn_1();
}

/**
 * @brief  Передаёт радиомодулю 1 байт по SPI и принимает ответный.
 * @note   Эта функция платформозависимая.
 * @param  byte: байт для передачи радиомодулю.
 * @retval Ответный байт от радиомодуля.
 */
static uint8_t nrf_spi_send_recv(uint8_t byte)
{
    while (!SPI_GetFlagStatus(SPI_FLAG_TXE));
    SPI_SendData(byte);
    while (!SPI_GetFlagStatus(SPI_FLAG_RXNE) || SPI_GetFlagStatus(SPI_FLAG_BSY));
    return SPI_ReceiveData(); 
}