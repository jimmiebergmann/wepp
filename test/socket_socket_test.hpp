#include "gtest/gtest.h"
#include "wepp/socket/socket.hpp"
#include "wepp/socket/tcpSocket.hpp"
#include "wepp/socket/tcpListener.hpp"
#include "wepp/socket/platform/socketFunctions.hpp"
#include <thread>

using namespace Wepp;


TEST(Socket, Handle)
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
        EXPECT_EQ(socket.getHandle(), Socket::Socket::Handle(0));

        socket.setHandle(1);
        EXPECT_EQ(socket.getHandle(), Socket::Socket::Handle(1));

        socket.releaseHandle(); // Required, else linux build will crash at socket destruction.
    }
    {
        Socket::Socket socket(2);
        EXPECT_EQ(socket.getHandle(), Socket::Socket::Handle(2));

        socket.releaseHandle(); // Required, else linux build will crash at socket destruction.
    }
    {
        Socket::Socket::Handle handle = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        EXPECT_TRUE(WeppIsSocketValid(handle));
        EXPECT_FALSE(WeppIsSocketInvalid(handle));
        if (WeppIsSocketValid(handle))
        {
            Socket::Socket socket(handle);
            EXPECT_EQ(socket.getHandle(), handle);
            socket.close();
            EXPECT_EQ(socket.getHandle(), Socket::Socket::Handle(0));
        }

    }
}

TEST(Socket, TcpSocket)
{
    const uint16_t port = 54345;

    {
        Socket::TcpListener listener;
        EXPECT_TRUE(listener.start(port).wait().successful());

        auto listenTask = listener.listen();

        Socket::TcpSocket socket;
        EXPECT_EQ(socket.connect("0.1.2.3", port), Socket::Socket::Status::Error);
        EXPECT_EQ(socket.connect("127.0.0.1", port), Socket::Socket::Status::Successful);

        EXPECT_TRUE(listenTask.wait().successful());
    }
}