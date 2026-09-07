## Next Steps

1. ~~SplitPane container (include/layout/SplitPane.hpp):~~
  - ~~Inherits directly from Widget (or SingleChildWidget).~~
  - ~~Owns two child widgets (first, second) along with a split ratio/size and an orientation (Horizontal or Vertical).~~
  - ~~Draws an optional divider bar between them with mouse drag support for resizing.~~
2. ~~Overlay container (include/layout/Overlay.hpp):~~
  - ~~Built on top of Stack to manage Z-ordering.~~
  - ~~Holds a base application layer plus an overlay list for popups, modals, dropdowns, and tooltips.~~
  - ~~Handles mouse event trapping/dismissal (modal backdrop click-outside to close).~~
3. Interactive controls (Buttons, Text Input):
  - Implement Button and TextInput so dialogs/overlays and split panes have interactive elements to test with.
