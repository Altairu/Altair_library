#include "mdd.h"

SkenMdd::SkenMdd(HardwareSerial &s) : serial(s), seq(0) // 初期化
{
}

void SkenMdd::init(void)
{
    serial.begin(115200);
}

bool SkenMdd::tcp(uint8_t id, const float (&command_data)[4], unsigned int resend_time, unsigned int max_wait_time)
{
    uint8_t receive_data = -1;
    unsigned int send_time = 0;
    unsigned int start_time = millis();
    do
    {
        if ((millis() - start_time) > max_wait_time)
        {
            return false;
        }
        if ((millis() - send_time) > resend_time)
        {
            sendData(id, command_data);
            send_time = millis();
        }
        if (serial.available() > 0)
        {
            receive_data = serial.read();
        }
    } while (receive_data != seq);
    return true;
}

void SkenMdd::udp(uint8_t id, const float (&command_data)[4])
{
    sendData(id, command_data);
}

void SkenMdd::sendData(uint8_t id, const float (&command_data)[4])
{
    uint8_t send_data[21] = {};
    send_data[0] = 0xA5;
    send_data[1] = 0xA5;
    send_data[2] = ++seq;
    send_data[3] = id;
    setFloatData(command_data, send_data);
    send_data[20] = calcChecksum(send_data);
    serial.write(send_data, 21);
}

void SkenMdd::setFloatData(const float (&command_data)[4], uint8_t (&send_data)[21])
{
    ConvertIntFloat cif;
    for (int i = 0; i < 4; i++)
    {
        cif.float_val = command_data[i];
        for (int j = 0; j < 4; j++)
        {
            send_data[4 + (i * 4) + j] = cif.uint8_val[j];
        }
    }
}

uint8_t SkenMdd::calcChecksum(const uint8_t (&send_data)[21])
{
    uint8_t checksum = 0;
    for (int i = 2; i < 20; i++)
    {
        checksum += send_data[i];
    }
    return checksum;
}
