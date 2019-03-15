#include <core/component.h>
#include <core/ports/periodicport.h>
#include <nodemanager/deploymentcontainer.h>
#include <proto/controlmessage/controlmessage.pb.h>

Component* component_ = 0;
//std::shared_ptr<Component> component;
//std::shared_ptr<PeriodicPort> periodic_port;
void callback_fn(){
    std::cerr << "TICK" << std::endl;
    int i = 0;

    while(i ++ < 5){
        std::cerr << i << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));   
        auto port = component_->GetPort("periodic2").lock();
        if(port){
            std::cerr << port->get_name();
        }
    }
}

void callback_fn2(){
    std::cerr << "TICK2" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(10000));   
    std::cerr << "!TICK2" << std::endl;
}

int main(){
    BaseMessage m;
    NodeManager::Container container;
    container.mutable_info()->set_name("test");
    {
        DeploymentContainer dc("test", "test", ".", container);
        auto component = dc.AddComponent(std::unique_ptr<Component>(new Component("Test")), "test_component").lock();
        component->RegisterPeriodicCallback("periodic", callback_fn);
        component->RegisterPeriodicCallback("periodic2", callback_fn2);
        dc.ConstructPeriodicPort(component, "periodic");
        dc.ConstructPeriodicPort(component, "periodic2");
        {
            auto periodic_port = component->GetTypedPort<PeriodicPort>("periodic");
            periodic_port->SetFrequency(1);
            periodic_port->Configure();
        }
        
        component->Configure();
        component_ = component.get();

        dc.Configure(container);
        dc.Activate();
        
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        std::cerr << "Passivate" << std::endl;
        dc.Passivate();
        std::cerr << "Passivated" << std::endl;
        std::cerr << "Terminate" << std::endl;
        dc.Terminate();
    }
    std::cerr << "Terminated" << std::endl;

    return 0;
}

