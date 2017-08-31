#pragma once
#include <vector>
#include <algorithm>
#include <fstream>

#include <tree/node/node.hpp>

template<class ForwardIt, class UnaryPredicate, class UnaryFunction>
void myreplace_if(ForwardIt first, ForwardIt last,
                UnaryPredicate p, UnaryFunction f)
{
    for (; first != last; ++first) {
        if(p(*first)) {
            f(*first);
        }
    }
}
////////////////////////////////////////////////////////////////////////////
///
/// slot stucture, used to store Nodes with a hash code in an interval.
///
/// \brief slot structures, store a set of Nodes.
////////////////////////////////////////////////////////////////////////////
template < std::size_t dim, typename node_value_type = std::size_t >
struct slot: private std::vector< Node<dim, node_value_type> >
{
    using node_type = Node<dim, node_value_type>;
    using parent = std::vector<node_type>;
    using parent::operator[];
    using parent::push_back;
    using parent::insert;
    using parent::erase;
    using parent::reserve;
    using parent::resize;
    using parent::begin;
    using parent::end;
    using parent::cbegin;
    using parent::cend;
    using parent::size;
    using parent::capacity;
    using parent::shrink_to_fit;

    static const node_value_type FreeBitsPart = node_type::FreeBitsPart;
    static const node_value_type voidbit = node_type::voidbit;
    static const node_value_type decal = dim*node_type::nlevels;

    node_type s1{0}, s2{node_type::AllOnes[node_type::nlevels-1]};

private:
    std::size_t slotrank;       //!< rank in a collection of slots.
    std::size_t startrank = 0;  //!< used to define a rank for all Nodes, in a slot collection.
    mutable unsigned char slotMark=0; // to put various marks on the slot.
    bool vCanBeDeleted=true;

public:
    slot() = default;
    // remark: the default copy constructor doesn't set the same capacity of the copied slot
    slot(const slot&) = default;

    slot(node_type _s1, node_type _s2, std::size_t size):s1{_s1}, s2{_s2}
    {
        reserve(size);
    }

    slot(std::size_t size)
    {
        reserve(size);
    }

    inline void copyInArray(node_type* array) const
    {
        for(std::size_t i=0; i<size(); i++)
            array[i] = (*this)[i];
    }

    //! find a Node.
    //! \param x: Node *hashed*
    //! \note we *do* *not* *check* if x is hashed.
    inline auto find(node_type const & node)
    {
        auto predicate = [&](auto const& n){return node==n;};
        auto index = std::find_if(cbegin(), cend(), predicate);
        return index;
    }

    //! put a Node at the end.
    //! \param x a *hashed* node (not checked).
    inline void put(node_type x)
    {
        push_back(x);
    }

    //! put a vector of Node's at the end.
    //! \param x vector of  *hashed* nodes (not checked).
    template<typename container>
    inline void put(container const& x)
    {
        insert(end(), x.cbegin(), x.cend());
    }

    //! position of the last entered Node in v[]
    inline int lastPos() const
    {
        return size()-1;
    }

    //! compress: ie, supress void Nodes with a given value
    //! \param N  supress nodes for which N&node !=0
    inline void compress(node_type N=node_type::voidbit)
    {
        const unsigned char mask = (N.value&FreeBitsPart)>>decal;
        if((mask&slotMark)==mask)
        {
            auto index = std::remove_if(begin(), end(), [&](auto const& n){return ((n.value&FreeBitsPart)==(N.value&FreeBitsPart));});
            resize(std::distance(begin(), index));
            slotMark -= mask;
        }
    }

    //! compress all marked Nodes. ie, supress Nodes with any value in
    //! the FreeBitsPart or marked as void
    inline void compressany()
    {
        auto index = std::remove_if(begin(), end(), [&](auto const& n){return (n.value&FreeBitsPart);});
        resize(std::distance(begin(), index));
        slotMark=0;
    }

    //! compress: ie, supress void Nodes with given values
    //! \param N  test value
    //! \param M  test value
    //! \note  a Node K are supressed iff K&N==N *or* K&M==M
    inline void compress(node_type N, node_type M)
    {
        const unsigned char n1 =(N.value&FreeBitsPart)>>decal;
        const unsigned char m1 =(M.value&FreeBitsPart)>>decal;
        if(((slotMark&n1)==n1)||((slotMark&m1)==m1))
        {
            auto index = std::remove_if(begin(), end(), [&](auto const& n){return ((n.value&FreeBitsPart)==(N.value&FreeBitsPart)) || ((n.value&FreeBitsPart)==(M.value&FreeBitsPart));});
            resize(std::distance(begin(), index));
            if((slotMark&n1)==n1) slotMark-=n1;
            if((slotMark&m1)==m1) slotMark-=m1;
        }
    }

    // How is the slot marked?
    inline node_type howMasked() const
    {
        node_type n{slotMark};
        return n << decal;
    }

    //! mark the slot with some value.
    //! \param N associated value.
    inline void setMark(node_type N)
    {
        const unsigned char mask = (N.value&FreeBitsPart)>>decal;
        if(!(slotMark&mask)) slotMark+=mask;
    }

    //! get the mark tag.
    inline unsigned char getMark()
    {
        return slotMark;
    }

    // ! change the mark the marked Nodes
    // ! \param a ancient mark
    // ! \param b new mark.
    inline void changeMark(node_type a, node_type b)
    {
        const unsigned char amask = (a.value&FreeBitsPart)>>decal;
        const unsigned char bmask = (b.value&FreeBitsPart)>>decal;
        if(slotMark&amask)
        {
            slotMark -= amask;
            if(!(slotMark&bmask))
                slotMark += bmask;
            myreplace_if(begin(), end(), [&](auto &n){return n.value&a.value;}, [&](auto &n){n.value += -a.value+b.value;});
        }
    }

    //! suppress a given mark
    //! \param N the mark
    //! \note throw an exception if not marked "mark".
    inline void unsetMark(node_type N)
    {
        const unsigned char mask = (N.value&FreeBitsPart)>>decal;
        if(slotMark&mask)
            slotMark -= mask;
    }

    //! test if the slot has been marked by an other mark as "mark"
    //! \param mark for the test.
    bool markedOther(node_type mark)
    {
        node_type N{static_cast<node_value_type>(FreeBitsPart-mark.value)};
        const unsigned char m = (N.value&FreeBitsPart)>>decal;
        return slotMark&m;
    }

    //! does this slot contains void Nodes ?
    inline bool hasvoidNodes() const
    {
        return slotMark&(voidbit>>decal);
    }

    //! mark the slot as containing void Nodes.
    inline void sethasvoidNodes()
    {
        setMark(voidbit);
    }

    //! make a logical "and" of all Nodes with a given value.
    //! \param N the value.
    inline void And(node_type N)
    {
        std::for_each(begin(), end(), [&](auto &n){n.value&=N.value;});
    }

    //! Tag, ie add  some value to all Nodes.
    //! \note a "and" with the value must be zero. Not tested if DEBUG
    //! is not set. We do not want to set a value to Nodes, but to tag them.
    //! \param N the value.
    inline void setTag(node_type N)
    {
        std::for_each(begin(), end(), [&](auto &n){n.value|=N.value;});
        const unsigned char m = (N.value&FreeBitsPart)>>decal;
        if(!(slotMark&m)) slotMark+=m;
    }

    //! empty the slot. Do not change s1 and s2, do not deallocate space.
    inline void empty()
    {
        resize(0);
    }

    //! cut the slot in nc slots.
    //! \note sizes of the resulting slots are not garanted to be equal.
    //! \param nc number of slots
    //! \param s  result. Array of nc slot*.
    //inline void cut(int nc,slot* __restrict__ s[]) const
    inline auto cut(std::size_t const nc)
    {
        std::vector<slot> slarray(nc);

        sort();

        std::size_t sizec = size()/nc;
        for(std::size_t i=0; i<nc-1; ++i)
        {
            slarray[i].insert(slarray[i].begin(), begin()+i*sizec, begin()+(i+1)*sizec);
            slarray[i].s1 = (*this)[i*sizec].pos();
            slarray[i].s2 = (*this)[(i+1)*sizec-1].pos();
        }
        std::size_t i = nc-1;
        slarray[i].insert(slarray[i].begin(), begin()+i*sizec, end());
        slarray[i].s1 = (*this)[i*sizec].pos();
        slarray[i].s2 = (*this)[size()-1].pos();
        return slarray;
    }

    //! cut this slot in 2 slots, at position pos, and then shrink it.
    //! the returned slot is the first part containing v[0,pos[
    //! \param pos
    //! \param s2new value for s2 of the *new* slot, and s1 of this slot.
    //! \note we do not check that pos is correct, except if DEBUG is set.
    //! \note for s2new: position part only; tested only if DEBUG set.
    inline auto cutBefore(std::size_t pos, node_type s2new)
    {
        slot newslot(s1, s2new, capacity());
        newslot.insert(newslot.begin(), begin(), begin()+pos);

        erase(begin(), begin()+pos);
        s1 = s2new;
        return newslot;
    }

    //! fusion this slot with slot sl
    //! \param sl  slot.
    inline void fusion(const slot& sl)
    {
        s1 = std::min(s1, sl.s1);
        s2 = std::max(s2, sl.s2);
        put(sl);
    }

    //! sort by hash function.
    inline void sort()
    {
        std::sort(begin(), end(), [&](auto &n1, auto &n2){return n1.pos() < n2.pos();});
    }

    //! reallocate to reduce size;
    //! \param lim we reduce size if allocsize/size>= lim
    //! \note return True iff slot is reduced.
    inline bool cutdown(std::size_t lim=2)
    {
        bool ret=false;
        if(size()!=0 && capacity()/size()>=lim)
        {
            shrink_to_fit();
            reserve(2*size());
            ret = true;
        }
        return ret;
    }

    //! Suppress all bits used to mark something.
    inline void clearFreeBits()
    {
        std::for_each(begin(), end(), [](auto &n){ n.clearFreeBits(); });
    }

    //! suppress ex-aequo.
    //! \note slot must be sorted (tested only if DEBUG is set).
    inline void uniq()
    {
        auto last = std::unique(begin(), end());
        resize(std::distance(begin(), last));
    }

    //!test if all nodes have their abscissa between s1 and s2.
    //!\param throwexept throw an exception if true.
    inline bool testWellFormed(bool throwexept=true) const
    {
        auto index = std::find_if(cbegin(), cend(), [&](auto &n){return (n<s1 || n >=s2);});
        return (index == end())? true: false;
    }

    //! look for ex-aequo
    inline bool exaequo() const
    {
        auto index = std::adjacent_find(begin(), end());
        return (index == end())? false: true;
    }

    inline int startRank() const
    {
        return startrank;
    }

    //! set startrank
    //! \param r
    inline void setStartRank(std::size_t r)
    {
        startrank = r;
    }

    //! return slotrank.
    inline int slotRank() const
    {
        return slotrank;
    }

    //! set the slot rank
    //! \param r
    inline void setSlotRank(std::size_t r)
    {
        slotrank = r;
    }

    //! Write slot to a file, already open.
    //! _param f the file.
    void dump(std::ofstream& f)
    {
        f << size() << "\n";
        f << s2.value << "\n";
        f << s1.value << "\n";
        f << startrank << "\n";
        for_each(cbegin(), cend(), [&](auto &n){f << n.value << "\n";});
    }

    //! restore a slot from a dump.
    //! \param file the file to restore from.
    void restore(std::ifstream& f)
    {
        std::size_t ssize;
        node_value_type ss1;
        node_value_type ss2;
        f >> ssize;
        f >> ss2;
        f >> ss1;
        f >> startrank;
        s1 = ss1;
        s2 = ss2;
        for(std::size_t j=0; j<ssize; j++)
        {
            node_value_type N;
            f >> N;
            put(node_type{N});
        }
    }

};

template<std::size_t dim, typename node_value_type>
std::ostream& operator<<(std::ostream& os, const slot<dim, node_value_type>& sl)
{
    os << "slot\n";
    os << "s1: " << sl.s1 << "\n";
    os << "s2: " << sl.s2 << "\n";
    os << "size: " << sl.size() << "\n";
    os << "capacity: " << sl.capacity() << "\n";
    os << "startrank: " << sl.startRank() << "\n";
    os << "slotrank: " << sl.slotRank() << "\n";
    os << "hasvoidNodes: " << sl.hasvoidNodes() << "\n";
    return os;
}
