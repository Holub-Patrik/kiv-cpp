#include <iostream>
#include <string>
#include <vector>

constexpr int RECORD_COUNT = 100;
constexpr int RECORD_COUNT_NEW = 1000;

class TRecord {
public:
  TRecord(int index, double value) : index{index}, value{value} {};
  ~TRecord(){};
  int index;
  double value;
};

void fill_records(std::vector<TRecord> &recs, int count) {
  for (int i = 0; i < count; i++) {
    TRecord tmp = TRecord(i, static_cast<double>((i * 19) % 29) * 0.1854);
    recs.push_back(tmp);
  }
}

double calculate_average(std::vector<TRecord> &recs) {
  double avg = 0;
  for (TRecord &rec : recs)
    avg += rec.value;
  avg /= recs.size();
  return avg;
}

int main(int argc, char **argv) {
  std::vector<TRecord> recs = {};
  double avg;

  fill_records(recs, RECORD_COUNT);

  avg = calculate_average(recs);
  std::cout << "Prumer celeho pole je: " << std::to_string(avg) << std::endl;

  // calculate average of first half
  {
    std::vector<TRecord> firt_half =
        std::vector<TRecord>(recs.begin(), recs.end() - recs.size() / 2);
    avg = calculate_average(firt_half);

    std::cout << "Prumer prvni poloviny pole je: " << std::to_string(avg)
              << std::endl;
  }
  // calculate average of second half
  {
    std::vector<TRecord> second_half =
        std::vector<TRecord>(recs.begin() + recs.size() / 2, recs.end());
    avg = calculate_average(second_half);

    std::cout << "Prumer druhe poloviny pole je: " << std::to_string(avg)
              << std::endl;
  }
  // increase record count

  fill_records(recs, RECORD_COUNT_NEW - RECORD_COUNT);

  avg = calculate_average(recs);
  std::cout << "Prumer celeho pole je: " << std::to_string(avg) << std::endl;

  // calculate average of first half
  {
    std::vector<TRecord> firt_half =
        std::vector<TRecord>(recs.begin(), recs.end() - recs.size() / 2);
    avg = calculate_average(firt_half);

    std::cout << "Prumer prvni pulky pole je: " << std::to_string(avg)
              << std::endl;
  }
  // calculate average of second half
  {
    std::vector<TRecord> second_half =
        std::vector<TRecord>(recs.begin() + recs.size() / 2, recs.end());
    avg = calculate_average(second_half);

    std::cout << "Prumer druhe poloviny pole je: " << std::to_string(avg)
              << std::endl;
  }
}
