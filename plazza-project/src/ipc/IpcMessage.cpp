// #include "ipc/IpcMessage.hpp"
// #include <sstream>
//
// std::string CountBusyCooksRequest::serialize() const {
//     return type.toString();
// }
//
// std::string CountBusyCooksResponse::serialize() const {
//     std::stringstream ss;
//     ss << type.toString() << " " << busyCooks;
//     return ss.str();
// }
//
// std::string IpcMessage::serialize() const {
//     std::stringstream ss;
//     ss << type.toString();
//     return ss.str();
// }
//
// IpcMessageType IpcMessage::type = IpcMessageType(IpcMessageType::UNKNOWN);
//
// std::shared_ptr<IpcMessage> IpcMessage::deserialize(const std::string& data) {
//     std::istringstream iss(data);
//     std::string type;
//     iss >> type;
//     if (type == IpcMessageType::toString(IpcMessageType::COUNT_BUSY_COOKS_REQUEST)) {
//         return std::make_shared<CountBusyCooksRequest>();
//     }
//
//     if (type == IpcMessageType::toString(IpcMessageType::COUNT_BUSY_COOKS_RESPONSE)) {
//         int count;
//         iss >> count;
//         return std::make_shared<CountBusyCooksResponse>(count);
//     }
//
//     return std::make_shared<IpcMessage>();
// }
