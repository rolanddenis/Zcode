#include <tree/node/node.hpp>
#include <tree/node/neighbor.hpp>
#include <tree/node/family.hpp>
#include <tree/node/refine.hpp>
#include <tree/node/direction.hpp>
#include <iostream>

int main()
{
    typedef unsigned short node_type;
    std::size_t const dim = 2;
    Node<dim, node_type> n{"1" + std::string(15, '0')}, n1;

    std::cout << n << "\n";

    // n += Node<dim, node_type>::Xbit; 
    // n.set_level(1);
    // n1.set_level(1);

    // std::cout << shareAncestor(n1, n1, 0) << "\n";
    // std::cout << firstSon(n) << " " << lastSon(n) << " " << father(n) << "\n";
    // std::cout << n.lastlevel() << "\n";
    // // std::cout << n.plus(direction::x, 0) << "\n\n";
    // // std::cout << n.plus(direction::x, 1) << "\n\n";
    // // std::cout << n.plus(direction::y, 0) << "\n\n";
    // // std::cout << n.plus(direction::y, 1) << "\n\n";
    // // std::cout << n.plus(direction::z, 0) << "\n\n";
    // // std::cout << n.plus(direction::z, 1) << "\n\n";
    // // n = n.minus(direction::x, 1);
    // // std::cout << n << "\n";
    // // std::cout << n.minus(direction::x, 1) << "\n\n";

    // // std::cout << n.plus(direction::y, 2) << "\n";
    // // std::cout << n.plus(direction::x) << "\n\n";
    // // std::cout << n.minus(direction::x, 3) << "\n";
    // // std::cout << n.minus(direction::x, 2) << "\n";
    // // std::cout << n.minus(direction::x) << "\n\n";
    // // std::cout << n1.minus(direction::x) << "\n";
    // // std::cout << n1.minus(direction::x, 2) << "\n\n";

    // // std::size_t const stencil = 3;
    // // std::array<Node<dim, node_type>, dim*2*stencil> star;
    // // starNeighbors<stencil>(n, star);
    // // for (auto&s: star)
    // //     std::cout << s << "\n";

    // std::size_t const stencil = 1;
    // // std::array<Node<dim, node_type>, ipow(2*stencil+1, dim)> box;
    // // boxNeighbors<stencil>(n, box);
    // // for (auto&b: box)
    // //     std::cout << b << "\n";

    // std::array<Node<dim, node_type>, ipow((2*stencil+1), dim)> P;
    // for (std::size_t i = 0; i < 1000000; ++i)
    //     {
    //         boxNeighbors<stencil>(n1, P);
    //         n1 = P[0];
    //     }

    // // for (auto&p: P)
    // //     std::cout << p << "\n";
    return 0; 
}