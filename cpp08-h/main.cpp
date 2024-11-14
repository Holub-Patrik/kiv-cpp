#include <array>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

// Pro ted nereste, zda je v roure dost mista - to by nahravalo na implementaci
// semaforu a to ted nechceme (semafor - viz ZOS, PPR, UPP, ...)

std::mutex mtx{};
std::condition_variable cv{};
bool working = false;

class StupidPipe final {
public:
  StupidPipe() {
    for (size_t i = 0; i < mBuffer.size(); i++) {
      mBuffer[i] = 0;
    }
  }

  void Write(char c) {
    std::unique_lock lck{mtx};

    cv.wait(lck, []() { return !working; });
    working = true;

    mBuffer[mWrite_Cursor] = c;
    mWrite_Cursor = (mWrite_Cursor + 1) % mBuffer.size();

    lck.unlock();
    working = false;
    cv.notify_all();
  }

  char Read(size_t delay) {
    std::unique_lock lck{mtx};
    cv.wait(lck, []() { return !working; });
    working = true;

    char c = mBuffer[mRead_Cursor];
    mBuffer[mRead_Cursor] = 0;
    mRead_Cursor = (mRead_Cursor + 1) % mBuffer.size();

    lck.unlock();
    working = false;
    cv.notify_all();

    if (!c) {
      std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    }
    return c;
  }

private:
  std::array<char, 128> mBuffer;
  size_t mWrite_Cursor = 0;
  size_t mRead_Cursor = 0;
};

void Producent_Thread(StupidPipe& r, char c, size_t delay) {
  {
    std::lock_guard<std::mutex> lck{mtx};

    std::cout << "Startuje producent '" << c << "'" << std::endl;
  }
  while (true) {
    r.Write(c);
    std::this_thread::sleep_for(std::chrono::milliseconds(delay));
  }
}

void Konzument_Thread(StupidPipe& r, size_t delay) {
  {
    std::lock_guard<std::mutex> lck{mtx};

    std::cout << "Startuje konzument" << std::endl;
  }

  while (true) {
    char c = r.Read(delay);

    std::cout << c;
    std::cout.flush();
  }
}

int main(int argc, char** argv) {
  StupidPipe roura{};

  std::thread prod1(&Producent_Thread, std::ref(roura), 'a', 1500);
  std::thread prod2(&Producent_Thread, std::ref(roura), 'b', 1200);
  std::thread prod3(&Producent_Thread, std::ref(roura), 'c', 2300);

  std::thread konz(&Konzument_Thread, std::ref(roura), 300);

  prod1.join();
  prod2.join();
  prod3.join();
  konz.join();

  return 0;
}
