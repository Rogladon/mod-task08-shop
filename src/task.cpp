#include "../include/task.h"
#include <iostream>

Server::Server(int threadsCount, int queueSize, int processTime) {
    cashboxPool = std::vector<CashBox>();
    for (int i = 0; i < threadsCount; i++) {
        cashboxPool.push_back(CashBox(processTime, stats));
    }
    stats.waitClient = 0;
    stats.threadPlain = 0;
    stats.threadWorking = 0;
    dequeClient = std::deque<Client>();
    dequeMaxSize = queueSize;
}

void Server::Stop() {
    isRunning = false;
    stats.customersRejected += dequeClient.size();
    monitoringThread.join();
    mainThread.join();
}

void Server::monitoring() {
    while (isRunning) {
        for (int i = 0; i < cashboxPool.size(); i++) {
            if (cashboxPool[i].in_use) {
                stats.threadWorking += monitoringInterval;
            }
            else {
                stats.threadPlain += monitoringInterval;
            }
        }
        stats.waitClient += dequeClient.size() * monitoringInterval;
        stats.dequeLenght += dequeClient.size();
        stats.observationsCount++;
        std::this_thread::sleep_for(std::chrono::milliseconds(monitoringInterval));
    }
}

void Server::Run() {
    while (isRunning)
    {
        for (int i = 0; i < cashboxPool.size(); i++) {
            if (!cashboxPool[i].in_use && dequeClient.size() > 0) {
                cashboxPool[i].start_processing(dequeClient.front());
                dequeClient.pop_front();
            }
        }
    }
}

void Server::Start() {
    isRunning = true;
    monitoringThread = std::thread(&Server::monitoring, this);
    mainThread = std::thread(&Server::Run, this);
}

void Server::addToDequeue(Client client) {
    if (dequeMaxSize > dequeClient.size()) {
        dequeClient.push_back(client);
    }
    else {
        stats.customersRejected++;
    }
}

Server::CashBox::CashBox(int processTime, Statistics& stats) {
    this->stats = &stats;
    in_use = false;
    this->processTime = processTime;
}

void Server::CashBox::start_processing(Client client) {
    this->in_use = true;
    currentClientId = client.id;
    thrd = std::async(&Server::CashBox::process, this, client.productCount);
}

void Server::CashBox::process(int productCount) {
    for (int i = 0; i < productCount; i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(this->processTime));
    }
    //ToDo: thread lock on stats recording maybe (mutex?)
    this->stats->waitClient += productCount * this->processTime;
    this->stats->customersServed++;
    this->in_use = false;
}

Client::Client(int id, int productsCount) {
    this->id = id;
    this->productCount = productsCount;
}