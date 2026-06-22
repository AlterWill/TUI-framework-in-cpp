#include <unistd.h>

#include <memory>

#include "box.hpp"
#include "text.hpp"
#include "tools.hpp"
#include "widgetTree.hpp"

using std::make_unique;

int main() {
  tools::invisiableCursor();
  tools::clearScreen();

  // 1. Create the root container (Box)
  auto MainContainer = std::make_unique<Box>(std::vector<std::unique_ptr<Widget>>{});
  MainContainer->setRect(1, 1, 20, 50);  // Set root width/height

  // 2. Create the first Text child and add it to the container
  auto text1 = std::make_unique<Text>(Rect{2, 2, 3, 30}, "Hello neighbor");
  MainContainer->layoutType->addChild(std::move(text1));

  // 3. Create a nested Box, add its Text child, and add it to MainContainer
  auto innerBox = std::make_unique<Box>(std::vector<std::unique_ptr<Widget>>{});
  innerBox->setRect(5, 6, 8, 30);

  auto text2 = std::make_unique<Text>(Rect{6, 7, 2, 20}, "Hi");
  innerBox->layoutType->addChild(std::move(text2));

  MainContainer->layoutType->addChild(std::move(innerBox));

  // 4. Initialize the WidgetTree with the root container
  WidgetTree tree(std::move(MainContainer));

  // 5. Render Loop
  for (;;) {
    tools::cursorHomePosition();
    tree.fb.resizeBuffer();  // Resize the frameBuffer
    tree.dfs();              // Traverse and render layouts
    tree.display();          // Output buffer to screen
    usleep(50000);           // Slow down update cycle (~20 FPS)
  }

  tools::visiableCursor();
  return 0;
}
