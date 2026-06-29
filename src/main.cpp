#include <unistd.h>

#include <memory>

#include "react_dsl.hpp"
#include "tools.hpp"

int main() {
  run_app(column(text("Hello neighbor ", Alignment::center),
                 text("Hi", Alignment::right)));
  return 0;
}
