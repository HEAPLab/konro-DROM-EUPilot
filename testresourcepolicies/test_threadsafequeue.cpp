#include "unittest.h"
#include "threadsafequeue.h"
#include <iostream>

using namespace std;

static int testThreadsafeQueue1()
{
    ThreadsafeQueue<int> tsQueue;
    tsQueue.push(1);
    tsQueue.push(2);
    tsQueue.push(3);

    int val;
    if (!tsQueue.tryPop(val) || val != 1)
        return TEST_FAILED;
    if (!tsQueue.tryPop(val) || val != 2)
        return TEST_FAILED;
    if (!tsQueue.tryPop(val) || val != 3)
        return TEST_FAILED;
    if (!tsQueue.empty())
        return TEST_FAILED;
    return TEST_OK;
}

static int testThreadsafeQueue2()
{
    ThreadsafeQueue<int> tsQueue;
    int val;
    if (tsQueue.waitAndPop(val, std::chrono::milliseconds(100)))
        return TEST_FAILED;
    return TEST_OK;
}

int main(int argc, char *argv[])
{
    if (testThreadsafeQueue1() != TEST_OK) return TEST_FAILED;
    if (testThreadsafeQueue2() != TEST_OK) return TEST_FAILED;

   return TEST_OK;
}
