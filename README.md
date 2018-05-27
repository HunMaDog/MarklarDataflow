# MarklarDataflow
Yet another data flow solution written in C++.

```cpp
    ...
    
    class HelloWorld
        : public Node<Name, Trigger>
    {
    public:
        HelloWorld(std::string const & name)
            : Node<Name, Trigger>(name)
        {
            set_trigger_function([this](){ queuing_task(); });
        }

    protected:
        void process() final
        {
            std::cout << "Hello world" << std::endl;
        }
    };
    
    ...
    
    int main()
    {
        HelloWorld hi("hello");
    
        hi.start();
        hi.trigger();
        hi.stop();
    
        return 0;
    }
```

# Feature
 - Thread pool
   + Can be adjusted separately to the nodes
 - Commands
   + Async executing
   + Custom commands can be created using public interface
 - Policy based node implementation
   + Identifier policy
   + Scheduler policy
   + Extender policy
   + Custom policy can be created
 - Pin
   + 1:N outbound connections
   + N:1 inbound connections
   + Flag based configuration
     - Accept - Accept incoming data
     - Publish - Publishing data towards the node
     - Fixed - Always publishing the last arrived data and it has only one at the same time
     - Optional - Not required for node's valid operation
     - Silent - Not use wake up signal
     - Single - Maximum one connection is allowed
     - Locked - Not accept any new connection
     - Can be modified while running
   + Can be added or removed while running
   + Custom pins can be created using the Pin asbtract class
 - Connection
   + Accept function connection
   + Can be separated from both sides
   + Connections can be added or removed while running
 - Flow control (aka scheduler policy)
   + Event driven
   + Data driven
   + Time driven
 - Log
   + Can be adjusted separately to the nodes and pins

# 3rd Party
 - WigWag
 - spdlog
 - Sole
 - Google Test

# Status
Not ready, but testable.

Work on
 - Exception handling
 - Log messages
 - Connection policy for common connection handling
 - Get owner Id from connection pool
 - Create signal and slot pins
 - More and better unit tests with fixtures
 - better code :)
