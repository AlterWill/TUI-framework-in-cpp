#include <unistd.h>

#include <memory>

#include "ColumnContainer.hpp"
#include "GridContainer.hpp"
#include "RowContainer.hpp"
#include "box.hpp"
#include "text.hpp"
#include "tools.hpp"
#include "widgetTree.hpp"

// Define the missing Container::render method to satisfy the linker
void Container::render(frameBuffer &fb) {
  for (auto &child : children) {
    child->render(fb);
  }
}

int main() {
  tools::invisiableCursor();
  tools::clearScreen();

  auto MainContainer = std::make_unique<Grid>(9, 9);

  auto text1 = std::make_unique<Text>("Hello neighbor ");

  auto innerBox = std::make_unique<Box>(std::make_unique<Row>(),
                                        std::vector<std::unique_ptr<Widget>>{});
  auto text2 = std::make_unique<Text>("Hi");
  text2->setAlignment(Alignment::right);
  innerBox->layoutType->addChild(std::move(text2));

  MainContainer->addChild(std::move(innerBox));
  MainContainer->addChild(std::move(text1));

  WidgetTree tree(std::move(MainContainer));

  for (;;) {
    tools::cursorHomePosition();
    tree.fb.resizeBuffer();
    tree.dfs();
    tree.display();
    usleep(50000);
  }

  tools::visiableCursor();
  return 0;
}
