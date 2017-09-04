#pragma once

#include <tree/node/cell.hpp>

//! refine n. Results in refined[0: treetype-1].
//! \param  n: node.
//! \param refined[]: the refined nodes.
template<typename Node_type, typename Node_array>
void refine(Node_type const& n, Node_array & refined)
  {
    typename Node_type::type nnew = n.value + Node_type::levelone;//we add one level.
    auto level = n.level() + 1;
    refined[0].value = nnew;// <=> add zero at right <=> do not modify n.
    for(int i=1; i< Node_type::treetype; ++i)
        refined[i].value = nnew + (Node_type::TailGen[i]>>(Node_type::dim*level));
  }
