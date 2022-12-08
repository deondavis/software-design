/* simple code to demonstrate future and promise construct in c++ 
   promise allows us to return state from a thread
   future is for reading that returned state
*/

#include <iostream>
#include <future>
#include <thread>

#ifdef VERSION_1

std::promise<std::string> producerObj;
std::future<std::string> consumerObj;

void producerFunction(void)
{
    std::cout<<"Inside producer"<<std::endl;

    producerObj.set_value("future is a promise");
}

void consumerFunction(void)
{
    std::cout<<"Inside consumer"<<std::endl;
    std::cout<<"consumer:"<<consumerObj.get()<<std::endl;
}

int main()
{
    std::cout<<"version 1"<<std::endl;
    std::thread producer (producerFunction);

    consumerObj =  producerObj.get_future();

    std::thread consumer (consumerFunction);

    producer.join();
    consumer.join();

    return 0;
}
#endif //VERSION_1

#ifdef VERSION_2
/* this version demonstrate pass by reference and how global variables be avoided*/

void producerFunction(std::promise<std::string> &producerObj)
{
    std::cout<<"Inside producer"<<std::endl;

    producerObj.set_value("future is a promise");
}

void consumerFunction(std::future<std::string> &consumerObj)
{
    std::cout<<"Inside consumer"<<std::endl;
    std::cout<<"consumer:"<<consumerObj.get()<<std::endl;
}

int main()
{
    std::cout<<"version 2"<<std::endl;

    std::promise<std::string> producerObj;
    std::thread producer (producerFunction,std::ref(producerObj));

    std::future<std::string> consumerObj =  producerObj.get_future();
    std::thread consumer (consumerFunction,std::ref(consumerObj));

    producer.join();
    consumer.join();

    return 0;
}
#endif //VERSION_2

#ifdef VERSION_3

/* using std::async 
   async will create the future and producer thread for us 
*/

std::string producerFunction()
{
    std::cout<<"Inside producer"<<std::endl;

    return "future is a promise";
}

void consumerFunction(std::future<std::string> &consumerObj)
{
    std::cout<<"Inside consumer"<<std::endl;
    std::cout<<"consumer:"<<consumerObj.get()<<std::endl;
}

int main()
{
    std::cout<<"version 3"<<std::endl;

    /* the return type of the function that async invokes should match that of fututre < tparams > type */ 
    std::future<std::string> consumerObj = std::async(producerFunction);

    std::thread consumer (consumerFunction,std::ref(consumerObj));

    consumer.join();

    return 0;
}

#endif //VERSION_3

#ifdef VERSION_4

/* using std::async 
   async will create the future and consumer thread for us 

   using lambdas for idiomatic code
*/

int main()
{
    std::cout<<"version 4"<<std::endl;

    /* lamda capture [&] implicitly captures the used automatic variables by reference
       in the given scope 
    */
    /* std::launch::deferred --> task is executed on the calling thread
       std::launch::async --> task is executed on a different thread
    */
    std::future<std::string> consumerObj = std::async(std::launch::deferred, [&]
                                                      { 
                                    std::cout<<"Inside producer"<<std::endl;
                                    return std::string("future is a promise"); });

    std::thread consumer([&]
                         { 
                          std::cout<<"Inside consumer"<<std::endl;
                          std::cout<<"consumer:"<<consumerObj.get()<<std::endl; });

    consumer.join();

    return 0;
}

#endif //VERSION_4

/* compilation tip:
   for version 1
   g++ -DVERSION_1 promise_future.cpp -o prod -lpthread
*/