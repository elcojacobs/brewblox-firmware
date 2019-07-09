
#include "../Connections.h"
class USBSerial;
class TCPClient;

namespace cbox {

template <>
StreamType
StreamDataIn<USBSerial>::streamTypeImpl()
{
    return StreamType::Usb;
}

template <>
StreamType
StreamDataIn<TCPClient>::streamTypeImpl()
{
    return StreamType::Tcp;
}

} // end namespace cbox