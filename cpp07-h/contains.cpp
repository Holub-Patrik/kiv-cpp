#include <algorithm>
#include <array>
#include <iostream>
#include <list>
#include <ranges>
#include <vector>

template <typename Tval, typename Tcont>
bool contains(Tval value, Tcont container) {
  return std::find(container.begin(), container.end(), value) !=
         container.end();
}

template <typename Tval, typename... Tcont>
bool any_of_contains_fold(Tval value, Tcont... containers) {
  return (contains(value, containers) || ...);
}

template <typename Tval, typename Tcend>
bool any_of_contains(Tval value, Tcend curr) {
  return contains(value, curr);
}

template <typename Tval, typename Tcend, typename... Tcont>
bool any_of_contains(Tval value, Tcend curr, Tcont... containers) {
  return contains(value, curr) || any_of_contains(value, containers...);
}

int main(int argc, char* argv[]) {
  std::vector<int> cont1{1, 2, 3};
  std::array<int, 6> cont2{5, 6, 7, 8, 9, 10};
  std::list<int> cont3{1, 2, 3, 4, 5};

  std::cout << "----- Fold -----" << std::endl;

  for (int val : std::views::iota(0, 20)) {
    std::string found =
        any_of_contains_fold(val, cont1, cont2) ? "true" : "false";
    std::cout << val << ":" << found << " ";

    if ((val + 1) % 5 == 0) {
      std::cout << std::endl;
    }
  }

  std::cout << "----- Vart -----" << std::endl;

  for (int val : std::views::iota(0, 20)) {
    std::string found = any_of_contains(val, cont1, cont2) ? "true" : "false";
    std::cout << val << ":" << found << " ";

    if ((val + 1) % 5 == 0) {
      std::cout << std::endl;
    }
  }

  std::cout << "----- Three -----" << std::endl;

  std::cout << "Test3: "
            << (any_of_contains(3, cont1, cont2, cont3) ? "true" : "false")
            << std::endl;

  return 0;
}
