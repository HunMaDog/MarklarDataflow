# MarklarDataflow

# Status
Big Changes Ahead
- refactoring to header only library
- policy based pin
- and lot of improvement

# Hello World
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

# 3rd Party
 - WigWag
 - spdlog
 - Sole
 - Google Test
