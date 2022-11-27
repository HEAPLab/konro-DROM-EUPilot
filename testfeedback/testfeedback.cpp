#include <iostream>

#include "konrofeedback.h"
#include <iostream>
#include <thread>
#include <string>

using namespace std;

int main(int argc, char *argv[])
{
    bool val = true;
    KonroFeedback feedback(val);
    for (int i = 0; i < 10; ++i) {
        cout << "Sending message" << endl;
        string reply = feedback.send();
        cout << "Received reply: " << reply << endl;
        cout << "Sleeping" << endl;
        this_thread::sleep_for(chrono::seconds(6));
        val= !val;
        feedback.setFeedback(val);
    }
}
