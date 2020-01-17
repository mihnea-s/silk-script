#include <cmath>
#include <cstdint>
#include <memory>
#include <sstream>
#include <string>

#include <silk/runtime/abstract/obj.h>
#include <silk/runtime/primitives.h>

namespace obj {

ObjectPtr make(std::int64_t value) {
  return std::unique_ptr<Object>(new Integer {value});
}

ObjectPtr make(double value) {
  return std::unique_ptr<Object>(new Double {value});
}

ObjectPtr make(bool value) {
  return std::unique_ptr<Object>(new Boolean {value});
}

ObjectPtr make(std::string value) {
  return std::unique_ptr<Object>(new String {value});
}

} // namespace obj

// Integer ----------------------------

std::string Integer::string() {
  return std::to_string(this->value);
}

bool Integer::truthy() {
  return this->value;
}

ObjectPtr Integer::operator==(ObjectPtr& other) {
  auto& other_num = obj::cast_to<Integer>(other);
  return obj::make(this->value == other_num.value);
}

ObjectPtr Integer::operator<(Orderable& other) {
  auto& other_num = obj::cast_to<Integer>(other);
  return obj::make(this->value < other_num.value);
}

ObjectPtr Integer::operator>(Orderable& other) {
  auto& other_num = obj::cast_to<Integer>(other);
  return obj::make(this->value > other_num.value);
}

ObjectPtr Integer::operator>=(Orderable& other) {
  auto& other_num = obj::cast_to<Integer>(other);
  return obj::make(this->value >= other_num.value);
}

ObjectPtr Integer::operator<=(Orderable& other) {
  auto& other_num = obj::cast_to<Integer>(other);
  return obj::make(this->value <= other_num.value);
}

ObjectPtr Integer::operator~() {
  return obj::make(~this->value);
}

ObjectPtr Integer::operator-() {
  return obj::make(-this->value);
}

ObjectPtr Integer::operator+(Concatenable& other) {
  auto& other_num = obj::cast_to<Integer>(other);
  return obj::make(this->value + other_num.value);
}

ObjectPtr Integer::operator-(Numerical& other) {
  auto& other_num = obj::cast_to<Integer>(other);
  return obj::make(this->value - other_num.value);
}

ObjectPtr Integer::operator*(Concatenable& other) {
  auto& other_num = obj::cast_to<Integer>(other);
  return obj::make(this->value * other_num.value);
}

ObjectPtr Integer::operator/(Numerical& other) {
  auto& other_num = obj::cast_to<Integer>(other);
  return obj::make(this->value * 1.0 / other_num.value);
}

ObjectPtr Integer::operator%(BitModifiable& other) {
  auto& other_num = obj::cast_to<Integer>(other);
  return obj::make(this->value % other_num.value);
}

ObjectPtr Integer::starstar(Numerical& other) {
  auto& other_num = obj::cast_to<Integer>(other);
  return obj::make((std::int64_t)std::pow(this->value, other_num.value));
}

ObjectPtr Integer::slashslash(Numerical& other) {
  auto& other_num = obj::cast_to<Integer>(other);
  return obj::make(this->value / other_num.value);
}

ObjectPtr Integer::operator&(BitModifiable& other) {
  auto& other_num = obj::cast_to<Integer>(other);
  return obj::make(this->value & other_num.value);
}

ObjectPtr Integer::operator|(BitModifiable& other) {
  auto& other_num = obj::cast_to<Integer>(other);
  return obj::make(this->value | other_num.value);
}

ObjectPtr Integer::operator^(BitModifiable& other) {
  auto& other_num = obj::cast_to<Integer>(other);
  return obj::make(this->value ^ other_num.value);
}

// Double -----------------------------

std::string Double::string() {
  return std::to_string(this->value);
}

bool Double::truthy() {
  return true;
}

ObjectPtr Double::operator==(ObjectPtr& other) {
  auto& other_num = obj::cast_to<Double>(other);
  return obj::make(other_num.value == this->value);
}

ObjectPtr Double::operator<(Orderable& other) {
  auto& other_num = obj::cast_to<Double>(other);
  return obj::make(this->value < other_num.value);
}

ObjectPtr Double::operator>(Orderable& other) {
  auto& other_num = obj::cast_to<Double>(other);
  return obj::make(this->value > other_num.value);
}

ObjectPtr Double::operator>=(Orderable& other) {
  auto& other_num = obj::cast_to<Double>(other);
  return obj::make(this->value >= other_num.value);
}

ObjectPtr Double::operator<=(Orderable& other) {
  auto& other_num = obj::cast_to<Double>(other);
  return obj::make(this->value <= other_num.value);
}

ObjectPtr Double::operator-() {
  return obj::make(-this->value);
}

ObjectPtr Double::operator+(Concatenable& other) {
  auto& other_num = obj::cast_to<Double>(other);
  return obj::make(this->value + other_num.value);
}

ObjectPtr Double::operator-(Numerical& other) {
  auto& other_num = obj::cast_to<Double>(other);
  return obj::make(this->value - other_num.value);
}

ObjectPtr Double::operator*(Concatenable& other) {
  auto& other_num = obj::cast_to<Double>(other);
  return obj::make(this->value * other_num.value);
}

ObjectPtr Double::operator/(Numerical& other) {
  auto& other_num = obj::cast_to<Double>(other);
  return obj::make(this->value / other_num.value);
}

ObjectPtr Double::starstar(Numerical& other) {
  auto& other_num = obj::cast_to<Double>(other);
  return obj::make(std::pow(this->value, other_num.value));
}

ObjectPtr Double::slashslash(Numerical& other) {
  auto& other_num = obj::cast_to<Double>(other);
  return obj::make((std::int64_t)(this->value / other_num.value));
}

// Boolean ----------------------------

std::string Boolean::string() {
  return std::to_string(this->value);
}

bool Boolean::truthy() {
  return this->value;
}

ObjectPtr Boolean::operator==(ObjectPtr& other) {
  return obj::make(this->value == other->truthy());
}

// String -----------------------------

std::string String::string() {
  return this->value;
}

bool String::truthy() {
  return !this->string().empty();
}

ObjectPtr String::operator==(ObjectPtr& other) {
  auto other_str = obj::try_cast<String>(other);
  if (other_str.has_value()) {
    auto& str = other_str.value().get().value;
    return obj::make(str.compare(this->value) == 0);
  } else {
    return obj::make(false);
  }
}

ObjectPtr String::operator<(Orderable& other) {
  auto& other_str = obj::cast_to<String>(other);
  return obj::make(this->value < other_str.value);
}

ObjectPtr String::operator>(Orderable& other) {
  auto& other_str = obj::cast_to<String>(other);
  return obj::make(this->value > other_str.value);
}

ObjectPtr String::operator>=(Orderable& other) {
  auto& other_str = obj::cast_to<String>(other);
  return obj::make(this->value >= other_str.value);
}

ObjectPtr String::operator<=(Orderable& other) {
  auto& other_str = obj::cast_to<String>(other);
  return obj::make(this->value <= other_str.value);
}

ObjectPtr String::operator+(Concatenable& other) {
  auto& other_str = obj::cast_to<String>(other);
  return obj::make(this->value + other_str.value);
}

ObjectPtr String::operator*(Concatenable& other) {
  auto& other_num = obj::cast_to<Integer>(other);

  std::ostringstream stream {};

  for (auto i = 0; i < other_num.value; i++) {
    stream << this->value;
  }

  return obj::make(stream.str());
}

// Vid --------------------------------

std::string Vid::string() {
  return "[vid]";
}

bool Vid::truthy() {
  return false;
}

ObjectPtr Vid::operator==(ObjectPtr& other) {
  auto other_vid = obj::try_cast<Vid>(other);
  return obj::make(other_vid.has_value());
}
