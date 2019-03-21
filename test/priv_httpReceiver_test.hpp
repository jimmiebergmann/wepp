#include "gtest/gtest.h"
#include "wepp/priv/httpReceiver.hpp"
#include "wepp/semaphore.hpp"


using namespace Wepp;

TEST(Http_HttpReceiverBuffer, Constructing)
{
    {
        const size_t size1 = 512, size2 = 1024;
        Priv::HttpReceiverBuffer buffer1(size1);
        Priv::HttpReceiverBuffer buffer2(size2);
        EXPECT_EQ(buffer1.unreadBytes(), 0);
        EXPECT_EQ(buffer2.unreadBytes(), 0);
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
        EXPECT_EQ(buffer.unreadBytes(), 0);
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

        EXPECT_EQ(buffer.readLine(output), Priv::HttpReceiverBuffer::FindResult::NotFound);
    }
}

TEST(Http_HttpReceiverBuffer, Newline)
{
    {
        const size_t bufferSize = 1024;
        Priv::HttpReceiverBuffer buffer(bufferSize);
        EXPECT_EQ(buffer.unreadBytes(), 0);
        std::string output = "";

        EXPECT_TRUE(buffer.receive("foo"));
        EXPECT_EQ(buffer.unreadBytes(), 3);
        EXPECT_TRUE(buffer.receive(" bar"));
        EXPECT_EQ(buffer.unreadBytes(), 7);

        EXPECT_EQ(buffer.readLine(output), Priv::HttpReceiverBuffer::FindResult::NotFound);
        EXPECT_EQ(buffer.unreadBytes(), 7);
        EXPECT_STRNE(output.c_str(), "foo bar");

        EXPECT_TRUE(buffer.receive("\r\n"));
        EXPECT_EQ(buffer.unreadBytes(), 9);

        EXPECT_EQ(buffer.readLine(output), Priv::HttpReceiverBuffer::FindResult::Found);
        EXPECT_EQ(buffer.unreadBytes(), 0);
        EXPECT_STREQ(output.c_str(), "foo bar");
    }
    {
        const size_t bufferSize = 1;
        Priv::HttpReceiverBuffer buffer(bufferSize);
        EXPECT_EQ(buffer.unreadBytes(), 0);

        EXPECT_FALSE(buffer.receive("\r\n"));
        EXPECT_EQ(buffer.unreadBytes(), 0);
        EXPECT_EQ(buffer.readNewline(), Priv::HttpReceiverBuffer::FindResult::NotFound);
        EXPECT_EQ(buffer.unreadBytes(), 0);
    }
    {
        const size_t bufferSize = 2;
        Priv::HttpReceiverBuffer buffer(bufferSize);
        EXPECT_EQ(buffer.unreadBytes(), 0);

        EXPECT_TRUE(buffer.receive("\r\n"));
        EXPECT_EQ(buffer.unreadBytes(), 2);
        EXPECT_EQ(buffer.readNewline(), Priv::HttpReceiverBuffer::FindResult::Found);
        EXPECT_EQ(buffer.unreadBytes(), 0);
    }
    {
        const size_t bufferSize = 8;
        Priv::HttpReceiverBuffer buffer(bufferSize);
        EXPECT_EQ(buffer.unreadBytes(), 0);

        EXPECT_TRUE(buffer.receive("\r\n"));
        EXPECT_EQ(buffer.unreadBytes(), 2);
        EXPECT_TRUE(buffer.receive("\r\n"));
        EXPECT_EQ(buffer.unreadBytes(), 4);
        EXPECT_TRUE(buffer.receive("\r\n"));
        EXPECT_EQ(buffer.unreadBytes(), 6);
        EXPECT_TRUE(buffer.receive("\r\n"));
        EXPECT_EQ(buffer.unreadBytes(), 8);
        EXPECT_FALSE(buffer.receive("\r\n"));
        EXPECT_EQ(buffer.unreadBytes(), 8);

        EXPECT_EQ(buffer.readNewline(), Priv::HttpReceiverBuffer::FindResult::Found);
        EXPECT_EQ(buffer.unreadBytes(), 6);
        EXPECT_EQ(buffer.readNewline(), Priv::HttpReceiverBuffer::FindResult::Found);
        EXPECT_EQ(buffer.unreadBytes(), 4);
        EXPECT_EQ(buffer.readNewline(), Priv::HttpReceiverBuffer::FindResult::Found);
        EXPECT_EQ(buffer.unreadBytes(), 2);
        EXPECT_EQ(buffer.readNewline(), Priv::HttpReceiverBuffer::FindResult::Found);
        EXPECT_EQ(buffer.unreadBytes(), 0);
        EXPECT_EQ(buffer.readNewline(), Priv::HttpReceiverBuffer::FindResult::NotFound);
        EXPECT_EQ(buffer.unreadBytes(), 0);
    }
}

TEST(Http_HttpReceiverBuffer, MakingSpace)
{
    {
        Priv::HttpReceiverBuffer buffer(32);
        EXPECT_EQ(buffer.unreadBytes(), 0);

        const std::string firstString  = "First test string";
        const std::string secondString = "Second string is too long";
        std::string output = "";

        EXPECT_TRUE(buffer.receive(firstString + "\r\n"));
        EXPECT_EQ(buffer.unreadBytes(), firstString.size() + 2);

        EXPECT_EQ(buffer.readLine(output), Priv::HttpReceiverBuffer::FindResult::Found);
        EXPECT_EQ(buffer.unreadBytes(), 0);
        EXPECT_STREQ(output.c_str(), firstString.c_str());

        EXPECT_TRUE(buffer.receive(secondString + "\r\n"));
        EXPECT_EQ(buffer.unreadBytes(), secondString.size() + 2);

        EXPECT_EQ(buffer.readLine(output), Priv::HttpReceiverBuffer::FindResult::Found);
        EXPECT_EQ(buffer.unreadBytes(), 0);
        EXPECT_STREQ(output.c_str(), secondString.c_str());
    }
}
/*
TEST(Http_HttpReceiverBuffer, RegexSearch)
{
    {


    }
}*/