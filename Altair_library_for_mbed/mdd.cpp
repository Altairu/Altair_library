#include "mdd.h"

SkenMdd::SkenMdd(BufferedSerial& s) :
    serial(s), seq(0)
{
}

void SkenMdd::init(void)
{
    serial.set_baud(115200); // シリアル通信のボーレートを設定
}

bool SkenMdd::tcp(uint8_t id, const float (&command_data)[4], unsigned int resend_time, unsigned int max_wait_time)
{
    uint8_t receive_data = -1;
    auto send_time = Kernel::Clock::now();  // 修正: Kernel::Clock::now() を使用
    auto start_time = Kernel::Clock::now(); // 修正: Kernel::Clock::now() を使用
    do {
        if ((Kernel::Clock::now() - start_time).count() > max_wait_time) {
            return false; // 最大待機時間を超えた場合、失敗を返す
        }
        if ((std::chrono::duration_cast<std::chrono::milliseconds>(Kernel::Clock::now() - send_time).count()) > resend_time) {
            sendData(id, command_data); // 再送信
            send_time = Kernel::Clock::now(); // 再送信後の時間を更新
        }
        if (serial.readable()) {
            serial.read(&receive_data, 1);
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
    serial.write(send_data, sizeof(send_data));
}

void SkenMdd::setFloatData(const float (&command_data)[4], uint8_t (&send_data)[21])
{
    ConvertIntFloat cif;
    for (int i = 0; i < 4; i++) {
        cif.float_val = command_data[i];
        for (int j = 0; j < 4; j++) {
            send_data[4 + (i * 4) + j] = cif.uint8_val[j];
        }
    }
}

uint8_t SkenMdd::calcChecksum(const uint8_t (&send_data)[21])
{
    uint8_t checksum = 0;
    for (int i = 2; i < 20; i++) {
        checksum += send_data[i];
    }
    return checksum;
}

