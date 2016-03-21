#pragma once
#ifndef ABSTRACT_PARAMETER_H_
#define ABSTRACT_PARAMETER_H_

#include <string>

#include <boost/any.hpp>

class AbstractParameter {
public:
  AbstractParameter(const std::string& name, const std::string& description, const boost::any& value) :
      name_(name), description_(description), default_value_(value), current_value_(value) {
  }
  virtual ~AbstractParameter(void) {
  }

  const std::string& getName() const {
    return name_;
  }
  const std::string& getDescription() const {
    return description_;
  }
  void setDefaultValue(const boost::any& value) {
    default_value_ = value;
  }
  void setValue(const boost::any& value) {
    current_value_ = value;
  }
  void reset() {
    current_value_ = default_value_;
  }

  virtual std::string valueTip() = 0;

protected:
  std::string name_;
  std::string description_;
  boost::any default_value_;
  boost::any current_value_;
};

#endif // ABSTRACT_PARAMETER_H_
