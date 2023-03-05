
#ifndef GEOARROW_GEOARROW_TYPES_H_INCLUDED
#define GEOARROW_GEOARROW_TYPES_H_INCLUDED

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Extra guard for versions of Arrow without the canonical guard
#ifndef ARROW_FLAG_DICTIONARY_ORDERED

#ifndef ARROW_C_DATA_INTERFACE
#define ARROW_C_DATA_INTERFACE

#define ARROW_FLAG_DICTIONARY_ORDERED 1
#define ARROW_FLAG_NULLABLE 2
#define ARROW_FLAG_MAP_KEYS_SORTED 4

struct ArrowSchema {
  // Array type description
  const char* format;
  const char* name;
  const char* metadata;
  int64_t flags;
  int64_t n_children;
  struct ArrowSchema** children;
  struct ArrowSchema* dictionary;

  // Release callback
  void (*release)(struct ArrowSchema*);
  // Opaque producer-specific data
  void* private_data;
};

struct ArrowArray {
  // Array data description
  int64_t length;
  int64_t null_count;
  int64_t offset;
  int64_t n_buffers;
  int64_t n_children;
  const void** buffers;
  struct ArrowArray** children;
  struct ArrowArray* dictionary;

  // Release callback
  void (*release)(struct ArrowArray*);
  // Opaque producer-specific data
  void* private_data;
};

#endif  // ARROW_C_DATA_INTERFACE

#ifndef ARROW_C_STREAM_INTERFACE
#define ARROW_C_STREAM_INTERFACE

struct ArrowArrayStream {
  // Callback to get the stream type
  // (will be the same for all arrays in the stream).
  //
  // Return value: 0 if successful, an `errno`-compatible error code otherwise.
  //
  // If successful, the ArrowSchema must be released independently from the stream.
  int (*get_schema)(struct ArrowArrayStream*, struct ArrowSchema* out);

  // Callback to get the next array
  // (if no error and the array is released, the stream has ended)
  //
  // Return value: 0 if successful, an `errno`-compatible error code otherwise.
  //
  // If successful, the ArrowArray must be released independently from the stream.
  int (*get_next)(struct ArrowArrayStream*, struct ArrowArray* out);

  // Callback to get optional detailed error information.
  // This must only be called if the last stream operation failed
  // with a non-0 return code.
  //
  // Return value: pointer to a null-terminated character array describing
  // the last error, or NULL if no description is available.
  //
  // The returned pointer is only valid until the next operation on this stream
  // (including release).
  const char* (*get_last_error)(struct ArrowArrayStream*);

  // Release callback: release the stream's own resources.
  // Note that arrays returned by `get_next` must be individually released.
  void (*release)(struct ArrowArrayStream*);

  // Opaque producer-specific data
  void* private_data;
};

#endif  // ARROW_C_STREAM_INTERFACE
#endif  // ARROW_FLAG_DICTIONARY_ORDERED

typedef int GeoArrowErrorCode;

#define GEOARROW_OK 0

struct GeoArrowStringView {
  const char* data;
  int64_t size_bytes;
};

struct GeoArrowBufferView {
  const uint8_t* data;
  int64_t size_bytes;
};

struct GeoArrowWritableBufferView {
  union {
    void* data;
    char* as_char;
    uint8_t* as_uint8;
    int32_t* as_int32;
    double* as_double;
  } data;
  int64_t size_bytes;
  int64_t capacity_bytes;
};

struct GeoArrowError {
  char message[1024];
};

enum GeoArrowType {
  GEOARROW_TYPE_UNINITIALIZED,

  GEOARROW_TYPE_WKB,
  GEOARROW_TYPE_LARGE_WKB,

  GEOARROW_TYPE_POINT,
  GEOARROW_TYPE_LINESTRING,
  GEOARROW_TYPE_POLYGON,
  GEOARROW_TYPE_MULTIPOINT,
  GEOARROW_TYPE_MULTILINESTRING,
  GEOARROW_TYPE_MULTIPOLYGON,

  GEOARROW_TYPE_POINT_Z,
  GEOARROW_TYPE_LINESTRING_Z,
  GEOARROW_TYPE_POLYGON_Z,
  GEOARROW_TYPE_MULTIPOINT_Z,
  GEOARROW_TYPE_MULTILINESTRING_Z,
  GEOARROW_TYPE_MULTIPOLYGON_Z,

  GEOARROW_TYPE_POINT_M,
  GEOARROW_TYPE_LINESTRING_M,
  GEOARROW_TYPE_POLYGON_M,
  GEOARROW_TYPE_MULTIPOINT_M,
  GEOARROW_TYPE_MULTILINESTRING_M,
  GEOARROW_TYPE_MULTIPOLYGON_M,

  GEOARROW_TYPE_POINT_ZM,
  GEOARROW_TYPE_LINESTRING_ZM,
  GEOARROW_TYPE_POLYGON_ZM,
  GEOARROW_TYPE_MULTIPOINT_ZM,
  GEOARROW_TYPE_MULTILINESTRING_ZM,
  GEOARROW_TYPE_MULTIPOLYGON_ZM
};

enum GeoArrowGeometryType {
  GEOARROW_GEOMETRY_TYPE_GEOMETRY = 0,
  GEOARROW_GEOMETRY_TYPE_POINT = 1,
  GEOARROW_GEOMETRY_TYPE_LINESTRING = 2,
  GEOARROW_GEOMETRY_TYPE_POLYGON = 3,
  GEOARROW_GEOMETRY_TYPE_MULTIPOINT = 4,
  GEOARROW_GEOMETRY_TYPE_MULTILINESTRING = 5,
  GEOARROW_GEOMETRY_TYPE_MULTIPOLYGON = 6,
  GEOARROW_GEOMETRY_TYPE_GEOMETRYCOLLECTION = 7
};

enum GeoArrowDimensions {
  GEOARROW_DIMENSIONS_UNKNOWN = 0,
  GEOARROW_DIMENSIONS_XY = 1,
  GEOARROW_DIMENSIONS_XYZ = 2,
  GEOARROW_DIMENSIONS_XYM = 3,
  GEOARROW_DIMENSIONS_XYZM = 4
};

enum GeoArrowCoordType {
  GEOARROW_COORD_TYPE_UNKNOWN = 0,
  GEOARROW_COORD_TYPE_SEPARATE = 1,
  GEOARROW_COORD_TYPE_INTERLEAVED = 2
};

enum GeoArrowEdgeType { GEOARROW_EDGE_TYPE_PLANAR, GEOARROW_EDGE_TYPE_SPHERICAL };

enum GeoArrowCrsType {
  GEOARROW_CRS_TYPE_NONE,
  GEOARROW_CRS_TYPE_UNKNOWN,
  GEOARROW_CRS_TYPE_PROJJSON
};

struct GeoArrowSchemaView {
  struct ArrowSchema* schema;
  struct GeoArrowStringView extension_name;
  struct GeoArrowStringView extension_metadata;
  enum GeoArrowType type;
  enum GeoArrowGeometryType geometry_type;
  enum GeoArrowDimensions dimensions;
  enum GeoArrowCoordType coord_type;
};

struct GeoArrowMetadataView {
  struct GeoArrowStringView metadata;
  enum GeoArrowEdgeType edge_type;
  enum GeoArrowCrsType crs_type;
  struct GeoArrowStringView crs;
};

struct GeoArrowCoordView {
  const double* values[4];
  int64_t n_coords;
  int32_t n_values;
  int32_t coords_stride;
};

struct GeoArrowWritableCoordView {
  double* values[4];
  int64_t size_coords;
  int64_t capacity_coords;
  int32_t n_values;
  int32_t coords_stride;
};

#define GEOARROW_COORD_VIEW_VALUE(coords_, row_, col_) \
  coords_->values[col_][row_ * coords_->coords_stride]

struct GeoArrowArrayView {
  struct GeoArrowSchemaView schema_view;
  int64_t length;
  const uint8_t* validity_bitmap;
  int32_t n_offsets;
  const int32_t* offsets[3];
  int32_t last_offset[3];
  struct GeoArrowCoordView coords;
};

struct GeoArrowWritableArrayView {
  struct GeoArrowSchemaView schema_view;
  int64_t length;
  int64_t n_buffers;
  struct GeoArrowWritableBufferView buffers[8];
  struct GeoArrowWritableCoordView coords;
};

struct GeoArrowBuilder {
  struct GeoArrowWritableArrayView view;
  void* private_data;
};

struct GeoArrowVisitor {
  int (*reserve_coord)(struct GeoArrowVisitor* v, int64_t n);
  int (*reserve_feat)(struct GeoArrowVisitor* v, int64_t n);

  int (*feat_start)(struct GeoArrowVisitor* v);
  int (*null_feat)(struct GeoArrowVisitor* v);
  int (*geom_start)(struct GeoArrowVisitor* v, enum GeoArrowGeometryType geometry_type,
                    enum GeoArrowDimensions dimensions);
  int (*ring_start)(struct GeoArrowVisitor* v);
  int (*coords)(struct GeoArrowVisitor* v, const struct GeoArrowCoordView* coords);
  int (*ring_end)(struct GeoArrowVisitor* v);
  int (*geom_end)(struct GeoArrowVisitor* v);
  int (*feat_end)(struct GeoArrowVisitor* v);

  struct GeoArrowError* error;

  void* private_data;
};

#ifdef __cplusplus
}
#endif

#endif
