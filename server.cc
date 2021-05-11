#include <SDL2/SDL.h>

#include <iostream>
#include <map>
#include <random>
#include <sstream>

#include "server.h"
#include "util.h"

class GameServer : public Server {
  public:
    GameServer() : Server(8099), timer_(0), rng_(Util::random_seed()) {
      std::cerr << "Started game server on localhost:8099" << std::endl;

      std::uniform_real_distribution<double> d(1, 25);
      stonks_.insert(std::make_pair("GOOG", d(rng_)));
      stonks_.insert(std::make_pair("APPL", d(rng_)));
      stonks_.insert(std::make_pair("DOGE", d(rng_)));
      stonks_.insert(std::make_pair("ETH", d(rng_)));
      stonks_.insert(std::make_pair("BTC", d(rng_)));
    }

    void shutdown() override {
      std::cerr << "Shutting down" << std::endl;
    }

    void receive(Socket& client, const Packet& p) override {
      std::istringstream input(reinterpret_cast<const char *>(p.data));
      std::ostringstream output;

      std::string word, symbol;
      int count;

      input >> word >> symbol;

      if (word == "price") {
        output << symbol << " " << stonks_[symbol] << std::endl;
      } else if (word == "buy") {
        input >> count;
        double value = buy(clients_[client], symbol, count);
        output << value << std::endl;
      } else if (word == "sell") {
        input >> count;
        double value = sell(clients_[client], symbol, count);
        output << value << std::endl;
      }

      client.send(output.str());
    }

    void connect(Socket& client) override {
      std::cerr << "New connection!" << std::endl;
      Client jamoke;
      clients_.insert(std::make_pair(client, jamoke));

      client.send("Welcome to stonks market, you have $100\n");
    }

    void disconnect(Socket& client) override {
      std::cerr << "Lost connection." << std::endl;
      clients_.erase(client);
    }

    void update(unsigned int elapsed) override {
      timer_ += elapsed;

      std::normal_distribution<double> d(1, 0.05);

      if (timer_ > 1000) {
        for (auto& s: stonks_) {
          const double change = d(rng_);
          s.second *= change;
          std::cerr << s.first << ":" << s.second << " ";
        }

        timer_ -= 1000;
        std::cerr << std::endl;
      }
    }

  private:

    struct Client {
      Client() : cash(100.0) {}
      double cash;
      std::map<std::string, int> shares_;
    };

    double buy(Client& client, const std::string& symbol, int count) {
      if (stonks_.count(symbol) == 0) return 0;

      const double cost = stonks_[symbol] * count;
      if (client.cash > cost) {
        client.cash -= cost;
        client.shares_[symbol] += count;
        return cost;
      } else {
        return 0;
      }
    }

    double sell(Client& client, const std::string& symbol, int count) {
      if (client.shares_.count(symbol) == 0) return 0;

      if (client.shares_[symbol] >= count) {
        const double value = count * stonks_[symbol];
        client.shares_[symbol] -= count;
        client.cash += value;
        return value;
      } else {
        return 0;
      }
    }

    unsigned int timer_;
    std::mt19937 rng_;
    std::map<std::string, double> stonks_;
    std::map<Socket, Client> clients_;
};

int main(int, char**) {
  SDL_Init(SDL_INIT_TIMER | SDL_INIT_EVENTS);

  GameServer server;
  server.loop();

  return 0;
}
