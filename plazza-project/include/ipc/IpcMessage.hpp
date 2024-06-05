#ifndef BUSMESSAGE_HPP
#define BUSMESSAGE_HPP

#include <memory>
#include <sstream>
#include <string>

#include "Constants.hpp"

class IpcMessage {
 protected:
  std::string senderIpcAddress;
  std::string receiverIpcAddress;
  std::string routingKey;
  long dispatchTimeSinceEpoch;
  std::string serializedPayload;

 public:
  IpcMessage(const std::string &senderIpcAddress,
             const std::string &receiverIpcAddress,
             const std::string &routingKey,
             const std::string &serializedPayload = DUMMY_PAYLOAD);

  virtual ~IpcMessage() = default;

  virtual std::string serialize() const;

  static std::shared_ptr<IpcMessage> deserialize(const std::string &data);

  std::string getSender() const;
  std::string getReceiver() const;
  std::string getRoutingKey() const;
  long getDispatchTimeSinceEpoch() const;
  std::string getSerializedPayload() const;
};

#endif  // BUSMESSAGE_HPP
