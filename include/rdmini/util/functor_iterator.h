#ifndef FUNCTOR_ITERATOR_H_
#define FUNCTOR_ITERATOR_H_

#include <utility>
#include <iterator>

/** Present a functor/lambda/function object as an output iterator */

namespace rdmini {

template <typename F>
class functor_iterator_adaptor {
    class proxy {
        friend class functor_iterator_adaptor<F>;
        F f;

        explicit proxy(const F &f_): f(f_) {}

    public:
        template <typename V>
        void operator=(V &&v) { f(std::forward<V>(v)); }
    } p;

public:
    explicit functor_iterator_adaptor(const F &f_): p(f_) {}
    
    // output iterator interface
    proxy &operator*() { return p; }
    functor_iterator_adaptor &operator++() { return *this; }
    functor_iterator_adaptor &operator++(int) { return *this; }

    typedef void value_type;
    typedef void difference_type;
    typedef void reference;
    typedef void pointer;
    typedef std::output_iterator_tag iterator_category;
};

template <typename F>
functor_iterator_adaptor<F> functor_iterator(const F &f) { return functor_iterator_adaptor<F>(f); }

}

#endif // ndef FUNCTOR_ITERATOR_H_
