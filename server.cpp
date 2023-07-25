#include <iostream>
#include <thread>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "ThreadSafeQueue.h"

using namespace std;

void receive(ThreadSafeQueue &q, int client_sock)
{
  while (1)
  {
    int received_int = 0;

    if (read(client_sock, &received_int, sizeof(received_int)) > 0)
    {
      received_int = ntohl(received_int);
      cout << "Received number = " << received_int << endl;
    }
    q.push(received_int);
  }
}

void process(ThreadSafeQueue &q, ThreadSafeQueue &q2)
{
  while (1)
  {
    int number = q.pop();

    int processedNumber = number % 10;

    cout << "Processed Number = " << processedNumber << endl;

    q2.push(processedNumber);
  }
}

void sendNumber(ThreadSafeQueue &q2, int client_sock)
{
  while (1)
  {
    int num = q2.pop();

    int converted_num = htonl(num);

    write(client_sock, &converted_num, sizeof(converted_num));
  }
}

int main()
{
  int server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket == -1)
  {
    cerr << "Failed to create socket." << endl;
    return 1;
  }

  struct sockaddr_in server_address;

  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = INADDR_ANY;
  server_address.sin_port = htons(8080);

  if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
  {
    cerr << "Failed to bind socket." << endl;
    close(server_socket);
    return 1;
  }

  cout << "Opened Socket connection on port 8080" << endl;

  if (listen(server_socket, 3) == -1)
  {
    cerr << "Failed to listen for incoming connections." << endl;
    close(server_socket);
    return 1;
  }

  cout << "Listening to Connections!!!" << endl;

  int client_socket;
  struct sockaddr_in client_address;
  socklen_t addrlen = sizeof(client_address);
  client_socket = accept(server_socket, (struct sockaddr *)&client_address, &addrlen);
  if (client_socket == -1)
  {
    cerr << "Failed to accept incoming connection." << endl;
    close(server_socket);
    return 1;
  }
  cout << "Accepted Connection with ID = " << client_socket << endl;

  ThreadSafeQueue q, q2;

  thread receiverThread(receive, ref(q), client_socket);
  thread processorThread(process, ref(q), ref(q2));
  thread senderThread(sendNumber, ref(q2), client_socket);

  receiverThread.join();
  processorThread.join();
  senderThread.join();

  close(client_socket);
  close(server_socket);
  return 0;
}
