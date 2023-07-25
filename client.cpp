#include <iostream>
#include <thread>
#include <cstdlib>
#include <ctime>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "ThreadSafeQueue.h"

using namespace std;

void generate(ThreadSafeQueue &q)
{
  srand((unsigned)time(NULL));

  clock_t t1 = clock();
  double deltaT = 0;
  const double DELAY_IN_SEC = 5;

  while (1)
  {
    deltaT = (clock() - t1) / CLOCKS_PER_SEC;

    if (deltaT > DELAY_IN_SEC)
    {
      int number = rand();

      cout << "\n Generated Number = " << number << endl;

      q.push(number);

      t1 = clock();
      deltaT = 0;
    }
  }
}

void receive(ThreadSafeQueue &q, int client_sock)
{
  while (1)
  {
    int number = q.pop();

    cout << "\n Received Number = " << number << endl;

    int converted_number = htonl(number);

    write(client_sock, &converted_number, sizeof(converted_number));

    int processed_number;
    read(client_sock, &processed_number, sizeof(processed_number));

    processed_number = ntohl(processed_number);

    cout << "\n Processed Number = " << processed_number << endl;
  }
}

int main()
{
  int client_socket = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(8080);
  inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr);
  if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
  {
    cerr << "Failed to connect to server." << endl;
    close(client_socket);
    return 1;
  }

  ThreadSafeQueue q;

  thread generatorThread(generate, ref(q));
  thread receiverThread(receive, ref(q), client_socket);

  generatorThread.join();
  receiverThread.join();

  close(client_socket);
  return 0;
}