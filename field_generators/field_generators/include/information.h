#pragma once
#ifndef INFORMATION_H
#define INFORMATION_H

#include "renderable.h"

class Information: public Renderable {
public:
  Information(float font_size = 24.0f);
  virtual ~Information(void);

  META_Renderable(Information)
  ;

  void setText(const std::string& text, float x = 32.0f, float y = 32.0f);
  void setFontSize(float font_size);

protected:
  virtual void updateImpl();

private:
  float width_, height_;
  std::string text_;
  float x_, y_;
  float font_size_;

  static std::mutex mutex_default_font_;
};

#endif // INFORMATION_H
