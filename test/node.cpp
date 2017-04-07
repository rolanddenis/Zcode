#include <tree/node.hpp>
#include <tree/direction.hpp>
#include <iostream>

int main()
{
    typedef unsigned short node_type;
    Node<2, node_type> n{0}, n1{0};
    n += Node<2, node_type>::Xbit; 
    n.set_level(1);
    std::cout << n << "\n";
    std::cout << n.plus(direction::y, 2) << "\n";
    std::cout << n.plus(direction::x) << "\n\n";
    std::cout << n.minus(direction::x, 3) << "\n";
    std::cout << n.minus(direction::x, 2) << "\n";
    std::cout << n.minus(direction::x) << "\n\n";
    std::cout << n1.minus(direction::x) << "\n";
    std::cout << n1.minus(direction::x, 2) << "\n";
    return 0; 
}