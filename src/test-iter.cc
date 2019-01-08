/*
 * Copyright © 2018  Google, Inc.
 *
 *  This is part of HarfBuzz, a text shaping library.
 *
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, modify, and distribute this
 * software and its documentation for any purpose, provided that the
 * above copyright notice and the following two paragraphs appear in
 * all copies of this software.
 *
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
 * ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN
 * IF THE COPYRIGHT HOLDER HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 *
 * THE COPYRIGHT HOLDER SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE COPYRIGHT HOLDER HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 *
 * Google Author(s): Behdad Esfahbod
 */

#include "hb.hh"
#include "hb-iter.hh"

#include "hb-array.hh"
#include "hb-set.hh"
#include "hb-ot-layout-common.hh"


template <typename T>
struct array_iter_t :
  hb_iter_t<array_iter_t<T>, T&>,
  hb_iter_mixin_t<array_iter_t<T>, T&>
{
  array_iter_t (hb_array_t<T> arr_) : arr (arr_) {}

  typedef T& __item_type__;
  enum { is_random_access_iterator = true };
  T& __item_at__ (unsigned i) const { return arr[i]; }
  void __forward__ (unsigned n) { arr += n; }
  void __rewind__ (unsigned n) { arr -= n; }
  unsigned __len__ () const { return arr.length; }

  private:
  hb_array_t<T> arr;
};

template <typename T>
struct some_array_t
{
  some_array_t (hb_array_t<T> arr_) : arr (arr_) {}

  typedef array_iter_t<T> iter_t;
  array_iter_t<T> iter () { return array_iter_t<T> (arr); }
  operator array_iter_t<T> () { return iter (); }
  operator hb_iter_t<array_iter_t<T> > () { return iter (); }

  private:
  hb_array_t<T> arr;
};


template <typename Iter> static
  hb_enable_if_t (hb_is_iterator (Iter),
void)
test_iterator (Iter it)
{
  Iter default_constructed;

  /* Iterate over a copy of it. */
  for (auto c = it.iter (); c; c++)
    *c;

  it += it.len ();
  it = it + 10;
  it = 10 + it;

  assert (*it == it[0]);

  if (it.is_random_access_iterator) {}
}

template <typename Iterable> static
  hb_enable_if_t (hb_is_iterable (Iterable),
void)
test_iterable (const Iterable &lst = Null(Iterable))
{
  // Test that can take iterator from.
  test_iterator (lst.iter ());
}

int
main (int argc, char **argv)
{
  const int src[10] = {};
  int dst[20];
  hb_vector_t<int> v;

  array_iter_t<const int> s (src); /* Implicit conversion from static array. */
  array_iter_t<const int> s2 (v); /* Implicit conversion from vector. */
  array_iter_t<int> t (dst);

  assert (hb_is_random_access_iterator (array_iter_t<int>));

  some_array_t<const int> a (src);

  s2 = s;

  hb_fill (t, 42);
  hb_copy (t, s);
 // hb_copy (t, a.iter ());

  test_iterable (v);
  hb_set_t st;
  test_iterable (st);
  hb_sorted_array_t<int> sa;
  test_iterable (sa);

  test_iterable<hb_array_t<int> > ();
  test_iterable<hb_sorted_array_t<const int> > ();
  test_iterable<hb_vector_t<float> > ();
  test_iterable<hb_set_t> ();
  test_iterable<OT::Coverage> ();

  hb_array_t<hb_vector_t<int> > pa;
  pa->as_array ();

  return 0;
}
