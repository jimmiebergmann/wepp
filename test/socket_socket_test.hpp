#include "gtest/gtest.h"
#include "wepp/socket/socket.hpp"
#include "wepp/socket/tcpSocket.hpp"
#include "wepp/socket/tcpListener.hpp"
#include <thread>

using namespace Wepp;


TEST(Socket, Socket)
{
   {
        EXPECT_EQ(Socket::Socket::getLastError(), 0);
        Socket::Socket::setLastError(10);
        EXPECT_EQ(Socket::Socket::getLastError(), 10);
        Socket::Socket::setLastError(0);
        EXPECT_EQ(Socket::Socket::getLastError(), 0);
    }
    {
        Socket::Socket socket;
        EXPECT_EQ(socket.handle(), Socket::Socket::Handle(0));

        socket = Socket::Socket::Handle(1);
        EXPECT_EQ(socket.handle(), Socket::Socket::Handle(1));

        socket = Socket::Socket::Handle(0); // Required, else linux build will crash at socket destruction.
    }
    {
        Socket::Socket socket(Socket::Socket::Handle(2));
        EXPECT_EQ(socket.handle(), Socket::Socket::Handle(2));

        socket = Socket::Socket::Handle(0); // Required, else linux build will crash at socket destruction.
    }
}

TEST(Socket, TcpSocket)
{
    const uint16_t port = 54345;

    {
        Socket::TcpListener listener;
        EXPECT_TRUE(listener.start(port).wait().successful());

        Socket::TcpSocket socket;
        EXPECT_FALSE(socket.connect("0.1.2.3", port));
        EXPECT_TRUE(socket.connect("127.0.0.1", port));
    }
}