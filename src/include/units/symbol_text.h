#pragma once

#include <units/bits/external/fixed_string.h>

namespace units {

template<typename StandardCharT, typename ASCIICharT, std::size_t N, std::size_t M>
struct basic_symbol_text {
  basic_fixed_string<StandardCharT, N> standard_;
  basic_fixed_string<ASCIICharT, M> ascii_;

  constexpr basic_symbol_text(StandardCharT s) noexcept: standard_(s), ascii_(s) {}
  constexpr basic_symbol_text(StandardCharT s, ASCIICharT a) noexcept: standard_(s), ascii_(a) {}
  constexpr basic_symbol_text(const StandardCharT (&s)[N + 1]) noexcept: standard_(s), ascii_(s) {}
  constexpr basic_symbol_text(const basic_fixed_string<StandardCharT, N>& s) noexcept: standard_(s), ascii_(s) {}
  constexpr basic_symbol_text(const StandardCharT (&s)[N + 1], const ASCIICharT (&a)[M + 1]) noexcept: standard_(s), ascii_(a) {}
  constexpr basic_symbol_text(const basic_fixed_string<StandardCharT, N>& s, const basic_fixed_string<ASCIICharT, M>& a) noexcept: standard_(s), ascii_(a) {}

  [[nodiscard]] constexpr auto& standard() { return standard_; }
  [[nodiscard]] constexpr const auto& standard() const { return standard_; }
  [[nodiscard]] constexpr auto& ascii() { return ascii_; }
  [[nodiscard]] constexpr const auto& ascii() const { return ascii_; }

  template<std::size_t N2, std::size_t M2>
  [[nodiscard]] constexpr friend basic_symbol_text<StandardCharT, ASCIICharT, N + N2, M + M2> operator+(
      const basic_symbol_text& lhs, const basic_symbol_text<StandardCharT, ASCIICharT, N2, M2>& rhs) noexcept
  {
    return basic_symbol_text<StandardCharT, ASCIICharT, N + N2, M + M2>(
        lhs.standard_ + rhs.standard_, lhs.ascii_ + rhs.ascii_);
  }

  template<std::size_t N2>
  [[nodiscard]] constexpr friend basic_symbol_text<StandardCharT, ASCIICharT, N + N2, M + N2> operator+(
      const basic_symbol_text& lhs, const basic_fixed_string<StandardCharT, N2>& rhs) noexcept
  {
    return (lhs + basic_symbol_text<StandardCharT, StandardCharT, N2, N2>(rhs));
  }
  
  template<std::size_t N2>
  [[nodiscard]] constexpr friend basic_symbol_text<StandardCharT, ASCIICharT, N + N2, M + N2> operator+(
      const basic_fixed_string<StandardCharT, N2>& lhs, const basic_symbol_text& rhs) noexcept
  {
    return (basic_symbol_text<StandardCharT, StandardCharT, N2, N2>(lhs) + rhs);
  }

  template<std::size_t N2>
  [[nodiscard]] constexpr friend basic_symbol_text<StandardCharT, ASCIICharT, N + N2 - 1, M + N2 - 1> operator+(
      const basic_symbol_text& lhs, const StandardCharT (&rhs)[N2]) noexcept
  {
    return (lhs + basic_symbol_text<StandardCharT, StandardCharT, N2 - 1, N2 - 1>(rhs));
  }
  
  template<std::size_t N2>
  [[nodiscard]] constexpr friend basic_symbol_text<StandardCharT, ASCIICharT, N + N2 - 1, M + N2 - 1> operator+(
      const StandardCharT (&lhs)[N2], const basic_symbol_text& rhs) noexcept
  {
    return (basic_symbol_text<StandardCharT, StandardCharT, N2 - 1, N2 - 1>(lhs) + rhs);
  }

  [[nodiscard]] constexpr friend basic_symbol_text<StandardCharT, ASCIICharT, N + 1, M + 1> operator+(
      const basic_symbol_text& lhs, StandardCharT rhs) noexcept
  {
    return (lhs + basic_symbol_text<StandardCharT, StandardCharT, 1, 1>(rhs));
  }
  
  [[nodiscard]] constexpr friend basic_symbol_text<StandardCharT, ASCIICharT, N + 1, M + 1> operator+(
      StandardCharT lhs, const basic_symbol_text& rhs) noexcept
  {
    return (basic_symbol_text<StandardCharT, StandardCharT, 1, 1>(lhs) + rhs);
  }

  [[nodiscard]] constexpr friend bool operator==(const basic_symbol_text& lhs,
                                                 const basic_symbol_text& rhs) noexcept
  {
    return (lhs.standard_ == rhs.standard_ && lhs.ascii_ == rhs.ascii_);
  }

  [[nodiscard]] constexpr friend bool operator!=(const basic_symbol_text& lhs,
                                                 const basic_symbol_text& rhs) noexcept
  {
    return !(lhs == rhs);
  }

  template<typename StandardCharT2, typename ASCIICharT2, std::size_t N2, std::size_t M2>
  [[nodiscard]] constexpr friend bool operator==(const basic_symbol_text&,
                                                 const basic_symbol_text<StandardCharT2, ASCIICharT2, N2, M2>&) noexcept
  {
    return false;
  }

  template<typename StandardCharT2, typename ASCIICharT2, std::size_t N2, std::size_t M2>
  [[nodiscard]] constexpr friend bool operator!=(const basic_symbol_text&,
                                                 const basic_symbol_text<StandardCharT2, ASCIICharT2, N2, M2>&) noexcept
  {
    return true;
  }

  [[nodiscard]] constexpr friend bool operator==(const basic_symbol_text& lhs,
                                                 const basic_fixed_string<StandardCharT, N>& rhs) noexcept
  {
    return (lhs.standard_ == rhs);
  }

  [[nodiscard]] constexpr friend bool operator!=(const basic_symbol_text& lhs,
                                                 const basic_fixed_string<StandardCharT, N>& rhs) noexcept
  {
    return !(lhs == rhs);
  }
  
  template<typename StandardCharT2, std::size_t N2>
  [[nodiscard]] constexpr friend bool operator==(const basic_symbol_text&,
                                                 const basic_fixed_string<StandardCharT2, N2>&) noexcept
  {
    return false;
  }

  template<typename StandardCharT2, std::size_t N2>
  [[nodiscard]] constexpr friend bool operator!=(const basic_symbol_text&,
                                                 const basic_fixed_string<StandardCharT2, N2>&) noexcept
  {
    return true;
  }
  
  [[nodiscard]] constexpr friend bool operator==(const basic_symbol_text& lhs,
                                                 const StandardCharT (&rhs)[N + 1]) noexcept
  {
    return (lhs.standard_ == rhs);
  }

  [[nodiscard]] constexpr friend bool operator!=(const basic_symbol_text& lhs,
                                                 const StandardCharT (&rhs)[N + 1]) noexcept
  {
    return !(lhs == rhs);
  }
  
  template<typename StandardCharT2, std::size_t N2>
  [[nodiscard]] constexpr friend bool operator==(const basic_symbol_text&,
                                                 const StandardCharT2 (&)[N2 + 1]) noexcept
  {
    return false;
  }

  template<typename StandardCharT2, std::size_t N2>
  [[nodiscard]] constexpr friend bool operator!=(const basic_symbol_text&,
                                                 const StandardCharT2 (&)[N2 + 1]) noexcept
  {
    return true;
  }

  [[nodiscard]] constexpr friend bool operator==(const basic_symbol_text& lhs,
                                                 StandardCharT rhs) noexcept
  {
    return (lhs.standard_ == rhs);
  }

  [[nodiscard]] constexpr friend bool operator!=(const basic_symbol_text& lhs,
                                                 StandardCharT rhs) noexcept
  {
    return !(lhs == rhs);
  }

  template<typename StandardCharT2>
  [[nodiscard]] constexpr friend bool operator==(const basic_symbol_text&,
                                                 StandardCharT2) noexcept
  {
    return false;
  }

  template<typename StandardCharT2>
  [[nodiscard]] constexpr friend bool operator!=(const basic_symbol_text&,
                                                 StandardCharT2) noexcept
  {
    return true;
  }

  template<typename StandardCharT2, typename ASCIICharT2, std::size_t N2, std::size_t M2>
  [[nodiscard]] constexpr friend bool operator<(const basic_symbol_text& lhs,
                                                const basic_symbol_text<StandardCharT2, ASCIICharT2, N2, M2>& rhs) noexcept
  {
    return (lhs.standard_ < rhs.standard_);
  }

  template<typename StandardCharT2, std::size_t N2>
  [[nodiscard]] constexpr friend bool operator<(const basic_symbol_text& lhs,
                                                const basic_fixed_string<StandardCharT2, N2>& rhs) noexcept
  {
    return (lhs.standard_ < rhs);
  }

  template<typename StandardCharT2, std::size_t N2>
  [[nodiscard]] constexpr friend bool operator<(const basic_symbol_text& lhs,
                                                const StandardCharT2 (&rhs)[N2]) noexcept
  {
    return (lhs.standard_ < basic_fixed_string(rhs));
  }

  template<typename StandardCharT2>
  [[nodiscard]] constexpr friend bool operator<(const basic_symbol_text& lhs,
                                                StandardCharT2 rhs) noexcept
  {
    return (lhs.standard_ < basic_fixed_string(rhs));
  }

  template<typename StandardCharT2, typename ASCIICharT2, std::size_t N2, std::size_t M2>
  [[nodiscard]] constexpr friend bool operator>(const basic_symbol_text& lhs,
                                                const basic_symbol_text<StandardCharT2, ASCIICharT2, N2, M2>& rhs) noexcept
  {
    return (lhs.standard_ > rhs.standard_);
  }

  template<typename StandardCharT2, std::size_t N2>
  [[nodiscard]] constexpr friend bool operator>(const basic_symbol_text& lhs,
        const basic_fixed_string<StandardCharT2, N2>& rhs) noexcept
  {
    return (lhs.standard_ > rhs);
  }

  template<typename StandardCharT2, std::size_t N2>
  [[nodiscard]] constexpr friend bool operator>(const basic_symbol_text& lhs,
        const StandardCharT2 (&rhs)[N2]) noexcept
  {
    return (lhs.standard_ > basic_fixed_string(rhs));
  }

  template<typename StandardCharT2>
  [[nodiscard]] constexpr friend bool operator>(const basic_symbol_text& lhs,
                                                StandardCharT2 rhs) noexcept
  {
    return (lhs.standard_ > basic_fixed_string(rhs));
  }

  template<typename StandardCharT2, typename ASCIICharT2, std::size_t N2, std::size_t M2>
  [[nodiscard]] constexpr friend bool operator<=(const basic_symbol_text& lhs,
                                                 const basic_symbol_text<StandardCharT2, ASCIICharT2, N2, M2>& rhs) noexcept
  {
    return (lhs.standard_ <= rhs.standard_);
  }

  template<typename StandardCharT2, std::size_t N2>
  [[nodiscard]] constexpr friend bool operator<=(const basic_symbol_text& lhs,
                                                 const basic_fixed_string<StandardCharT2, N2>& rhs) noexcept
  {
    return (lhs.standard_ <= rhs);
  }

  template<typename StandardCharT2, std::size_t N2>
  [[nodiscard]] constexpr friend bool operator<=(const basic_symbol_text& lhs,
                                                 const StandardCharT2 (&rhs)[N2]) noexcept
  {
    return (lhs.standard_ <= basic_fixed_string(rhs));
  }

  template<typename StandardCharT2>
  [[nodiscard]] constexpr friend bool operator<=(const basic_symbol_text& lhs,
                                                 StandardCharT2 rhs) noexcept
  {
    return (lhs.standard_ <= basic_fixed_string(rhs));
  }

  template<typename StandardCharT2, typename ASCIICharT2, std::size_t N2, std::size_t M2>
  [[nodiscard]] constexpr friend bool operator>=(const basic_symbol_text& lhs,
                                                 const basic_symbol_text<StandardCharT2, ASCIICharT2, N2, M2>& rhs) noexcept
  {
    return (lhs.standard_ >= rhs.standard_);
  }

  template<typename StandardCharT2, std::size_t N2>
  [[nodiscard]] constexpr friend bool operator>=(const basic_symbol_text& lhs,
                                                 const basic_fixed_string<StandardCharT2, N2>& rhs) noexcept
  {
    return (lhs.standard_ >= rhs);
  }

  template<typename StandardCharT2, std::size_t N2>
  [[nodiscard]] constexpr friend bool operator>=(const basic_symbol_text& lhs,
                                                 const StandardCharT2 (&rhs)[N2]) noexcept
  {
    return (lhs.standard_ >= basic_fixed_string(rhs));
  }

  template<typename StandardCharT2>
  [[nodiscard]] constexpr friend bool operator>=(const basic_symbol_text& lhs,
                                                 StandardCharT2 rhs) noexcept
  {
    return (lhs.standard_ >= basic_fixed_string(rhs));
  }
};

template<typename StandardCharT>
basic_symbol_text(StandardCharT) -> basic_symbol_text<StandardCharT, StandardCharT, 1, 1>;

template<typename StandardCharT, typename ASCIICharT>
basic_symbol_text(StandardCharT, ASCIICharT) -> basic_symbol_text<StandardCharT, ASCIICharT, 1, 1>;

template<typename StandardCharT, std::size_t N>
basic_symbol_text(const StandardCharT (&)[N]) -> basic_symbol_text<StandardCharT, StandardCharT, N - 1, N - 1>;

template<typename StandardCharT, std::size_t N>
basic_symbol_text(const basic_fixed_string<StandardCharT, N>&) -> basic_symbol_text<StandardCharT, StandardCharT, N, N>;

template<typename StandardCharT, typename ASCIICharT, std::size_t N, std::size_t M>
basic_symbol_text(const StandardCharT (&)[N], const ASCIICharT (&)[M]) -> basic_symbol_text<StandardCharT, ASCIICharT, N - 1, M - 1>;

template<typename StandardCharT, typename ASCIICharT, std::size_t N, std::size_t M>
basic_symbol_text(const basic_fixed_string<StandardCharT, N>&,
                  const basic_fixed_string<ASCIICharT, M>&)
-> basic_symbol_text<StandardCharT, ASCIICharT, N, M>;

}  // namespace units
