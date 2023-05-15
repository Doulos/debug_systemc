#pragma once
/// A simple transaction class with a unique id, a numeric data value, and a simple name.

#include <string>
#include <cstdint>
#include <climits>
#include <random>
#include <ostream>
#include <systemc>

using Id_t = size_t;

struct Transaction
{
  static constexpr const char* mesgType = "/Doulos/transaction";
  static constexpr const Id_t BAD_ID{SIZE_MAX};
  using Data_t = int32_t;

  // Constructor
  explicit Transaction( Data_t data ) : m_id{nextid()}, m_data{data} {}
  Transaction() : m_id{nextid()}, m_data{} { randomize(); }

  // Rule of zero

  // Compare ignoring id
  bool operator==( const Transaction& rhs ) const { return m_data == rhs.m_data; }

  // Include the id
  [[nodiscard]] bool identical( const Transaction& rhs) const { return m_id == rhs.m_id and *this == rhs; }

  [[nodiscard]] inline Id_t  id() const { return m_id; }
  inline Data_t& data() { return m_data; }

  void newid()
  {
    auto id = nextid();
    SC_REPORT_INFO_VERB( mesgType,
                         ( std::string{"New id "} + std::to_string(id)
                         + std::string{" assigned. Previously "} + std::to_string(m_id)
                         ).c_str(),
                         sc_core::SC_DEBUG );
    m_id = id;
  }


  void randomize() {
    static std::mt19937_64 gen{};
    std::uniform_int_distribution<Data_t> dist;
    m_data = dist(gen);
  }

  // I/O support
  [[nodiscard]] std::string to_string() const {
    auto result = std::string{"{ id:"} + std::to_string(m_id) + " data:" + std::to_string(m_data) + " }";
    return result;
  }
  friend std::ostream& operator<<( std::ostream&  os, const Transaction& rhs ) {
    os << rhs.to_string();
    return os;
  }

  // Tracing support
  using sc_trace_file = sc_core::sc_trace_file;
  friend void sc_trace(sc_trace_file* f, const Transaction& rhs, const std::string& name)
  {
    sc_trace( f, rhs.m_data, name + ".data" );
  }

private:
  static Id_t nextid() { static Id_t next{0}; return next++; }
  Id_t   m_id;
  Data_t m_data;
};
