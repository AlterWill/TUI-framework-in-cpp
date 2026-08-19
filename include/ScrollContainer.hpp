#pragma once 

#include "singleChildWidget.hpp"

class Scroll : public SingleChildWidget{
  // if there is a buffer, I can just sent the part of the buffer to the framebuffer
  // need offset x or y OR x and y?

 /*
  it contains teh viewport and container view
  the viewport contains the actual widget size 
  container view contains a large area for the widget to render

  scroll offset moves the viewport across the container view
  */
};
