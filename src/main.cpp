#include <unistd.h>

#include <memory>

#include "ColumnContainer.hpp"
#include "GridContainer.hpp"
#include "text.hpp"
#include "tools.hpp"
#include "widgetTree.hpp"

int main() {
  tools::invisiableCursor();
  tools::clearScreen();

  auto MainContainer = std::make_unique<Column>();

  auto text1 = std::make_unique<Text>("Hello neighbor ");
  text1->setAlignment(Alignment::center);

  auto text2 = std::make_unique<Text>("Hi");
  text2->setAlignment(Alignment::right);

  MainContainer->addChild(std::move(text1));
  MainContainer->addChild(std::move(text2));

  WidgetTree tree(std::move(MainContainer));

  for (;;) {
    tools::cursorHomePosition();
    tree.fb.resizeBuffer();
    tree.layout({0, 0, tree.fb.row, tree.fb.col});
    tree.render();
    tree.display();
    usleep(50000);
  }

  tools::visiableCursor();
  return 0;
}
