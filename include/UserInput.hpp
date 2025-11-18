#ifndef USERINPUT
#define USERINPUT

struct UserInput { };

struct PauseUserInput : public UserInput{
    unsigned int id;
};

#endif