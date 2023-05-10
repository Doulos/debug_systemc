#pragma once
/// A simple transaction class with a unique id and a numeric value.

#include <string>
#include <cstdint>
#include <random>
#include <ostream>
#include <systemc>

using Id_t = size_t;

struct Transaction
{
  static constexpr const Id_t BAD_ID{SIZE_T_MAX};
  using Data_t = int32_t;
  explicit Transaction( Data_t value ) : m_id{nextid()}, m_value{value} {}
  Transaction() : m_id{nextid()}, m_value{} { randomize(); }
  // Copy constructor - value only
  Transaction( const Transaction& rhs ) : m_id{nextid()}, m_value{rhs.m_value} {}
  // Move constructor
  Transaction( Transaction&& rhs ) = default;
  bool operator==( const Transaction& rhs ) const { return m_value == rhs.m_value; }
  [[nodiscard]] bool identical( const Transaction& rhs) const { return m_id == rhs.m_id and *this == rhs; }
  // Copy assignment
  Transaction& operator=( const Transaction& rhs ) { m_value = rhs.m_value; return *this; }
  // Move assignment
  Transaction& operator=( Transaction&& rhs ) {
    m_id = std::exchange( rhs.m_id, BAD_ID );
    m_value = std::exchange( rhs.m_value, ~rhs.m_value );
  }
  inline Id_t  id() const { return m_id; }
  inline Data_t& value() { return m_value; }
  void randomize() {
    thread_local std::mt19937_64 gen{};
    std::uniform_int_distribution<Data_t> dist;
    m_value = dist(gen);
  }
  std::string to_string() const {
    auto result = std::string{"{ id:"} + std::to_string(m_id) + " value:" + std::to_string(m_value) + " }";
    return result;
  }
  friend std::ostream& operator<<( std::ostream&  os, const Transaction& rhs ) {
    os << rhs.to_string();
    return os;
  }
  using sc_trace_file = sc_core::sc_trace_file;
  friend void sc_trace(sc_trace_file* f, const Transaction& rhs, const std::string& name)
  {
    sc_trace( f, rhs.m_value, name + ".value" );
  }
    private:
  static Id_t nextid() { static Id_t next{0}; return next++; }
  Id_t   m_id;
  Data_t m_value;
};
