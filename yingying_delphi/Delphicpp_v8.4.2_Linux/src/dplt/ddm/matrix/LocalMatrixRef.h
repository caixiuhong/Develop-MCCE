#ifndef DDM__MATRIX__LOCAL_MATRIX_REF_H__
#define DDM__MATRIX__LOCAL_MATRIX_REF_H__

#include "../dart-impl/dart.h"

#include "../../ddm/Team.h"
#include "../../ddm/Pattern.h"
#include "../../ddm/GlobRef.h"
#include "../../ddm/HView.h"

#include "../../ddm/iterator/GlobIter.h"
#include "../../ddm/iterator/GlobViewIter.h"

#include "../../ddm/Matrix.h"


namespace ddm {

/// Forward-declaration
template <
  typename T,
  dim_t NumDimensions,
  typename IndexT,
  class PatternT >
class Matrix;
/// Forward-declaration
template <
  typename T,
  dim_t NumDimensions,
  dim_t CUR,
  class PatternT >
class MatrixRef;
/// Forward-declaration
template <
  typename T,
  dim_t NumDimensions,
  dim_t CUR,
  class PatternT >
class LocalMatrixRef;

/**
 * Local part of a Matrix, provides local operations.
 *
 * \see DDMMatrixConcept
 *
 * \ingroup Matrix
 */
template <
  typename T,
  dim_t NumDimensions,
  dim_t CUR = NumDimensions,
  class PatternT =
    TilePattern<NumDimensions, ROW_MAJOR, ddm::default_index_t> >
class LocalMatrixRef
{
private:
  typedef LocalMatrixRef<T, NumDimensions, CUR, PatternT> self_t;

  typedef MatrixRefView<T, NumDimensions, PatternT>
    MatrixRefView_t;
  typedef std::array<typename PatternT::size_type, NumDimensions>
    Extents_t;
  typedef std::array<typename PatternT::index_type, NumDimensions>
    Offsets_t;
  typedef ddm::ViewSpec<NumDimensions,typename PatternT::index_type>
    ViewSpec_t;
  template <dim_t NumViewDim>
    using LocalMatrixRef_t =
          LocalMatrixRef<T, NumDimensions, NumViewDim, PatternT>;

public:
  template<
    typename T_,
    dim_t NumDimensions_,
    typename IndexT_,
    class PatternT_ >
  friend class Matrix;
  template<
    typename T_,
    dim_t NumDimensions1,
    dim_t NumDimensions2,
    class PatternT_ >
  friend class LocalMatrixRef;

public:
  typedef T                                                        value_type;
  typedef PatternT                                               pattern_type;
  typedef typename PatternT::index_type                            index_type;

  typedef typename PatternT::size_type                              size_type;
  typedef typename PatternT::index_type                       difference_type;

  typedef GlobViewIter<      value_type, PatternT>                   iterator;
  typedef GlobViewIter<const value_type, PatternT>             const_iterator;

  typedef std::reverse_iterator<iterator>                    reverse_iterator;
  typedef std::reverse_iterator<const_iterator>        const_reverse_iterator;

  typedef GlobRef<      value_type>                                 reference;
  typedef GlobRef<const value_type>                           const_reference;

  typedef GlobViewIter<      value_type, PatternT>                    pointer;
  typedef GlobViewIter<const value_type, PatternT>              const_pointer;

  typedef       T *                                             local_pointer;
  typedef const T *                                       const_local_pointer;

  typedef self_t                                                   local_type;

  template <dim_t NumViewDim>
    using view_type =
          LocalMatrixRef<T, NumDimensions, NumViewDim, PatternT>;

public:
  typedef std::integral_constant<dim_t, CUR>
    rank;

  static constexpr dim_t ndim() {
    return CUR;
  }

public:
  /**
   * Default constructor.
   */
  LocalMatrixRef<T, NumDimensions, CUR, PatternT>()
  {
    DDM_LOG_TRACE_VAR("LocalMatrixRef<T,D,C>()", NumDimensions);
    DDM_LOG_TRACE_VAR("LocalMatrixRef<T,D,C>()", CUR);
  }

  template <class T_>
  LocalMatrixRef<T, NumDimensions, CUR, PatternT>(
    const LocalMatrixRef<T_, NumDimensions, CUR+1, PatternT> & previous,
    index_type coord);

  /**
   * Constructor, creates a local view reference to a Matrix view.
   */
  template <class T_>
  LocalMatrixRef<T, NumDimensions, CUR, PatternT>(
    Matrix<T_, NumDimensions, index_type, PatternT> * mat
  );

  /**
   * View at local block at given local block coordinates.
   */
  LocalMatrixRef<T, NumDimensions, CUR, PatternT> block(
    const std::array<index_type, NumDimensions> & block_lcoords
  );

  /**
   * View at local block at given local block offset.
   */
  LocalMatrixRef<T, NumDimensions, CUR, PatternT> block(
    index_type block_lindex
  );

  inline operator LocalMatrixRef<T, NumDimensions, CUR-1, PatternT> && ();

  // SHOULD avoid cast from MatrixRef to LocalMatrixRef.
  // Different operation semantics.
  inline operator MatrixRef<T, NumDimensions, CUR, PatternT> ();

  inline    T                   & local_at(size_type pos);

  constexpr const Team          & team()                const noexcept;

  constexpr size_type             size()                const noexcept;
  constexpr size_type             local_size()          const noexcept;
  constexpr size_type             local_capacity()      const noexcept;
  inline    size_type             extent(dim_t dim)     const noexcept;
  constexpr Extents_t             extents()             const noexcept;
  inline    index_type            offset(dim_t dim)     const noexcept;
  constexpr Offsets_t             offsets()             const noexcept;
  constexpr bool                  empty()               const noexcept;

  /**
   * Synchronize units associated with the matrix.
   *
   * \see  DDMContainerConcept
   */
  inline    void                  barrier()             const;

  /**
   * The pattern used to distribute matrix elements to units in its
   * associated team.
   *
   * NOTE: This method is not implemented as local matrix views do
   *       not have a pattern. The pattern of the referenced matrix
   *       refers to the global data domain.
   */
  constexpr const PatternT      & pattern()             const;

  inline          iterator        begin()                     noexcept;
  constexpr const_iterator        begin()               const noexcept;
  inline          iterator        end()                       noexcept;
  constexpr const_iterator        end()                 const noexcept;

  inline          local_pointer   lbegin()                    noexcept;
  constexpr const_local_pointer   lbegin()              const noexcept;
  inline          local_pointer   lend()                      noexcept;
  constexpr const_local_pointer   lend()                const noexcept;

  /**
   * Fortran-style subscript operator.
   * As an example, the operation \c matrix(i,j) is equivalent to
   * \c matrix[i][j].
   *
   * \returns  A global reference to the element at the given global
   *           coordinates.
   */
  template<typename ... Args>
  inline T & at(
    /// Global coordinates
    Args... args);

  /**
   * Fortran-style subscript operator, alias for \c at().
   * As an example, the operation \c matrix(i,j) is equivalent to
   * \c matrix[i][j].
   *
   * \returns  A global reference to the element at the given global
   *           coordinates.
   * \see  at
   */
  template<typename... Args>
  inline T & operator()(
    /// Coordinates of element in global cartesian index space.
    Args... args);

  /**
   * Subscript assignment operator, access element at given offset
   * in global element range.
   */
  LocalMatrixRef<T, NumDimensions, CUR-1, PatternT>
    operator[](index_type n);

  /**
   * Subscript operator, access element at given offset in
   * global element range.
   */
  constexpr LocalMatrixRef<const T, NumDimensions, CUR-1, PatternT>
    operator[](index_type n) const;

  LocalMatrixRef<T, NumDimensions, NumDimensions-1, PatternT>
    col(size_type n);
  constexpr LocalMatrixRef<const T, NumDimensions, NumDimensions-1, PatternT>
    col(size_type n) const;

  LocalMatrixRef<T, NumDimensions, NumDimensions-1, PatternT>
    row(size_type n);
  constexpr LocalMatrixRef<const T, NumDimensions, NumDimensions-1, PatternT>
    row(size_type n) const;

  template<dim_t NumSubDimensions>
  LocalMatrixRef<T, NumDimensions, NumDimensions-1, PatternT>
    sub(size_type n);

  template<dim_t NumSubDimensions>
  LocalMatrixRef<const T, NumDimensions, NumDimensions-1, PatternT>
    sub(size_type n) const;
  
  template<dim_t SubDimension>
  LocalMatrixRef<T, NumDimensions, NumDimensions, PatternT>
    sub(size_type n,
        size_type range);

  template<dim_t SubDimension>
  LocalMatrixRef<const T, NumDimensions, NumDimensions, PatternT>
  sub(size_type n,
      size_type range) const;

  /**
   * Create a view representing the matrix slice within a row
   * range.
   * Same as \c sub<0>(offset, extent).
   *
   * \returns  A matrix local view
   *
   * \see  sub
   */
  LocalMatrixRef<T, NumDimensions, NumDimensions, PatternT>
  rows(
    /// Offset of first row in range
    size_type offset,
    /// Number of rows in the range
    size_type range);
  
  constexpr LocalMatrixRef<const T, NumDimensions, NumDimensions, PatternT>
  rows(
    /// Offset of first row in range
    size_type offset,
    /// Number of rows in the range
    size_type range) const;

  /**
   * Create a view representing the matrix slice within a column
   * range.
   * Same as \c sub<1>(offset, extent).
   *
   * \returns  A matrix local view
   *
   * \see  sub
   */
  LocalMatrixRef<T, NumDimensions, NumDimensions, PatternT>
  cols(
    /// Offset of first column in range
    size_type offset,
    /// Number of columns in the range
    size_type extent);
  
  constexpr LocalMatrixRef<const T, NumDimensions, NumDimensions, PatternT>
  cols(
    /// Offset of first column in range
    size_type offset,
    /// Number of columns in the range
    size_type extent) const;

private:
  MatrixRefView_t _refview;
};

/**
 * Local Matrix representation, provides local operations.
 * Partial Specialization for value deferencing.
 *
 * \ingroup Matrix
 */
template <
  typename T,
  dim_t NumDimensions,
  class PatternT >
class LocalMatrixRef<T, NumDimensions, 0, PatternT>
{
 private:
   typedef LocalMatrixRef<T, NumDimensions, 0, PatternT> self_t;

 public:
  template<
    typename T_,
    dim_t NumDimensions_,
    typename IndexT_,
    class PatternT_ >
  friend class Matrix;
  template<
    typename T_,
    dim_t NumDimensions1,
    dim_t NumDimensions2,
    class PatternT_ >
  friend class LocalMatrixRef;

 public:
  typedef typename PatternT::index_type  index_type;
  typedef typename PatternT::size_type   size_type;

 public:
  /**
   * Default constructor.
   */
  LocalMatrixRef<T, NumDimensions, 0, PatternT>()
  {
    DDM_LOG_TRACE_VAR("LocalMatrixRef<T,D,0>()", NumDimensions);
  }

  /**
   * Copy constructor.
   */
  template <class T_>
  LocalMatrixRef<T, NumDimensions, 0, PatternT>(
    const LocalMatrixRef<T_, NumDimensions, 0, PatternT> & other)
  : _refview(other._refview)
  {
    DDM_LOG_TRACE_VAR("LocalMatrixRef<T,D,0>(other)", NumDimensions);
  }

  template <class T_>
  LocalMatrixRef<T, NumDimensions, 0, PatternT>(
    const LocalMatrixRef<T_, NumDimensions, 1, PatternT> & previous,
    index_type coord);

  inline T * local_at(index_type pos) const;

  inline bool is_local() const {
    return true;
  }

  inline operator T() const;

  /**
   * Assignment operator.
   */
  inline T operator= (const T & value);
  inline T operator+=(const T & value);
  inline T operator+ (const T & value);
  inline T operator-=(const T & value);
  inline T operator- (const T & value);
  inline T operator*=(const T & value);
  inline T operator* (const T & value);
  inline T operator/=(const T & value);
  inline T operator/ (const T & value);

 private:
  MatrixRefView<T, NumDimensions, PatternT> _refview;
};

} // namespace ddm

#include "../../ddm/matrix/internal/LocalMatrixRef-inl.h"

#endif
