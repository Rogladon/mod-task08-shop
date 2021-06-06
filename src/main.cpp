#include <thread>
#include <cstdlib>
#include "../include/task.h"
#include <iostream>

int main(int argc, char* argv[]) {
    int totalTime = 10000;
    double clientIntensivity = (double)1 / 10;
    int oneTimePeriod = 10;
    int curTime;
    int clientCount = 0;
    int meanProducts = 10;
    int threadsCount = 5;
    int dequeSize = 10;
    int productProcessTime = 10;
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    Server server = Server(threadsCount, dequeSize, productProcessTime);
    server.Start();
    int clients_per_time_unit;
    int productCount;
    while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - begin).count() < totalTime) {
        clients_per_time_unit = rand() % int(2 * oneTimePeriod * clientIntensivity);
        for (int i = 0; i < clients_per_time_unit; i++) {
            productCount = rand() % (2 * meanProducts);
            Client client = Client(clientCount, productCount);
            server.addToDequeue(client);
            clientCount++;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(oneTimePeriod));
    }
    server.Stop();
    std::cout << "Customers served: " << server.stats.customersServed << " Customers rejected: " << server.stats.customersRejected << "\n";
    std::cout << "Mean deque length: " << server.stats.dequeLenght / server.stats.observationsCount << "\n";
    std::cout << "Threads working time: " << server.stats.threadWorking << " Threads plain time: " << server.stats.threadPlain << "\n";
    std::cout << "Mean client wait: " << server.stats.waitClient / server.stats.customersServed << "\n";
    std::cout << "Rejection probability: " << (double)server.stats.customersRejected / (server.stats.customersServed + server.stats.customersRejected) << "\n";
    std::cout << "Relative capacity: " << 1 - (double)server.stats.customersRejected / (server.stats.customersServed + server.stats.customersRejected) << "\n";
    std::cout << "Absolute capacity: " << clientIntensivity * (1 - (double)server.stats.customersRejected / (server.stats.customersServed + server.stats.customersRejected)) << std::endl;
    return 0;
}