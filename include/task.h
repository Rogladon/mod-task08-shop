#include <deque>
#include <thread>
#include <vector>
#include <future>


class Client {
public:
    int id;
    int productCount;
    Client(int id, int productsCount);
};

class Server {
    struct Statistics
    {
        int dequeLenght = 0;
        int waitClient = 0;
        int customersServed = 0;
        int customersRejected = 0;
        int threadWorking = 0;
        int threadPlain = 0;
        int observationsCount = 0;
    };
    class CashBox {
        int currentClientId = -1;
        std::future<void> thrd;
        int processTime;
        void process(int productCount);
    public:
        Statistics* stats;
        CashBox(int processTime, Statistics& stats);
        bool in_use = false;
        void start_processing(Client client);
    };
    void monitoring();
    std::deque<Client> dequeClient;
    int dequeMaxSize;
    std::vector<CashBox> cashboxPool;
    int monitoringInterval;
    std::thread mainThread;
    std::thread monitoringThread;
    bool isRunning = false;
public:
    void Run();
public:
    void addToDequeue(Client client);
    Server(int threadsCount, int queueSize, int processTime);
    Statistics stats;
    void Start();
    void Stop();
};