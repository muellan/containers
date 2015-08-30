/*****************************************************************************
 *
 * AM utilities
 *
 * released under MIT license
 *
 * 2008-2015 André Müller
 *
 *****************************************************************************/


#ifndef AMLIB_CONTAINERS_SPECIAL_VALUES_H_
#define AMLIB_CONTAINERS_SPECIAL_VALUES_H_


namespace am {



/*****************************************************************************
 *
 *
 *
 *****************************************************************************/
template<class T, int val>
struct static_value
{
    using value_type = T;

    static constexpr value_type value() { return value_type(val); }
};



} // namespace am


#endif
