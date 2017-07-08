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
template <typename node_type, std::size_t level>
constexpr node_type Ones(std::size_t dim, node_type bit) {
  return Ones<node_type, level-1>(dim, bit) + (bit>>(dim*level));
}

template <>
constexpr unsigned short Ones<unsigned short, 0>(std::size_t dim, unsigned short bit) {
    return bit;
}

template <>
constexpr unsigned int Ones<unsigned int, 0>(std::size_t dim, unsigned int bit) {
    return bit;
}

template <>
constexpr std::size_t Ones<std::size_t, 0>(std::size_t dim, std::size_t bit) {
    return bit;
}

template<typename node_type, std::size_t... i>
constexpr auto Ones_array(std::size_t dim, node_type bit, std::index_sequence<i...>) {
    return std::array<node_type, sizeof...(i)>{{Ones<node_type, i>(dim, bit)...}};
}

template<std::size_t size, typename node_type>
constexpr auto Ones_array(std::size_t dim, node_type bit) {
    return Ones_array(dim, bit, std::make_index_sequence<size>{});
}

template <std::size_t n>
constexpr int Stencil(int stencil) {
  return -stencil + n;
}

template<std::size_t... i>
constexpr auto Stencil_array(int stencil, std::index_sequence<i...>) {
    return std::array<int, sizeof...(i)>{{Stencil<i>(stencil)...}};
}

template<std::size_t stencil>
constexpr auto Stencil_array() {
    return Stencil_array(stencil, std::make_index_sequence<2*stencil+1>{});
}

// // C++ 17 version
// template <std::size_t levels>
// constexpr auto Ones_array(std::size_t dim, std::size_t bit){
//   std::array<std::size_t, levels> a{bit};
//   for (std::size_t i=1; i<levels; i++)
//     a[i] = a[i-1] + bit>>(dim*i);
//   return a;
// }

constexpr std::size_t max_level(std::size_t const dim, std::size_t  const freebits, std::size_t  const size)
{
  std::size_t tmp = size - freebits, level = 0;
  
  while (tmp/dim > (1ul << level))
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
