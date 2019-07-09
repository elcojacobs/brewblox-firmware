
#include "../Connections.h"
class USBSerial;
class TCPClient;

namespace cbox {

template <>
DataIn::StreamType
StreamDataIn<USBSerial>::streamTypeImpl()
{
    return DataIn::StreamType::Serial;
}

template <>
DataIn::StreamType
StreamDataIn<TCPClient>::streamTypeImpl()
{
    return DataIn::StreamType::Tcp;
}

} // end namespace cbox