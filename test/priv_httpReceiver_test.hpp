#include "test.hpp"
#include "wepp/priv/httpReceiver.hpp"
#include "wepp/socket/tcpListener.hpp"

using namespace Wepp;

TEST(Http_HttpReceiverBuffer, Constructing)
{
    {
        const size_t size1 = 512, size2 = 1024;
        Priv::HttpReceiverBuffer buffer1(size1);
        Priv::HttpReceiverBuffer buffer2(size2);
        EXPECT_EQ(buffer1.unreadBytes(), size_t(0));
        EXPECT_EQ(buffer2.unreadBytes(), size_t(0));
        EXPECT_EQ(buffer1.size(), size1);
        EXPECT_NE(buffer1.size(), size1 + 1);
        EXPECT_EQ(buffer2.size(), size2);
        EXPECT_NE(buffer2.size(), size2 + 1);
    }
}

TEST(Http_HttpReceiverBuffer, ReceiveString)
{
    {
        const size_t bufferSize = 1024;
        Priv::HttpReceiverBuffer buffer(bufferSize);
        EXPECT_EQ(buffer.unreadBytes(), size_t(0));
        std::string output = "";

        EXPECT_TRUE(buffer.receive("foo"));
        EXPECT_TRUE(buffer.receive(" bar"));
        EXPECT_TRUE(buffer.receive("\r\n"));

        EXPECT_TRUE(buffer.receive("hello"));
        EXPECT_TRUE(buffer.receive(" world"));
        EXPECT_TRUE(buffer.receive("\r\n"));

        EXPECT_TRUE(buffer.receive("cool"));

        EXPECT_EQ(buffer.readLine(output), Priv::HttpReceiverBuffer::FindResult::Found);
        EXPECT_STREQ(output.c_str(), "foo bar");

        EXPECT_EQ(buffer.readLine(output), Priv::HttpReceiverBuffer::FindResult::Found);
        EXPECT_STREQ(output.c_str(), "hello world");

        EXPECT_EQ(buffer.readLine(output), Priv::HttpReceiverBuffer::FindResult::NewlineNotFound);
    }
}

TEST(Http_HttpReceiverBuffer, ReceiveSocket)
{
    {
        const size_t bufferSize = 1024;
        Priv::HttpReceiverBuffer buffer(bufferSize);
        EXPECT_EQ(buffer.unreadBytes(), size_t(0));
        
        const uint16_t port = 7655;
        Socket::TcpListener listener;
        EXPECT_TRUE(listener.start(port).wait().successful());
        auto listenTask = listener.listen();

        Socket::TcpSocket client;
        EXPECT_TRUE(client.connect("127.0.0.1", port));
        EXPECT_TRUE(listenTask.wait().successful());
        auto peer = listenTask();

        const std::string line1 = "Hello world.";
        const std::string line2 = "Second line!";
        const std::string sendData = line1 + "\r\n" + line2 + "\r\n";

        EXPECT_EQ(client.send(sendData), int(sendData.size()));
        EXPECT_EQ(buffer.receive(*peer), int(sendData.size()));
        EXPECT_EQ(buffer.unreadBytes(), size_t(sendData.size()));
        
        std::string readLine = "";
        EXPECT_EQ(buffer.readLine(readLine), Priv::HttpReceiverBuffer::FindResult::Found);
        EXPECT_STREQ(readLine.c_str(), line1.c_str());
        EXPECT_EQ(buffer.readLine(readLine), Priv::HttpReceiverBuffer::FindResult::Found);
        EXPECT_STREQ(readLine.c_str(), line2.c_str());
        EXPECT_EQ(buffer.unreadBytes(), size_t(0));
    }
}

TEST(Http_HttpReceiverBuffer, Newline)
{
   {
        const size_t bufferSize = 1024;
        Priv::HttpReceiverBuffer buffer(bufferSize);
        EXPECT_EQ(buffer.unreadBytes(), size_t(0));
        std::string output = "";
        
        EXPECT_TRUE(buffer.receive("foo"));
        EXPECT_EQ(buffer.unreadBytes(), size_t(3));
        EXPECT_TRUE(buffer.receive(" bar"));
        EXPECT_EQ(buffer.unreadBytes(), size_t(7));
        
        EXPECT_EQ(buffer.readLine(output), Priv::HttpReceiverBuffer::FindResult::NewlineNotFound);
        EXPECT_EQ(buffer.unreadBytes(), size_t(7));
        EXPECT_STRNE(output.c_str(), "foo bar");

        EXPECT_TRUE(buffer.receive("\r\n"));
        EXPECT_EQ(buffer.unreadBytes(), size_t(9));

        EXPECT_EQ(buffer.readLine(output), Priv::HttpReceiverBuffer::FindResult::Found);
        EXPECT_EQ(buffer.unreadBytes(), size_t(0));
        EXPECT_STREQ(output.c_str(), "foo bar");
    }
    {
        const size_t bufferSize = 1;
        Priv::HttpReceiverBuffer buffer(bufferSize);
        EXPECT_EQ(buffer.unreadBytes(), size_t(0));

        EXPECT_FALSE(buffer.receive("\r\n"));
        EXPECT_EQ(buffer.unreadBytes(), size_t(0));
        EXPECT_EQ(buffer.readNewline(), Priv::HttpReceiverBuffer::FindResult::NewlineNotFound);
        EXPECT_EQ(buffer.unreadBytes(), size_t(0));
    }
    {
        const size_t bufferSize = 2;
        Priv::HttpReceiverBuffer buffer(bufferSize);
        EXPECT_EQ(buffer.unreadBytes(), size_t(0));

        EXPECT_TRUE(buffer.receive("\r\n"));
        EXPECT_EQ(buffer.unreadBytes(), size_t(2));
        EXPECT_EQ(buffer.readNewline(), Priv::HttpReceiverBuffer::FindResult::Found);
        EXPECT_EQ(buffer.unreadBytes(), size_t(0));
    }
    {
        const size_t bufferSize = 8;
        Priv::HttpReceiverBuffer buffer(bufferSize);
        EXPECT_EQ(buffer.unreadBytes(), size_t(0));

        EXPECT_TRUE(buffer.receive("\r\n"));
        EXPECT_EQ(buffer.unreadBytes(), size_t(2));
        EXPECT_TRUE(buffer.receive("\r\n"));
        EXPECT_EQ(buffer.unreadBytes(), size_t(4));
        EXPECT_TRUE(buffer.receive("\r\n"));
        EXPECT_EQ(buffer.unreadBytes(), size_t(6));
        EXPECT_TRUE(buffer.receive("\r\n"));
        EXPECT_EQ(buffer.unreadBytes(), size_t(8));
        EXPECT_FALSE(buffer.receive("\r\n"));
        EXPECT_EQ(buffer.unreadBytes(), size_t(8));

        EXPECT_EQ(buffer.readNewline(), Priv::HttpReceiverBuffer::FindResult::Found);
        EXPECT_EQ(buffer.unreadBytes(), size_t(6));
        EXPECT_EQ(buffer.readNewline(), Priv::HttpReceiverBuffer::FindResult::Found);
        EXPECT_EQ(buffer.unreadBytes(), size_t(4));
        EXPECT_EQ(buffer.readNewline(), Priv::HttpReceiverBuffer::FindResult::Found);
        EXPECT_EQ(buffer.unreadBytes(), size_t(2));
        EXPECT_EQ(buffer.readNewline(), Priv::HttpReceiverBuffer::FindResult::Found);
        EXPECT_EQ(buffer.unreadBytes(), size_t(0));
        EXPECT_EQ(buffer.readNewline(), Priv::HttpReceiverBuffer::FindResult::NewlineNotFound);
        EXPECT_EQ(buffer.unreadBytes(), size_t(0));
    }
}

TEST(Http_HttpReceiverBuffer, MakingSpace)
{
    {
        Priv::HttpReceiverBuffer buffer(32);
        EXPECT_EQ(buffer.unreadBytes(), size_t(0));

        const std::string firstString = "First test string";
        const std::string secondString = "Second string is too long";
        std::string output = "";

        EXPECT_TRUE(buffer.receive(firstString + "\r\n"));
        EXPECT_EQ(buffer.unreadBytes(), firstString.size() + 2);

        EXPECT_EQ(buffer.readLine(output), Priv::HttpReceiverBuffer::FindResult::Found);
        EXPECT_EQ(buffer.unreadBytes(), size_t(0));
        EXPECT_STREQ(output.c_str(), firstString.c_str());

        EXPECT_TRUE(buffer.receive(secondString + "\r\n"));
        EXPECT_EQ(buffer.unreadBytes(), secondString.size() + 2);

        EXPECT_EQ(buffer.readLine(output), Priv::HttpReceiverBuffer::FindResult::Found);
        EXPECT_EQ(buffer.unreadBytes(), size_t(0));
        EXPECT_STREQ(output.c_str(), secondString.c_str());
    }
}

TEST(Http_HttpReceiverBuffer, ReadAll)
{
    {
        Priv::HttpReceiverBuffer buffer(64);
        EXPECT_EQ(buffer.unreadBytes(), size_t(0));

        const std::string data = "Hello world, this is a test.";

        for (size_t i = 0; i < 3; i++)
        {
            GTEST_PRINT("Loop: " << i);
            buffer.receive(data);
            EXPECT_EQ(buffer.unreadBytes(), data.size());

            std::string readData = "";
            EXPECT_EQ(buffer.readAll(readData), data.size());
            EXPECT_STREQ(readData.c_str(), data.c_str());
            EXPECT_EQ(buffer.unreadBytes(), size_t(0));
        }
    }
}


/*
TEST(Http_HttpReceiverBuffer, RegexSearch)
{
    {


    }
}*/