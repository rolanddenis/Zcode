#include <tree/node.hpp>
#include <tree/direction.hpp>
#include <iostream>

int main()
{
     Node<2> n{0};
     std::cout << n << "\n";
     std::cout << n.plus(direction::x, 1) << "\n";
     return 0; 
}