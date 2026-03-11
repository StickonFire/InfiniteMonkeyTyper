#include "Model.hpp"

class ModelRequest {
    public:
        virtual void dispatch(Model *target) = 0;
};

class PauseRequest: public ModelRequest {
    unsigned int id;
    public:
        PauseRequest(unsigned int id);
        void dispatch(Model *target) override;
};

class UnpauseRequest: public ModelRequest {
    unsigned int id;
    public:
        UnpauseRequest(unsigned int id);
        void dispatch(Model *target) override;
};

class RemoveRequest: public ModelRequest {
    unsigned int id;
    public:
        RemoveRequest(unsigned int id);
        void dispatch(Model *target) override;
};

class CreateRequest: public ModelRequest {
    unsigned int seed;
    std::string query;

    public:
        CreateRequest(std::string query, unsigned int seed);
        void dispatch(Model *target) override;
};