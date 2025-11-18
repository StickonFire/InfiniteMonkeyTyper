#ifndef CONTROLLER
#define CONTROLLER

#include "UserInput.hpp"
#include "View.hpp"
#include "Model.hpp"

#include <queue>

class Controller {
    std::queue<UserInput> inputQueue;
    View *view;
    Model *model;

    Controller(View *view, Model *model);

    public:
        /**
         * The main method 
         */
        void receiveModelInput(UserInput *input);

        void receiveViewInput(UserInput *input);
        /**
         * The main method for the controller, intended for use as a separate thread.
         * It runs an infinite loop with a body held in the function singleLoop.
         */
        void runLoop();

    private:

        /**
         * This function runs a predetermined amount of time. 
         * It checks the queue for inputs, processes them for the Model, runs the updated Model,
         * passes the results to the View, then waits to keep up with a set time for running.
         */
        void singleLoop();

        /**
         * A helper method to the receiveModelInput.
         * In the case of a 
         */
        void handleViewChange(UserInput *input);

        void generateViewLogic();

        /**
         * 
         */
        void notifyViews();
};
#endif