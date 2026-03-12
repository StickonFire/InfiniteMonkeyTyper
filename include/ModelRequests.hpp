#include "Model.hpp"

/**
 * Virtual Class representing a request from the Controller to the Model.
 * The dispatch function is used to tell the Model exactly what to do when it consumes the request.
 */
class ModelRequest {
    public:
        virtual void dispatch(AbstractModel *target) = 0;
};

class PauseRequest: public ModelRequest {
    unsigned int id;
    public:
        PauseRequest(unsigned int id);
        void dispatch(AbstractModel *target) override;
};

class UnpauseRequest: public ModelRequest {
    unsigned int id;
    public:
        UnpauseRequest(unsigned int id);
        void dispatch(AbstractModel *target) override;
};

class RemoveRequest: public ModelRequest {
    unsigned int id;
    public:
        RemoveRequest(unsigned int id);
        void dispatch(AbstractModel *target) override;
};

class CreateRequest: public ModelRequest {
    unsigned int seed;
    std::string query;

    public:
        CreateRequest(std::string query, unsigned int seed);
        void dispatch(AbstractModel *target) override;
};