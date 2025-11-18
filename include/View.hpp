#ifndef VIEW
#define VIEW

class View {
    virtual ~View() = default;
    virtual void receiveUpdate();
};

#endif