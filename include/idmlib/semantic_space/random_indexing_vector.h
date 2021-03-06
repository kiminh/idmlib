#ifndef IDMLIB_SSP_RANDOMINDEXINGVECTOR_H_
#define IDMLIB_SSP_RANDOMINDEXINGVECTOR_H_


#include <string>
#include <iostream>
#include <idmlib/idm_types.h>
#include <am/matrix/sparse_vector.h>
#include <boost/serialization/access.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>
NS_IDMLIB_SSP_BEGIN

///@brief The random indexing vector generated by RandomIndexingGenerator
template <typename T>
class RandomIndexingVector
{
public:

  ///@brief Update the normal vector
  template <typename U, typename I>
  void AddTo(std::vector<U>& value_vector, I freq) const
  {
    for(uint32_t i=0;i<positive_position.size();i++)
    {
      value_vector[positive_position[i]] += freq;
    }
    for(uint32_t i=0;i<negative_position.size();i++)
    {
      value_vector[negative_position[i]] -= freq;
    }
  }
  
  ///@brief Update the sparse vector
  template <typename V, typename I, typename D>
  void AddTo(izenelib::am::SparseVector<V,T>& value_vector, I freq, D dimensions) const
  {
    std::vector<std::pair<T, V> > changed;
    for(uint32_t i=0;i<positive_position.size();i++)
    {
      changed.push_back(std::make_pair(positive_position[i], freq) );
    }
    for(uint32_t i=0;i<negative_position.size();i++)
    {
      V rev = (V)freq*-1;
      changed.push_back(std::make_pair(negative_position[i], rev) );
    }
    for(uint32_t i=0;i<value_vector.value.size();i++)
    {
      changed.push_back(value_vector.value[i] );
    }
    std::sort(changed.begin(), changed.end());
    T key = 0;
    V value = 0;
    value_vector.value.resize(0);
    for(uint32_t i=0;i<changed.size();i++)
    {
      T current_key = changed[i].first;
      if(current_key!=key && value!=0)
      {
        value_vector.value.push_back(std::make_pair(key, value) );
        value = 0;
      }
      value += changed[i].second;
      key = current_key;
    }
    if(value!=0)
    {
      value_vector.value.push_back(std::make_pair(key, value) );
    }
  }
  
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version)
  {
      ar & positive_position & negative_position;
  }
  
 
public: 
  std::vector<T> positive_position;
  std::vector<T> negative_position;
};

   
NS_IDMLIB_SSP_END



#endif 
