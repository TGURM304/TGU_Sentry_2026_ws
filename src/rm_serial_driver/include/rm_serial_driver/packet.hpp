// Copyright (c) 2022 ChenJun
// Licensed under the Apache-2.0 License.

#ifndef RM_SERIAL_DRIVER__PACKET_HPP_
#define RM_SERIAL_DRIVER__PACKET_HPP_

#include <algorithm>
#include <cstdint>
#include <vector>

namespace rm_serial_driver
{
    struct ReceivePacket {
        uint8_t head[2] = {'T', 'G'};
        uint8_t mode;           // 0: 不控制, 1: 控制云台但不开火，2: 控制云台且开火
        float yaw;              // 目标偏航角(弧度制)
        float yaw_vel;          // 目标偏航角速度
        float yaw_acc;          // 目标偏航角加速度
        float pitch;            // 目标俯仰角(弧度制)
        float pitch_vel;        // 目标俯仰角速度
        float pitch_acc;        // 目标俯仰角加速度
        float vx;
        float vy;
        uint16_t crc16;
    } __attribute__((packed));
    static_assert(sizeof(ReceivePacket) <= 64);

struct SendPacket
{
  uint8_t head[2] = {'T', 'G'};
  uint8_t mode = 0;           // 0: 不控制, 1: 控制云台但不开火，2: 控制云台且开火
  float yaw = 0;              // 目标偏航角(弧度制)
  float yaw_vel = 0;          // 目标偏航角速度
  float yaw_acc = 0;          // 目标偏航角加速度
  float pitch = 0;            // 目标俯仰角(弧度制)
  float pitch_vel = 0;        // 目标俯仰角速度
  float pitch_acc = 0;        // 目标俯仰角加速度
  float vx = 0;
  float vy = 0;
  uint16_t crc16 = 0;
} __attribute__((packed));

inline ReceivePacket fromVector(const std::vector<uint8_t> & data)
{
  ReceivePacket packet;
  std::copy(data.begin(), data.end(), reinterpret_cast<uint8_t *>(&packet));
  return packet;
}

inline std::vector<uint8_t> toVector(const SendPacket & data)
{
  std::vector<uint8_t> packet(sizeof(SendPacket));
  std::copy(
    reinterpret_cast<const uint8_t *>(&data),
    reinterpret_cast<const uint8_t *>(&data) + sizeof(SendPacket), packet.begin());
  return packet;
}

}  // namespace rm_serial_driver

#endif  // RM_SERIAL_DRIVER__PACKET_HPP_
