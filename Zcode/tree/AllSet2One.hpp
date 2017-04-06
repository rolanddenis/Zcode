#pragma once
#include <array>
#include <utility>
/////////////////////////////////////////////////////////////////////////
///
/// Generate a number with all digits in range (0,n-1) set to one
///  (metaprogram).
///
/// Use it like: 
///
///  static const int k=AllSet2One<n>:value;
///  with n=3, this gives k=7. 
///
/// \brief generate a number with digits (0,n-1) set to 1.
////////////////////////////////////////////////////////////////////////
template<int dim, int n, typename node_type> struct AllSet2One
{
  static const node_type value= (AllSet2One<dim, n-1, node_type>::value<<1)+1;
};
template<int dim, typename node_type> struct AllSet2One<dim, 0, node_type>
{
  static const node_type value= 0;
};

// Construction of Ones array
// C++ 14 version
template <std::size_t level>
constexpr std::size_t Ones(std::size_t dim, std::size_t bit) {
  return Ones<level-1>(dim, bit) + (bit>>(dim*level));
}

template <>
constexpr std::size_t Ones<0>(std::size_t dim, std::size_t bit) {
    return bit;
}

template<std::size_t... i>
constexpr auto Ones_array(std::size_t dim, std::size_t bit, std::index_sequence<i...>) {
    return std::array<std::size_t, sizeof...(i)>{{Ones<i>(dim, bit)...}};
}

template<std::size_t size>
constexpr auto Ones_array(std::size_t dim, std::size_t bit) {
    return Ones_array(dim, bit, std::make_index_sequence<size>{});
}

// // C++ 17 version
// template <std::size_t levels>
// constexpr auto Ones_array(std::size_t dim, std::size_t bit){
//   std::array<std::size_t, levels> a{bit};
//   for (std::size_t i=1; i<levels; i++)
//     a[i] = a[i-1] + bit>>(dim*i);
//   return a;
// }

constexpr std::size_t max_level(std::size_t dim, std::size_t freebits, std::size_t size)
{
  std::size_t tmp = size - freebits, level = 0;
  
  while (tmp/dim > (1 << level))
  {
    tmp--;
    level++;
  }
  return level;
}


constexpr std::size_t ipow_impl(int64_t base, int exp, int64_t result = 1) {
  return exp < 1 ? result : ipow_impl(base*base, exp/2, (exp % 2) ? result*base : result);
}

constexpr std::size_t ipow(int64_t base, int exp) {
  return ipow_impl(base, exp);
}
