#include <algorithm>
#include <concepts>
#include <exception>
#include <iostream>
#include <istream>
#include <optional>
#include <ostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <variant>

template <typename T>
concept DB_types = std::same_as<T, int> || std::same_as<T, double> ||
                   std::convertible_to<T, std::string>;

using DB_store_type = std::variant<int, double, std::string>;
class DB {
private:
  std::unordered_map<std::string, DB_store_type> m_store;

public:
  DB() = default;
  virtual ~DB() = default;

  DB(const DB& other) : m_store(other.m_store) {}
  DB(DB&& other) : m_store(std::move(other.m_store)) {}

  DB& operator=(const DB& other) {
    if (this == &other)
      return *this;

    DB temp(other);
    m_store = std::move(other.m_store);

    return *this;
  }

  DB& operator=(DB&& other) {
    DB temp(std::move(other));
    std::swap(m_store, temp.m_store);
    return *this;
  }

  template <DB_types T> std::optional<T> Get(std::string key) const {
    auto it = m_store.find(key);
    if (it != m_store.end()) {
      try {
        auto ret_val = std::get<T>(it->second);
        return ret_val;
      } catch (std::exception ex) {
        return std::nullopt;
      }
    }

    return std::nullopt;
  }

  void Put(std::string key, DB_store_type value) {
    auto it = m_store.find(key);
    if (it != m_store.end()) {
      std::cout << "Prepisuji: \"" << key << "\" = ";
      print_val(it->second);
    }

    m_store[key] = value;
  }

  void Print_Contents() {
    for (auto it = m_store.begin(); it != m_store.end(); it++) {
      std::cout << "\"" << it->first << "\" = ";
      print_val(it->second);
    }
  }

  void print_val(DB_store_type val) {
    try {
      std::string s_val = std::get<std::string>(val);
      std::cout << "\"" << s_val << "\"" << std::endl;
      return;
    } catch (std::exception ex) {
      // not optimal using try catch as control flow, I know;
    }
    try {
      double d_val = std::get<double>(val);
      std::cout << d_val << std::endl;
      return;
    } catch (std::exception ex) {
      // not optimal using try catch as control flow, I know;
    }
    try {
      int i_val = std::get<int>(val);
      std::cout << i_val << std::endl;
      return;
    } catch (std::exception ex) {
      // not optimal using try catch as control flow, I know;
    }
  }
};

template <typename T>
concept DB_interface = std::convertible_to<T, DB> && requires(T a) {
  a.Get("");
  a.Put("", 0);
  a.Print_Contents();
};

void run_db_term();
DB_store_type convert_val(std::string);

int main(int argc, char* argv[]) {
  DB db_test{};
  db_test.Put("aaa", 25);
  db_test.Put("aaa", "test");
  db_test.Put("abc", 35.98);

  auto test_out = db_test.Get<std::string>("aaa");
  if (test_out) {
    std::cout << *test_out << std::endl;
  } else {
    std::cout << "fail" << std::endl;
  }

  db_test.Print_Contents();
  auto db_copy = db_test;

  auto db_moved = std::move(db_test);
  db_moved.Put("db_moved", 1);

  db_copy.Put("db_copied", 1);

  db_moved.Print_Contents();
  db_copy.Print_Contents();
  db_test.Print_Contents();
  run_db_term();
  return 0;
}

void run_db_term() {
  DB db{};
  while (true) {
    std::cout << "> ";
    std::string command;
    std::cin >> command;

    if (command == "put") {
      std::cout << "Zadejte klic: ";
      std::string key;
      std::cin >> key;
      std::cout << "Zadejte hodnotu: ";
      std::string s_val;
      std::cin >> s_val;
      DB_store_type db_val = convert_val(s_val);

      db.Put(key, db_val);
    } else if (command == "get") {

    } else if (command == "list") {

    } else if (command == "exit") {
      db.Print_Contents();
      break;
    } else {
      std::cout << "invalid input" << std::endl;
    }
  }
}

DB_store_type convert_val(std::string input) {
  int i_val;
  double d_val;
  std::stringstream ss(input);
  if (!((ss >> i_val).fail() || !(ss >> std::ws).eof())) {
    return i_val;
  }

  if (!((ss >> d_val).fail() || !(ss >> std::ws).eof())) {
    return d_val;
  }

  return input;
}
